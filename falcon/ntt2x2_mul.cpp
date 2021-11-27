#include "params.h"
#include "ntt.h"
#include "ntt2x2.h"
#include "util.h"
#include "fifo.h"
#include "consts.h"
#include "address_encoder_decoder.h"
#include "ram_util.h"
#include "butterfly_unit.h"

/* Point-wise multiplication
 * Input: ram, mul_ram, mapping
 * Output: ram
 */
void ntt2x2_mul(bram *ram, const bram *mul_ram, enum MAPPING mapping)
{
    int ram_i;
    uint16_t data_in[4], data_out[4];
    uint16_t w1, w2, w3, w4;

    for (auto l = 0; l < FALCON_N / 4; ++l)
    {
        ram_i = resolve_address(mapping, l);

        // Read address from RAM
        read_ram(&data_in[0], &data_in[1],
                 &data_in[2], &data_in[3],
                 ram, ram_i);

        // Read address from MUL_RAM
        read_ram(&w1, &w2, &w3, &w4, mul_ram, l);

        // Send to butterfly circuit
        buttefly_circuit(data_out, data_in,
                             w2, w4, w1, w3, MUL_MODE);

        // Write back
        write_ram(ram, ram_i,
                  data_out[0], data_out[1],
                  data_out[2], data_out[3]);
    }
}
