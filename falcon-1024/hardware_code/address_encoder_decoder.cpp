#include <stdio.h>
#include <stdint.h>
#include "config.h"

/* 
 * Figure out how to compute address decoder/encoder on fly. 
 * However, for N=256, it's better to use table-based approach since it's fit in 1 LUT. 
 * The only computation should take more than LUT. 
 * Modulo can be replace by AND operation, divide can be replace by right shift as well. 
 */
unsigned resolve_address(enum MAPPING mapping, unsigned addr)
{
    unsigned ram_i;
    const unsigned f = FALCON_N >> 4;
    switch (mapping)
    {
    case AFTER_INVNTT:
        // This can be implemented with shift and mask
        ram_i = (addr % f)*4 + addr/f;
        break;

    case AFTER_NTT:
        // This can be implemented with shift and mask
        ram_i = (addr % 4)*f + addr/4;
        break;

    case NATURAL:
        ram_i = addr;
        break;
    }
    return ram_i;
}
