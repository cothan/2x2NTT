#include <stdint.h>
#include "ntt2x2.h"

void read_ram(int32_t *a, int32_t *b, int32_t *c, int32_t *d,
              const bram *ram, const int ram_i)
{
    *a = ram->vec[ram_i].coeffs[0];
    *b = ram->vec[ram_i].coeffs[1];
    *c = ram->vec[ram_i].coeffs[2];
    *d = ram->vec[ram_i].coeffs[3];
}

void write_ram(bram *ram, const int ram_i,
               const int32_t a, const int32_t b,
               const int32_t c, const int32_t d)
{
    ram->vec[ram_i].coeffs[0] = a;
    ram->vec[ram_i].coeffs[1] = b;
    ram->vec[ram_i].coeffs[2] = c;
    ram->vec[ram_i].coeffs[3] = d;
}
