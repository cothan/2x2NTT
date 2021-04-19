#include "params.h"
#include "fifo.h"
#include "butterfly_unit.h"
#include "ram_util.h"
#include "mytypes.h"
#include "consts.h"

void ntt2x2(u96 ram[DILITHIUM_N / 4], u96 mul_ram[DILITHIUM_N / 4], enum OPERATION mode, enum MAPPING decode)
{
    static u24  fifo_i[DEPT_I] = {0},
                fifo_a[DEPT_A] = {0},
                fifo_b[DEPT_B] = {0},
                fifo_c[DEPT_C] = {0},
                fifo_d[DEPT_D] = {0};
    const int w_m1 = 2;
    const int w_m2 = 1;

    // Twiddle factor value
    u24 w1, w2, w3, w4;
    // Value read from RAM 
    u24 a, b, c, d;
    // Value write to RAM
    u24 fa, fb, fc, fd, fi;
    bool write_en = false;

    int i1, i2, i3, i4;
    int ram_index, bound, addr;

    int count = 0;

    switch (mode)
    {
    case MUL_MODE:
        bound = 1;
        break;

    case INVERSE_NTT_MODE:
        bound = DILITHIUM_LOGN;
        break;

    case FORWARD_NTT_MODE:
        bound = 0;
        break;

    default:
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
#pragma HLS LOOP_FLATTEN
                if (k == 0)
                {
                    if (mode == INVERSE_NTT_MODE)
                    {
                        // INV_NTT mode
                        // Layer s
                        i1 = (DILITHIUM_N >> s) - 1;
                        i2 = (DILITHIUM_N >> s) - 2;
                        // Layer s + 1
                        i4 = i3 = (DILITHIUM_N >> (s + 1)) - 1;
                    }
                    else if (mode == FORWARD_NTT_MODE)
                    {
                        // FORWARD_NTT_MODE
                        // #error "FORWARD_NTT_MODE is not ready"
                    }
                }

                addr = k + j;
                ram_index = (decode == DECODE_TRUE) ? addr_decoder(addr) : addr;

                if (mode == MUL_MODE)
                {
                    // MUL_MODE
                    read_ram(&w1, &w2, &w3, &w4, mul_ram, ram_index);
                }
                else
                {
                    read_twiddle(&w1, &w2, &w3, &w4, zetas_barret, i1, i2, i3, i4);
                }

                read_ram(&a, &b, &c, &d, ram, ram_index);

                // 2x2 circuit
                buttefly_circuit(&a, &b, &c, &d, w1, w2, w3, w4, mode);

                // FIFO
                FIFO(DEPT_A, fifo_a, a);
                FIFO(DEPT_B, fifo_b, b);
                FIFO(DEPT_C, fifo_c, c);
                FIFO(DEPT_D, fifo_d, d);
                fi = FIFO(DEPT_I, fifo_i, ram_index);


                // FIFO_A is full
                write_en = count == DEPT_I;

                // Reset counter when count == DEPT_I, happen once
                count = (count == DEPT_I) ? 0 : count++;

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
                else
                {
                    // #error "FORWARD_NTT is not ready"
                }
                // Decode only happen once
                decode = 0;
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