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
    int write = 0;

    const int w_m1 = 2;
    const int w_m2 = 1;

    int32_t w1, w2, w3, w4;

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
                    w1 = -zetas_barret[i1];
                    w2 = -zetas_barret[i2];
                    w3 = -zetas_barret[i3];
                    w4 = -zetas_barret[i4];
                }
                else
                {
                    // MUL
                    read_ram(&w1, &w2, &w3, &w4, mul_ram, ram_i);
                }

                read_ram(&a, &b, &c, &d, ram, ram_i);

                // 2x2 circuit
                buttefly_circuit(&a, &b, &c, &d, w1, w2, w3, w4, mode);

                // FIFO
                FIFO(DEPT_A, fifo_a, a);
                FIFO(DEPT_B, fifo_b, b);
                FIFO(DEPT_C, fifo_c, c);
                FIFO(DEPT_D, fifo_d, d);
                fi = FIFO(DEPT_I, fifo_i, ram_i);

                if (count == DEPT_I)
                {
                    // FIFO_A is full
                    write = 1;
                    count = 0;
                }
                else
                {
                    count++;
                }

                if (write)
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


#define DEPT_W 4

void forward_ntt2x2(bram *ram, bram *mul_ram, enum OPERATION mode, enum MAPPING decode)
{
    int32_t fifo_i[DEPT_W] = {0},
            fifo_a[DEPT_A] = {0},
            fifo_b[DEPT_B] = {0},
            fifo_c[DEPT_C] = {0},
            fifo_d[DEPT_D] = {0},
            fifo_w[DEPT_W] = {0};

    int32_t fa, fb, fc, fd, fi, fw;

    // int i1, i2, i3, i4;
    int ram_i, bound, addr;

    int count = 0;

    // --------------------
    int s_array[4] = {4, 2, 0, 4};
    int start_ntt = 0;
    // int32_t a, b, c, d;
    // a = -1;
    // b = -1;
    // c = -1;
    // d = -1;
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
        bound = DILITHIUM_LOGN;
        printf("FOFWARD NTT is not ready\n");
        break;

    default:
        printf("Error: MODE is wrong\n");
        bound = 0;
        break;
    }

    (void) mul_ram;
    (void) fw; 
    (void) fi;
    (void) decode;

    for (int si = 0; si < bound; ++si)
    {
        for (int j = 0; j < 1 << s_array[si]; j++)
        {
            for (int k = 0; j < DILITHIUM_N / 4; j += 1 << s_array[si])
            {
                
                // Load then store to FIFO
                addr = k + j;

                ram_i = addr;

                fa = FIFO(DEPT_A, fifo_a, -1);
                fb = FIFO(DEPT_B, fifo_b, -1);
                fc = FIFO(DEPT_C, fifo_c, -1);
                fd = FIFO(DEPT_D, fifo_d, -1);
                // fi = FIFO(DEPT_W, fifo_i, -1);
                // fw = FIFO(DEPT_W, fifo_w, -1);

                write_fifo(fifo_a, fifo_b, fifo_c, fifo_d, count, ram, ram_i);

                start_ntt = count == DEPT_W;
                
                count++;
                

                // Compute

                if (start_ntt)
                {
                    printf("%d, %d, %d, %d\n", fa, fb, fc, fd);
                    printf("--------\n");
                }

                // printf("w: %2d - %2d - %2d - %2d\n", i1, i2, i3, i4);

                print_array(fifo_d, DEPT_D, "FIFO_A");
                print_array(fifo_c, DEPT_C, "FIFO_B");
                print_array(fifo_b, DEPT_B, "FIFO_C");
                print_array(fifo_a, DEPT_A, "FIFO_D");
                print_array(fifo_i, DEPT_W, "FIFO_I");
                print_array(fifo_w, DEPT_W, "FIFO_W");
                printf("--------%d\n", ram_i);

                // printf("=====================\n\n");
            }
        }
    }

    // for (int i = 0; i < DEPT_I+1; i++)
    // {
    //     printf("=====END\n");
    //     move_fifo(&fa, &fb, &fc, &fd, &fi, &fw,
    //                 fifo_a, fifo_b, fifo_c, fifo_d, 
    //                 fifo_i, ram_i, fifo_w, w);
    //     printf("%d, %d, %d, %d\n", fa, fb, fc, fd);


    //     // print_array(fifo_d, DEPT_D, "FIFO_A");
    //     // print_array(fifo_c, DEPT_C, "FIFO_B");
    //     // print_array(fifo_b, DEPT_B, "FIFO_C");
    //     // print_array(fifo_a, DEPT_A, "FIFO_D");
    //     printf("--------%d\n", ram_i);
    // }
}