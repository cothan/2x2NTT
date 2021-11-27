#include "params.h"
#include <stdio.h>
#include <stdint.h>

/* 
 * Figure out how to compute address decoder/encoder on fly. 
 * However, for N=256, it's better to use table-based approach since it's fit in 1 LUT. 
 * The only computation should take more than LUT. 
 * Modulo can be replace by AND operation, divide can be replace by right shift as well. 
 */
int resolve_address(enum MAPPING mapping, int addr)
{
    int ram_i;
    const int f = FALCON_N/4/4;
    switch (mapping)
    {
    case DECODE_TRUE:
        // ram_i = addr_decoder(addr);
        ram_i = (addr % f)*4 + addr/f;
        break;

    case ENCODE_TRUE:
        // ram_i = addr_encoder(addr);
        ram_i = (addr % 4)*f + addr/4;
        break;

    default:
        ram_i = addr;
        break;
    }
    return ram_i;
}

void resolve_twiddle(uint16_t tw_i[4], uint16_t *last, uint16_t tw_base_i[4],
                    const int k, const int s, enum OPERATION mode)
{
    int l1, l2, l3, l4;
    int l1_base, l2_base, l3_base, l4_base;
    int t_last = *last;
    if (mode == INVERSE_NTT_MODE)
    {
        l1_base = (FALCON_N >> s) - 1;
        l2_base = (FALCON_N >> s) - 2;
        l3_base = l4_base = (FALCON_N >> (s + 1)) - 1;

        // INVERSE_NTT_MODE
        // Layer s
        l1 = l1_base;
        l2 = l2_base;
        // Layer s + 1
        l3 = l3_base;
        l4 = l4_base;

        if (k == 0)
        {
            tw_i[0] = l1;
            tw_i[1] = l2;
            tw_i[2] = l3;
            tw_i[3] = l4;

            tw_base_i[0] = l1_base;
            tw_base_i[1] = l2_base;
            tw_base_i[2] = l3_base;
            tw_base_i[3] = l4_base;
        }
    }
    else if (mode == FORWARD_NTT_MODE)
    {
        l1_base = l2_base = 1 << s;
        l3_base = (1 << (s + 1));
        l4_base = (1 << (s + 1)) + 1;

        // FORWARD_NTT_MODE
        // Layer s
        l1 = l1_base;
        l2 = l2_base;
        // Layer s + 1
        l3 = l3_base;
        l4 = l4_base;

        if (s < 6 && k == 0)
        {
            tw_i[0] = l1;
            tw_i[1] = l2;
            tw_i[2] = l3;
            tw_i[3] = l4;

            tw_base_i[0] = l1_base;
            tw_base_i[1] = l2_base;
            tw_base_i[2] = l3_base;
            tw_base_i[3] = l4_base;
        }
        // FORWARD_NTT_MODE
        else if (s >= 6 && !t_last)
        {
            *last = 1;

            tw_i[0] = l1;
            tw_i[1] = l2;
            tw_i[2] = l3;
            tw_i[3] = l4;

            tw_base_i[0] = l1_base;
            tw_base_i[1] = l2_base;
            tw_base_i[2] = l3_base;
            tw_base_i[3] = l4_base;
        }
    }
}
