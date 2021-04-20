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

int MAX(int a, int b)
{
    return (a < b) ? b : a;
}

void twiddle_resolve(int32_t *i1, int *i2,
                     int *i3, int *i4, int *last_ntt_level,
                     int32_t *i1_base, int32_t *i2_base,
                     int32_t *i3_base, int32_t *i4_base,
                     int k, int s, enum OPERATION mode)
{
    if (mode == INVERSE_NTT_MODE)
    {
        if (k == 0)
        {
            *i1_base = *i2_base = *i3_base = *i4_base = 0;
            // INVERSE_NTT_MODE
            // Layer s
            *i1 = (DILITHIUM_N >> s) - 1;
            *i2 = (DILITHIUM_N >> s) - 2;
            // Layer s + 1
            *i4 = *i3 = (DILITHIUM_N >> (s + 1)) - 1;
        }
    }
    else if (mode == FORWARD_NTT_MODE)
    {
        // FORWARD_NTT_MODE
        if (k == 0 && s < 6)
        {
            *i1_base = *i2_base = 1 << s;
            *i3_base = (1 << (s + 1));
            *i4_base = (1 << (s + 1)) + 1;

            *i1 = *i1_base;
            *i2 = *i2_base;
            *i3 = *i3_base;
            *i4 = *i4_base;
        }
        else if (s >= 6 && !last_ntt_level)
        {
            *i1_base = *i2_base = 1 << s;
            *i3_base = (1 << (s + 1));
            *i4_base = (1 << (s + 1)) + 1;

            *i1 = *i1_base;
            *i2 = *i2_base;
            *i3 = *i3_base;
            *i4 = *i4_base;
            *last_ntt_level = 1;
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

void ntt2x2(bram *ram, bram *mul_ram, enum OPERATION mode, enum MAPPING mapping)
{
    int32_t fifo_i[DEPT_I] = {0},
            fifo_a[DEPT_A] = {0},
            fifo_b[DEPT_B] = {0},
            fifo_c[DEPT_C] = {0},
            fifo_d[DEPT_D] = {0},
            fifo_w[DEPT_W] = {0};
    int32_t a, b, c, d;
    int32_t fa, fb, fc, fd, fi;

    int i1, i2, i3, i4;
    int i1_base, i2_base, i3_base, i4_base;
    int ram_i, bound, addr;

    int count = 0;
    int write_en = 0;
    int last_ntt_level = 0;
    int mask1, mask2;

    const int w_m1 = 2;
    const int w_m2 = 1;

    int32_t w[4], w_pipo[4];

    switch (mode)
    {
    case MUL_MODE:
        bound = 1;
        break;

    case INVERSE_NTT_MODE:
        bound = DILITHIUM_LOGN;
        break;

    case FORWARD_NTT_MODE:
        bound = DILITHIUM_LOGN;
        printf("FOFWARD NTT is not ready\n");
        break;

    default:
        printf("Error: MODE is wrong\n");
        bound = 0;
        break;
    }

    // iterate 2 levels at a time
    for (int s = 0; s < bound; s += 2)
    {
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

                read_ram(&a, &b, &c, &d, ram, ram_i);

                // 2x2 circuit
                buttefly_circuit(&a, &b, &c, &d, w[0], w[1], w[2], w[3], mode);

                // FIFO
                FIFO(DEPT_A, fifo_a, a);
                FIFO(DEPT_B, fifo_b, b);
                FIFO(DEPT_C, fifo_c, c);
                FIFO(DEPT_D, fifo_d, d);
                fi = FIFO(DEPT_I, fifo_i, ram_i);

                if (count == DEPT_I)
                {
                    // FIFO_A is full
                    write_en = 1;
                    count = 0;
                }
                else
                {
                    count++;
                }

                if (write_en)
                {
                    read_fifo(&fa, &fb, &fc, &fd, count, mode, fifo_a, fifo_b, fifo_c, fifo_d);
                    write_ram(ram, fi, fa, fb, fc, fd);

                    // writeback
                    // printf("[%d] <= (%d, %d, %d, %d)\n", fi, fa, fb, fc, fd);
                }
                if (mode == INVERSE_NTT_MODE)
                {
                    // Only adjust omega in NTT mode
                    i1 -= w_m1;
                    i2 -= w_m1;
                    i3 -= w_m2;
                    i4 -= w_m2;
                }
                else if (mode == FORWARD_NTT_MODE)
                {
                    mask1 = (2 << s) - 1;
                    mask2 = (2 << (s + 1)) - 1;
                    if (s < 6)
                    {
                        i1 = MAX(i1_base, (i1 + 1) & mask1);
                        i2 = MAX(i2_base, (i2 + 1) & mask1);
                        i3 = MAX(i3_base, (i3 + 2) & mask2);
                        i4 = MAX(i4_base, (i4 + 2) & mask2);
                    }
                    else
                    {
                        i1 += w_m1;
                        i2 += w_m1;
                        i3 += w_m2;
                        i4 += w_m2;
                    }
                }
            }
        }
    }
    // Write back left over coefficients in FIFO
    for (int i = 0; i < DEPT_I; i++)
    {
        fi = FIFO(DEPT_I, fifo_i, 0);
        FIFO(DEPT_A, fifo_a, 0);
        FIFO(DEPT_B, fifo_b, 0);
        FIFO(DEPT_C, fifo_c, 0);
        FIFO(DEPT_D, fifo_d, 0);
        count++;

        read_fifo(&fa, &fb, &fc, &fd, count, mode, fifo_a, fifo_b, fifo_c, fifo_d);
        write_ram(ram, fi, fa, fb, fc, fd);
        // writeback
        // printf("[%d] <= (%d, %d, %d, %d)\n", fi, fa, fb, fc, fd);
    }
    /* // For debugging purpose 
    print_array(fifo_i, DEPT_I, "fifo_i");
    print_array(fifo_a, DEPT_A, "fifo_a");
    print_array(fifo_b, DEPT_B, "fifo_b");
    print_array(fifo_c, DEPT_C, "fifo_c");
    print_array(fifo_d, DEPT_D, "fifo_d"); 
    */
}

#define DEPT_W 4

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

    int count = 0;

    // --------------------
    int s_array[4] = {4, 2, 0, 4};
    int si;
    int start_ntt = 0;
    int32_t a, b, c, d;
    int mask1, mask2;
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

                if (mode == FORWARD_NTT_MODE)
                {
                    if (s < 6 && k == 0)
                    {
                        i1_base = i2_base = 1 << s;
                        i3_base = (1 << (s + 1));
                        i4_base = (1 << (s + 1)) + 1;

                        i1 = i1_base;
                        i2 = i2_base;
                        i3 = i3_base;
                        i4 = i4_base;
                    }
                    else if (s >= 6 && !last_ntt_level)
                    {
                        i1_base = i2_base = 1 << s;
                        i3_base = (1 << (s + 1));
                        i4_base = (1 << (s + 1)) + 1;

                        i1 = i1_base;
                        i2 = i2_base;
                        i3 = i3_base;
                        i4 = i4_base;
                        last_ntt_level = 1;
                    }
                }

                if (mode == FORWARD_NTT_MODE)
                {
                    w_pipo[0] = zetas_barret[i1];
                    w_pipo[1] = zetas_barret[i2];
                    w_pipo[2] = zetas_barret[i3];
                    w_pipo[3] = zetas_barret[i4];
                }

                PIPO(w, DEPT_W * 4, fifo_w, w_pipo);

                fi = FIFO(DEPT_W, fifo_i, ram_i);
                write_fifo(&a, &b, &c, &d, fifo_a, fifo_b, fifo_c, fifo_d, count, ram, ram_i);

                if (count == DEPT_W)
                    start_ntt = 1;

                count++;

                // Compute

                if (start_ntt)
                {
                    buttefly_circuit(&a, &b, &c, &d, w[0], w[1], w[2], w[3], mode);
                    write_ram(ram, fi, a, b, c, d);
                }

                if (mode == FORWARD_NTT_MODE)
                {
                    mask1 = (2 << s) - 1;
                    mask2 = (2 << (s + 1)) - 1;
                    if (s < 6)
                    {
                        i1 = MAX(i1_base, (i1 + 1) & mask1);
                        i2 = MAX(i2_base, (i2 + 1) & mask1);
                        i3 = MAX(i3_base, (i3 + 2) & mask2);
                        i4 = MAX(i4_base, (i4 + 2) & mask2);
                    }
                    else
                    {
                        i1 += 1;
                        i2 += 1;
                        i3 += 2;
                        i4 += 2;
                    }
                }
            }
        }
    }
    for (int i = 0; i < DEPT_W; i++)
    {
        fi = FIFO(DEPT_W, fifo_i, 0);
        PIPO(w, DEPT_W * 4, fifo_w, w_pipo);
        write_fifo(&a, &b, &c, &d, fifo_a, fifo_b, fifo_c, fifo_d, count, ram, ram_i);
        buttefly_circuit(&a, &b, &c, &d, w[0], w[1], w[2], w[3], FORWARD_NTT_MODE);
        write_ram(ram, fi, a, b, c, d);
    }
}
