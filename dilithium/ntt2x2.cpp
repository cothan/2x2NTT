#include <stdio.h>
#include "params.h"
#include "ntt.h"
#include "ntt2x2.h"
#include "util.h"
#include "fifo.h"
#include "consts.h"
#include "address_encoder_decoder.h"
#include "ram_util.h"
#include "butterfly_unit.h"

template <typename T>
const T MAX(const T a, const T b)
{
    return (a < b) ? b : a; // or: return comp(a,b)?b:a; for version (2)
}

void twiddle_resolve(int *i1, int *i2,
                     int *i3, int *i4, int *last_ntt_level,
                     int32_t *i1_base, int32_t *i2_base,
                     int32_t *i3_base, int32_t *i4_base,
                     int k, int s, enum OPERATION mode)
{
    int l1, l2, l3, l4;
    int l1_base, l2_base, l3_base, l4_base;
    int t_last = *last_ntt_level;
    if (mode == INVERSE_NTT_MODE)
    {
        l1_base = (DILITHIUM_N >> s) - 1;
        l2_base = (DILITHIUM_N >> s) - 2;
        l3_base = l4_base = (DILITHIUM_N >> (s + 1)) - 1;

        // INVERSE_NTT_MODE
        // Layer s
        l1 = l1_base;
        l2 = l2_base;
        // Layer s + 1
        l3 = l3_base;
        l4 = l4_base;

        if (k == 0)
        {
            *i1 = l1;
            *i2 = l2;
            *i3 = l3;
            *i4 = l4;

            *i1_base = l1_base;
            *i2_base = l2_base;
            *i3_base = l3_base;
            *i4_base = l4_base;
        }
    }
    else if (mode == FORWARD_NTT_MODE)
    {
        l1_base = l2_base = 1 << s;
        l3_base = (1 << (s + 1));
        l4_base = (1 << (s + 1)) + 1;

        l1 = l1_base;
        l2 = l2_base;
        l3 = l3_base;
        l4 = l4_base;

        if (s < 6 && k == 0)
        {
            *i1 = l1;
            *i2 = l2;
            *i3 = l3;
            *i4 = l4;

            *i1_base = l1_base;
            *i2_base = l2_base;
            *i3_base = l3_base;
            *i4_base = l4_base;
        }
        // FORWARD_NTT_MODE
        else if (s >= 6 && !t_last)
        {
            *last_ntt_level = 1;

            *i1 = l1;
            *i2 = l2;
            *i3 = l3;
            *i4 = l4;

            *i1_base = l1_base;
            *i2_base = l2_base;
            *i3_base = l3_base;
            *i4_base = l4_base;
        }
    }
}

void get_twiddle(int32_t w[4], enum OPERATION mode,
                 const bram *mul_ram, const int index,
                 const int i1, const int i2,
                 const int i3, const int i4)
{
    switch (mode)
    {
    case FORWARD_NTT_MODE:
        w[0] = zetas_barret[i1];
        w[1] = zetas_barret[i2];
        w[2] = zetas_barret[i3];
        w[3] = zetas_barret[i4];
        break;

    case MUL_MODE:
        read_ram(&w[0], &w[1], &w[2], &w[3], mul_ram, index);
        break;

    case INVERSE_NTT_MODE:
        w[0] = -zetas_barret[i1];
        w[1] = -zetas_barret[i2];
        w[2] = -zetas_barret[i3];
        w[3] = -zetas_barret[i4];
        break;

    default:
        // Not support
        break;
    }
}

void update_indexes(int *i1, int *i2, int *i3, int *i4,
                    const int i1_base, const int i2_base,
                    const int i3_base, const int i4_base,
                    const int s, enum OPERATION mode)
{
    int mask1, mask2;
    const int w_m1 = 2;
    const int w_m2 = 1;
    int l1, l2, l3, l4;

    mask1 = (2 << s) - 1;
    mask2 = (2 << (s + 1)) - 1;

    l1 = *i1;
    l2 = *i2;
    l3 = *i3;
    l4 = *i4;

    // Adjust address
    if (mode == INVERSE_NTT_MODE)
    {
        // Only adjust omega in NTT mode
        l1 -= w_m1;
        l2 -= w_m1;
        l3 -= w_m2;
        l4 -= w_m2;
    }
    else if (mode == FORWARD_NTT_MODE)
    {
        if (s < 6)
        {
            l1 = MAX<int>(i1_base, (l1 + 1) & mask1);
            l2 = MAX<int>(i2_base, (l2 + 1) & mask1);
            l3 = MAX<int>(i3_base, (l3 + 2) & mask2);
            l4 = MAX<int>(i4_base, (l4 + 2) & mask2);
        }
        else
        {
            l1 += w_m2;
            l2 += w_m2;
            l3 += w_m1;
            l4 += w_m1;
        }
    }
    *i1 = l1;
    *i2 = l2;
    *i3 = l3;
    *i4 = l4;
}

void ntt2x2(bram *ram, bram *mul_ram, enum OPERATION mode, enum MAPPING mapping)
{
    int32_t fifo_i[DEPT_W] = {0},
            fifo_a[DEPT_A] = {0},
            fifo_b[DEPT_B] = {0},
            fifo_c[DEPT_C] = {0},
            fifo_d[DEPT_D] = {0},
            fifo_w[4 * DEPT_W] = {0};
    int32_t a, b, c, d;
    int32_t fa, fb, fc, fd, fi;
    int32_t wa, wb, wc, wd;

    int i1, i2, i3, i4;
    int i1_base, i2_base, i3_base, i4_base;
    int ram_i, bound, addr;

    int count = 0, fw_count = 0;
    int write_en = 0;
    int last_ntt_level = 0;

    int32_t w[4], w_pipo[4];
    int fw_ntt_array[4] = {4, 2, 0, 4};
    int inv_ntt_array[4] = {0, 2, 4, 6};
    int *distance_array;
    int s;

    switch (mode)
    {
    case MUL_MODE:
        bound = 1;
        distance_array = inv_ntt_array;
        break;

    case INVERSE_NTT_MODE:
        bound = DILITHIUM_LOGN;
        distance_array = inv_ntt_array;
        break;

    case FORWARD_NTT_MODE:
        bound = DILITHIUM_LOGN;
        distance_array = fw_ntt_array;
        break;

    default:
        printf("Error: MODE is wrong\n");
        bound = 0;
        break;
    }

    // iterate 2 levels at a time
    for (int l = 0; l < bound; l += 2)
    {
        s = distance_array[l >> 1];
        for (int j = 0; j < (1 << s); j++)
        {
            for (int k = 0; k < DILITHIUM_N / 4; k += 1 << s)
            {
                addr = k + j;
                ram_i = resolve_address(mapping, addr);

                twiddle_resolve(&i1, &i2, &i3, &i4, &last_ntt_level,
                                &i1_base, &i2_base, &i3_base, &i4_base,
                                k, s, mode);
                get_twiddle(w, mode, mul_ram, ram_i, i1, i2, i3, i4);

                // Buffer w for Forward NTT
                PIPO<DEPT_W * 4>(w_pipo, fifo_w, w);
                fi = FIFO_I(DEPT_W, fifo_i, ram_i, mode);

                read_ram(&a, &b, &c, &d, ram, ram_i);

                if (count == DEPT_I)
                {
                    write_en = 1;
                    count = 0;
                }
                else
                {
                    count++;
                }

                // Main circuit
                buttefly_circuit(&a, &b, &c, &d, w_pipo, w, mode);

                // Writeback for Inverse NTT
                // FIFO
                write_fifo(&a, &b, &c, &d, fifo_a, fifo_b, fifo_c, fifo_d, count, mode, ram, ram_i);
                read_fifo(&fa, &fb, &fc, &fd, count, mode, fifo_a, fifo_b, fifo_c, fifo_d);

                if (write_en)
                {
                    wa = fa;
                    wb = fb;
                    wc = fc;
                    wd = fd;
                    write_ram(ram, fi, wa, wb, wc, wd);
                }

                // Adjust address
                update_indexes(&i1, &i2, &i3, &i4, i1_base, i2_base, i3_base, i4_base, s, mode);
            }
        }
    }
    // Write back left over coefficients in FIFO
    for (int i = 0; i < DEPT_I; i++)
    {
        fi = FIFO_I(DEPT_W, fifo_i, 0, mode);
        PIPO<DEPT_W * 4>(w_pipo, fifo_w, w);

        write_fifo(&a, &b, &c, &d, fifo_a, fifo_b, fifo_c, fifo_d, count, mode, ram, ram_i);
        count++;

        read_fifo(&a, &b, &c, &d, count, mode, fifo_a, fifo_b, fifo_c, fifo_d);

        write_ram(ram, fi, a, b, c, d);
    }
}

void forward_ntt2x2(bram *ram, bram *mul_ram, enum OPERATION mode, enum MAPPING decode)
{
    int32_t fifo_i[DEPT_W] = {0},
            fifo_a[DEPT_A] = {0},
            fifo_b[DEPT_B] = {0},
            fifo_c[DEPT_C] = {0},
            fifo_d[DEPT_D] = {0},
            fifo_w[4 * DEPT_W] = {0};

    // int32_t fa, fb, fc, fd;
    int32_t fi, fw;

    int i1, i2, i3, i4;
    int i1_base, i2_base, i3_base, i4_base;
    int32_t w[4], w_pipo[4];
    int ram_i, bound, addr;

    int fw_count = 0;

    // --------------------
    int s_array[4] = {4, 2, 0, 4};
    int si;
    int write_en = 0;
    int32_t a, b, c, d;
    // --------------------

    switch (mode)
    {
    case MUL_MODE:
        bound = 1;
        break;

    case INVERSE_NTT_MODE:
        bound = DILITHIUM_LOGN;
        break;

    case FORWARD_NTT_MODE:
        bound = 8;
        break;

    default:
        printf("Error: MODE is wrong\n");
        bound = 0;
        break;
    }

    int last_ntt_level = 0;

    (void)mul_ram;
    (void)fw;
    (void)fi;
    (void)decode;

    for (int s = 0; s < bound; s += 2)
    {
        si = s_array[s >> 1];
        for (int j = 0; j < 1 << si; j++)
        {
            for (int k = 0; k < DILITHIUM_N / 4; k += 1 << si)
            {
                addr = k + j;
                ram_i = addr;

                twiddle_resolve(&i1, &i2, &i3, &i4, &last_ntt_level,
                                &i1_base, &i2_base, &i3_base, &i4_base,
                                k, s, mode);

                get_twiddle(w, mode, mul_ram, ram_i, i1, i2, i3, i4);

                write_fifo(&a, &b, &c, &d, fifo_a, fifo_b, fifo_c,
                           fifo_d, fw_count, mode, ram, ram_i);

                PIPO<DEPT_W * 4>(w_pipo, fifo_w, w);
                fi = FIFO_I(DEPT_W, fifo_i, ram_i, mode);

                if (fw_count == DEPT_W)
                {
                    write_en = 1;
                }
                fw_count++;

                // Compute
                buttefly_circuit(&a, &b, &c, &d, w_pipo, w, mode);

                if (write_en)
                {
                    write_ram(ram, fi, a, b, c, d);
                }

                update_indexes(&i1, &i2, &i3, &i4, i1_base, i2_base,
                               i3_base, i4_base, s, mode);
            }
        }
    }
    for (int i = 0; i < DEPT_W; i++)
    {
        fi = FIFO<DEPT_W>(fifo_i, 0);
        PIPO<DEPT_W * 4>(w_pipo, fifo_w, w);
        write_fifo(&a, &b, &c, &d, fifo_a, fifo_b, fifo_c, fifo_d, fw_count, mode, ram, ram_i);
        buttefly_circuit(&a, &b, &c, &d, w_pipo, w, mode);
        write_ram(ram, fi, a, b, c, d);
    }
}
