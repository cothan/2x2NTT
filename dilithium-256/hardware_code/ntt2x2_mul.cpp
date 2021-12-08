#include "config.h"
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
    data_t data_in[4], data_out[4];
    data_t w_in[4], w_out[4];

    for (unsigned l = 0; l < BRAM_DEPT; ++l)
    {
        ram_i = resolve_address(mapping, l);

        // Read address from RAM
        read_ram(data_in, ram, ram_i);

        // Read address from MUL_RAM
        read_ram(w_in, mul_ram, l);
        w_out[0] = w_in[1];
        w_out[1] = w_in[3];
        w_out[2] = w_in[0];
        w_out[3] = w_in[2];

        // Send to butterfly circuit
        buttefly_circuit<data2_t, data_t>(data_out, data_in, w_out, MUL_MODE);

        // Write back
        write_ram(ram, ram_i, data_out);
    }
}
