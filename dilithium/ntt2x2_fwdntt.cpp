#include "params.h"
#include "ntt.h"
#include "ntt2x2.h"
#include "util.h"
#include "fifo.h"
#include "consts.h"
#include "address_encoder_decoder.h"
#include "ram_util.h"
#include "butterfly_unit.h"
#include <cstdio>
#include <cstring>

/* Forward NTT
 * Input: ram, zetas_barret, mode, mapping
 * Output: ram 
 */
void ntt2x2_fwdntt(bram *ram, enum OPERATION mode, enum MAPPING mapping)
{
    // Initialize FIFO
    int32_t fifo_i[DEPT_W] = {0};
    int32_t fifo_a[DEPT_A] = {0};
    int32_t fifo_b[DEPT_B] = {0};
    int32_t fifo_c[DEPT_C] = {0};
    int32_t fifo_d[DEPT_D] = {0};
    int32_t fifo_w[DEPT_W][DEPT_W] = {0};

    // Initialize Forward NTT
    int32_t fw_ntt_pattern[4] = {4, 2, 0, 4};
    int32_t s, last = 0;

    // Initialize twiddle
    int32_t tw_i[4] = {0}, tw_base_i[4] = {0};
    int32_t w1, w2, w3, w4;
    int32_t w[4];

    // Intialize index
    int32_t k, j;

    // Initialize coefficients
    int32_t data_in[4] = {0},
            data_out[4] = {0},
            data_fifo[4] = {0},
            null[4] = {0};

    // Initialize writeback
    auto count = 0;
    bool write_en = false;

    for (auto l = 0; l < DILITHIUM_LOGN; l += 2)
    {
        for (auto i = 0; i < DILITHIUM_N / 4; ++i)
        {
            // #pragma HLS LOOP_FLATTEN
            // #pragma HLS PIPELINE II = 1
            /* ============================================== */

            if (i == 0)
            {
                k = j = 0;
            }

            /* ============================================== */

            auto addr = k + j;

            // Prepare address
            auto ram_i = resolve_address(mapping, addr);

            // Read ram by address
            read_ram(&data_in[0], &data_in[1],
                     &data_in[2], &data_in[3],
                     ram, ram_i);

            // Write data_in to FIFO, extract output to data_fifo
            // In this mode, new_value[4] = null[4]
            read_write_fifo<int32_t>(mode, data_fifo, data_in, null, fifo_a,
                                     fifo_b, fifo_c, fifo_d, count);
            count = (count + 1) & 3;

            // Prepare twiddle
            resolve_twiddle(tw_i, &last, tw_base_i, k, l, mode);

            // Read Twiddle
            read_twiddle(&w1, &w2, &w3, &w4, mode, tw_i);

            /* ============================================== */
            // Rolling FIFO
            auto fi = FIFO<DEPT_W>(fifo_i, ram_i);

            // printf("--------------%d\n", count);
            // print_array(tw_i, 4, "twiddle");
            // print_array(fifo_i, DEPT_W, "FIFO_I");
            // print_array(fifo_a, DEPT_A, "FIFO_A");
            // print_array(fifo_b, DEPT_B, "FIFO_B");
            // print_array(fifo_c, DEPT_C, "FIFO_C");
            // print_array(fifo_d, DEPT_D, "FIFO_D");
            // print_array(data_in, 4, "data_in");

            PIPO<DEPT_W, int32_t>(w, fifo_w, w1, w2, w3, w4);
            /* ============================================== */

            // Calculate
            buttefly_circuit(data_out, data_fifo,
                             w[0], w[1], w[2], w[3], mode);

            /* ============================================== */
            // count equal the size of FIFO_I
            if (count == 0 && i != 0)
            {
                write_en = true;
            }

            if (write_en)
            {
                write_ram(ram, fi, data_out[0], data_out[1],
                          data_out[2], data_out[3]);
            }

            /* ============================================== */
            // Update loop
            if (mode == FORWARD_NTT_MODE)
            {
                s = fw_ntt_pattern[l >> 1];
            }
            else
            {
                s = l;
            }

            if (k + (1 << s) < DILITHIUM_N / 4)
            {
                k += (1 << s);
            }
            else
            {
                k = 0;
                ++j;
            }

            update_indexes(tw_i, tw_base_i, l, mode);
        }
        /* ============================================== */
    }

    for (auto i = 0; i < DEPT_W; i++)
    {
        // Extract left over data in FIFO to data_in
        read_write_fifo<int32_t>(mode, data_in, null, null, fifo_a,
                                 fifo_b, fifo_c, fifo_d, count);

        // Rolling FIFO
        auto fi = FIFO<DEPT_W>(fifo_i, 0);

        // Buffer twiddle
        PIPO<DEPT_W>(w, fifo_w, 0, 0, 0, 0);

        buttefly_circuit(data_out, data_in,
                         w[0], w[1], w[2], w[3], mode);

        // Write back
        write_ram(ram, fi, data_out[0], data_out[1],
                  data_out[2], data_out[3]);
    }
}