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


/* Inverse NTT
 * Input: ram, zetas_barret, mode, mapping
 * Output: ram 
 */
void ntt2x2_invntt(bram *ram, enum OPERATION mode, enum MAPPING mapping)
{
    // Initialize FIFO
    int32_t fifo_i[DEPT_W] = {0};
    int32_t fifo_a[DEPT_A] = {0};
    int32_t fifo_b[DEPT_B] = {0};
    int32_t fifo_c[DEPT_C] = {0};
    int32_t fifo_d[DEPT_D] = {0};
    // int32_t fa, fb, fc, fd;

    // Initialize Forward NTT
    int32_t fw_ntt_pattern[4] = {4, 2, 0, 4};
    int32_t s, last = 0;

    // Initialize twiddle
    int32_t tw_i[4], tw_base_i[4];
    int32_t w1, w2, w3, w4;

    // Intialize index
    int32_t k, j;

    // Initialize coefficients
    int32_t data_in[4] = {0},
            data_fifo[4] = {0},
            data_out[4] = {0},
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

            // Prepare twiddle
            resolve_twiddle(tw_i, &last, tw_base_i, k, l, mode);

            // Read twiddle
            read_twiddle(&w1, &w2, &w3, &w4, mode, tw_i);

            /* ============================================== */

            // Calculate
            buttefly_circuit(data_out, data_in,
                             w1, w2, w3, w4, mode);

            /* ============================================== */
            // Rolling FIFO index
            auto fi = FIFO<DEPT_I>(fifo_i, ram_i);

            // Write data_out to FIFO A, B, C, D
            // FIFO_PISO<DEPT_A, int32_t>(fifo_a, false, data_fifo, data_out[0]);
            // FIFO_PISO<DEPT_B, int32_t>(fifo_b, false, data_fifo, data_out[1]);
            // FIFO_PISO<DEPT_C, int32_t>(fifo_c, false, data_fifo, data_out[2]);
            // FIFO_PISO<DEPT_D, int32_t>(fifo_d, false, data_fifo, data_out[3]);

            // Replace by single write FIFO, null as output since we don't care about output
            // Rolling FIFO and extract data
            count = (count + 1) & 3;
            read_write_fifo<int32_t>(mode, data_fifo, null, data_out, fifo_a,
                                     fifo_b, fifo_c, fifo_d, count);

            /* ============================================== */
            // Conditional
            if (count == 0 && i != 0)
            {
                write_en = true;
            }

            // Write back
            if (write_en)
            {
                write_ram(ram, fi, data_fifo[0], data_fifo[1],
                          data_fifo[2], data_fifo[3]);
            }

            /* ============================================== */
            if (mode == FORWARD_NTT_MODE)
            {
                s = fw_ntt_pattern[l >> 1];
            }
            else
            {
                s = l;
            }

            // Update loop
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

    for (auto i = 0; i < DEPT_I; i++)
    {
        // #pragma HLS PIPELINE II = 1

        /* ============================================== */
        // Emptying FIFO
        // Rolling FIFO index
        auto fi = FIFO<DEPT_I>(fifo_i, 0);

        // Write data_out to FIFO A, B, C, D
        // FIFO<DEPT_A>(fifo_a, 0);
        // FIFO<DEPT_B>(fifo_b, 0);
        // FIFO<DEPT_C>(fifo_c, 0);
        // FIFO<DEPT_D>(fifo_d, 0);

        // Rolling the FIFO and extract data from FIFO
        count = (count + 1) & 3;
        read_write_fifo<int32_t>(mode, data_fifo, null, null, fifo_a, fifo_b, fifo_c, fifo_d, count);

        /* ============================================== */
        // Write back
        write_ram(ram, fi, data_fifo[0], data_fifo[1], data_fifo[2], data_fifo[3]);

        // printf("--------------\n");
        // print_array(fifo_i, DEPT_I, "FIFO_I");
        // print_array(fifo_a, DEPT_A, "FIFO_A");
        // print_array(fifo_b, DEPT_B, "FIFO_B");
        // print_array(fifo_c, DEPT_C, "FIFO_C");
        // print_array(fifo_d, DEPT_D, "FIFO_D");
    }
}