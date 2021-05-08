#include <stdint.h>
#include "ntt2x2.h"

void read_ram(int32_t *a, int32_t *b, int32_t *c, int32_t *d,
              const bram *ram, const int ram_i)
{
    *a = ram->coeffs[ram_i][0];
    *b = ram->coeffs[ram_i][1];
    *c = ram->coeffs[ram_i][2];
    *d = ram->coeffs[ram_i][3];
}

void write_ram(bram *ram, const int ram_i,
               const int32_t a, const int32_t b,
               const int32_t c, const int32_t d)
{
    ram->coeffs[ram_i][0] = a;
    ram->coeffs[ram_i][1] = b;
    ram->coeffs[ram_i][2] = c;
    ram->coeffs[ram_i][3] = d;
}
