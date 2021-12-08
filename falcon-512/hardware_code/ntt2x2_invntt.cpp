#include "../params.h"
#include "ntt2x2.h"
#include "ram_util.h"
#include "butterfly_unit.h"
#include "fifo.h"
#include "address_encoder_decoder.h"
#include "util.h"
#include <cstdio>
#include <cstring>


/* Inverse NTT
 * Input: ram, zetas_barret, mode, mapping
 * Output: ram 
 */
void ntt2x2_invntt(bram *ram, enum OPERATION mode, enum MAPPING mapping)
{
    // Initialize FIFO
    data_t fifo_i[DEPT_W] = {0};
    data_t fifo_a[DEPT_A] = {0};
    data_t fifo_b[DEPT_B] = {0};
    data_t fifo_c[DEPT_C] = {0};
    data_t fifo_d[DEPT_D] = {0};
    // data_t fa, fb, fc, fd;

    // Initialize Forward NTT
#if FALCON_MODE == 5
    unsigned fw_ntt_pattern[] = {6, 4, 2, 0, 6};
#elif FALCON_MODE == 1
    unsigned fw_ntt_pattern[] = {5, 3, 1, 6, 5};
#elif FALCON_MODE == 0
    unsigned fw_ntt_pattern[] = {4, 2, 0, 4};
#else
#error "FALCON_MODE supports [0,1,5]"
#endif
    unsigned s, last = 0;

    // Initialize twiddle
    unsigned tw_i[4], tw_base_i[4];
    data_t w_in[4];

    // Intialize index
    data_t k, j;

    // Initialize coefficients
    const data_t null[4] = {0};
    data_t data_in[4] = {0},
           data_fifo[4] = {0},
           data_out[4] = {0};

    // Initialize writeback
    unsigned count; // 2-bit counter
    bool write_en = false;

    // Bypass loop
    count = 3;
    for (unsigned l = 0; l < (FALCON_LOGN & 1); l++)
    {
        for (unsigned i = 0; i < BRAM_DEPT; ++i)
        {
            /* ============================================== */

            if (i == 0)
            {
                k = j = 0;
            }

            /* ============================================== */

            unsigned addr = k + j;

            // Prepare address
            unsigned ram_i = resolve_address(mapping, addr);

            // Read ram by address
            // printf("\naddr = %u\n", ram_i);
            read_ram(data_in, ram, ram_i);

            // Prepare twiddle
            resolve_twiddle(tw_i, &last, tw_base_i, k, l, mode);

            // Read twiddle
            read_twiddle(w_in, mode, tw_i);

            /* ============================================== */

            // Calculate
            buttefly_circuit<data2_t, data_t>(data_out, data_in,
                                              w_in, INVERSE_NTT_MODE_BYPASS);

            /* ============================================== */
            unsigned fi = FIFO<DEPT_I - 1>(fifo_i, ram_i);
            // Replace by single write FIFO, null as output since we don't care about output
            // Rolling FIFO and extract data
            count = (count + 2) & 3;
            read_write_fifo<data_t>(INVERSE_NTT_MODE_BYPASS, data_fifo, null, data_out, fifo_a,
                                    fifo_b, fifo_c, fifo_d, count);

            /* ============================================== */
            // Conditional
            if (i == DEPT_I - 1)
            {
                write_en = true;
            }

            // Write back
            if (write_en)
            {
                write_ram(ram, fi, data_fifo);
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
            if (k + (1 << s) < BRAM_DEPT)
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

    for (unsigned i = 0; i < DEPT_I - 1; i++)
    {
        /* ============================================== */
        // Emptying FIFO
        // Rolling FIFO index
        unsigned fi = FIFO<DEPT_I - 1>(fifo_i, 0);

        // Rolling the FIFO and extract data from FIFO
        count = (count + 2) & 3;
        read_write_fifo<data_t>(INVERSE_NTT_MODE_BYPASS, data_fifo, null, null, fifo_a, fifo_b, fifo_c, fifo_d, count);

        /* ============================================== */
        // Write back
        write_ram(ram, fi, data_fifo);
    }

    // Normal loop
    count = 0;
    for (unsigned l = (FALCON_LOGN & 1); l < FALCON_LOGN; l += 2)
    {
        for (unsigned i = 0; i < BRAM_DEPT; ++i)
        {
            /* ============================================== */

            if (i == 0)
            {
                k = j = 0;
            }

            /* ============================================== */

            unsigned addr = k + j;

            // Prepare address
            unsigned ram_i = resolve_address(mapping, addr);

            // Read ram by address
            read_ram(data_in, ram, ram_i);

            // Prepare twiddle
            resolve_twiddle(tw_i, &last, tw_base_i, k, l, mode);

            // Read twiddle
            read_twiddle(w_in, mode, tw_i);

            /* ============================================== */

            // Calculate
            buttefly_circuit<data2_t, data_t>(data_out, data_in, w_in, mode);

            /* ============================================== */
            // Rolling FIFO index
            unsigned fi = FIFO<DEPT_I>(fifo_i, ram_i);

            // Replace by single write FIFO, null as output since we don't care about output
            // Rolling FIFO and extract data
            count = (count + 1) & 3;
            read_write_fifo<data_t>(mode, data_fifo, null, data_out, fifo_a,
                                    fifo_b, fifo_c, fifo_d, count);

            /* ============================================== */
            // Conditional

            // Write back
            if (write_en)
            {
                write_ram(ram, fi, data_fifo);
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
            if (k + (1 << s) < BRAM_DEPT)
            {
                k += (1 << s);
            }
            else
            {
                k = 0;
                ++j;
            }
            if (l == 8) printf("%u addr = %u\n", l, addr);
            update_indexes(tw_i, tw_base_i, l, mode);
        }
        /* ============================================== */
    }

    for (unsigned i = 0; i < DEPT_I; i++)
    {
        /* ============================================== */
        // Emptying FIFO
        // Rolling FIFO index
        unsigned fi = FIFO<DEPT_I>(fifo_i, 0);

        // Rolling the FIFO and extract data from FIFO
        count = (count + 1) & 3;
        read_write_fifo<data_t>(mode, data_fifo, null, null, fifo_a, fifo_b, fifo_c, fifo_d, count);

        /* ============================================== */
        // Write back
        write_ram(ram, fi, data_fifo);
    }
}
