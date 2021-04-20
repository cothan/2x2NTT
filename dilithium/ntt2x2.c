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

#define MAX(x, y) ((x < y) ? y : x);

void ntt2x2(bram *ram, bram *mul_ram, enum OPERATION mode, enum MAPPING decode)
{
    int32_t fifo_i[DEPT_I] = {0},
            fifo_a[DEPT_A] = {0},
            fifo_b[DEPT_B] = {0},
            fifo_c[DEPT_C] = {0},
            fifo_d[DEPT_D] = {0};
    int32_t a, b, c, d;
    int32_t fa, fb, fc, fd, fi;

    int i1, i2, i3, i4;
    int ram_i, bound, addr;

    int count = 0;
    int write_en = 0;

    const int w_m1 = 2;
    const int w_m2 = 1;

    int32_t w[4];

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
                if (k == 0 && mode != MUL_MODE)
                {
                    if (mode == INVERSE_NTT_MODE)
                    {
                        // NTT mode
                        // Layer s
                        i1 = (DILITHIUM_N >> s) - 1;
                        i2 = (DILITHIUM_N >> s) - 2;
                        // Layer s + 1
                        i4 = i3 = (DILITHIUM_N >> (s + 1)) - 1;
                    }
                }

                // printf("%d - %d - %d - %d\n", i1, i2, i3, i4);
                addr = k + j;
                if (decode == DECODE_TRUE)
                {
                    ram_i = addr_decoder(addr);
                }
                else
                {
                    ram_i = addr;
                }

                if (mode == INVERSE_NTT_MODE)
                {
                    // w1 = -zetas[i1];
                    // w2 = -zetas[i2];
                    // w3 = -zetas[i3];
                    // w4 = -zetas[i4];
                    w[0] = -zetas_barret[i1];
                    w[1] = -zetas_barret[i2];
                    w[2] = -zetas_barret[i3];
                    w[3] = -zetas_barret[i4];
                }
                else
                {
                    // MUL
                    read_ram(&w[0], &w[1], &w[2], &w[3], mul_ram, ram_i);
                }

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
                // // Decode only happen once
                // if (decode == DECODE_TRUE)
                // {
                //     printf("%d\n", k);
                // }
            }
        }
        decode = DECODE_FALSE;
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

int max(int a, int b)
{
    if (a < b)
        return b;
    return a;
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
    int32_t w[4];
    int ram_i, bound, addr;

    int count = 0;

    // --------------------
    int s_array[4] = {4, 2, 0, 4};
    int s;
    int start_ntt = 0;
    int32_t a, b, c, d;
    int32_t tb, tc;
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
        printf("FOFWARD NTT is not ready\n");
        break;

    default:
        printf("Error: MODE is wrong\n");
        bound = 0;
        break;
    }

    int once = 0;

    (void)mul_ram;
    (void)fw;
    (void)fi;
    (void)decode;
    int last = 0;

    int32_t line_in[4], line_out[4];

    for (int si = 0; si < bound; si += 2)
    {
        s = s_array[si >> 1];
        for (int j = 0; j < 1 << s; j++)
        {
            for (int k = 0; k < DILITHIUM_N / 4; k += 1 << s)
            {
                if (si < 6)
                {
                    if (k == 0 && mode == FORWARD_NTT_MODE)
                    {
                        i1_base = i2_base = 1 << si;
                        i3_base = (1 << (si + 1));
                        i4_base = (1 << (si + 1)) + 1;

                        i1 = i1_base;
                        i2 = i2_base;
                        i3 = i3_base;
                        i4 = i4_base;
                    }
                }
                else if (!once)
                {
                    i1_base = i2_base = 1 << si;
                    i3_base = (1 << (si + 1));
                    i4_base = (1 << (si + 1)) + 1;

                    i1 = i1_base;
                    i2 = i2_base;
                    i3 = i3_base;
                    i4 = i4_base;
                    once = 1;
                }

                if (mode == FORWARD_NTT_MODE)
                {
                    w[0] = zetas_barret[i1];
                    w[1] = zetas_barret[i2];
                    w[2] = zetas_barret[i3];
                    w[3] = zetas_barret[i4];

                    // w[0] = i1;
                    // w[1] = i2;
                    // w[2] = i3;
                    // w[3] = i4;
                }

                PIPO(line_out, DEPT_W * 4, fifo_w, w);

                // Load then store to FIFO
                addr = k + j;

                ram_i = addr;

                if (si >= 6)
                {
                    last++;
                }

                fi = FIFO(DEPT_W, fifo_i, ram_i);
                write_fifo(&a, &b, &c, &d, fifo_a, fifo_b, fifo_c, fifo_d, count, ram, ram_i);

                if (count == DEPT_W)
                    start_ntt = 1;

                count++;

                // Compute

                if (start_ntt)
                {
                    // printf("[%d] %d, %d, %d, %d\n", fi,  a, b, c, d);
                    // print_array(line_out, 4, "hls w:");
                    buttefly_circuit(&a, &b, &c, &d, line_out[0], line_out[1], line_out[2], line_out[3], FORWARD_NTT_MODE);
                    // 
                    
                    if (last > 4)
                    {
                        tb = b;
                        tc = c;
                    }
                    else
                    {
                        tb = b;
                        tc = c;
                    }
                    write_ram(ram, fi, a, tb, tc, d);  
                }

                // printf("w: %2d - %2d - %2d - %2d\n", i1, i2, i3, i4);

                // print_array(fifo_d, DEPT_D, "FIFO_A");
                // print_array(fifo_c, DEPT_C, "FIFO_B");
                // print_array(fifo_b, DEPT_B, "FIFO_C");
                // print_array(fifo_a, DEPT_A, "FIFO_D");

                // print_array(fifo_i, DEPT_W, "FIFO_I");
                // print_array(fifo_w, DEPT_W, "FIFO_W");
                // printf("--------%d\n", ram_i);

                // printf("=====================\n\n");

                if (mode == FORWARD_NTT_MODE)
                {
                    int mask1 = (2 << si) - 1;
                    int mask2 = (2 << (si + 1)) - 1;
                    if (si < 6)
                    {
                        i1 = max(i1_base, (i1 + 1) & mask1);
                        i2 = max(i2_base, (i2 + 1) & mask1);
                        i3 = max(i3_base, (i3 + 2) & mask2);
                        i4 = max(i4_base, (i4 + 2) & mask2);
                    }
                    else
                    {
                        // printf("mask: %d - %d\n", mask1, mask2);
                        i1 = (i1 + 1) & mask1;
                        i2 = (i2 + 1) & mask1;
                        i3 = (i3 + 2) & mask2;
                        i4 = (i4 + 2) & mask2;
                    }
                }
            }
            // printf("=====================\n");
        }
    }
    for (int i = 0; i < DEPT_W; i++)
    {
        fi = FIFO(DEPT_W, fifo_i, 0);
        PIPO(line_out, DEPT_W * 4, fifo_w, w);
        write_fifo(&a, &b, &c, &d, fifo_a, fifo_b, fifo_c, fifo_d, count, ram, ram_i);
        buttefly_circuit(&a, &b, &c, &d, line_out[0], line_out[1], line_out[2], line_out[3], FORWARD_NTT_MODE);
        // Writeback
        write_ram(ram, fi, a, b, c, d);
    }

    // for (int i = 0; i < DEPT_I+1; i++)
    // {
    //     printf("=====END\n");
    //     move_fifo(&fa, &fb, &fc, &fd, &fi, &fw,
    //                 fifo_a, fifo_b, fifo_c, fifo_d,
    //                 fifo_i, ram_i, fifo_w, w);
    //     printf("%d, %d, %d, %d\n", fa, fb, fc, fd);

    // print_array(fifo_d, DEPT_D, "FIFO_A");
    // print_array(fifo_c, DEPT_C, "FIFO_B");
    // print_array(fifo_b, DEPT_B, "FIFO_C");
    // print_array(fifo_a, DEPT_A, "FIFO_D");

    // print_array(fifo_i, DEPT_W, "FIFO_I");
    // print_array(fifo_w, DEPT_W, "FIFO_W");
        // printf("--------%d\n", fi);
    // }
    printf("==========\n\n\n");
}

/* 
Correct 2 layers
 */