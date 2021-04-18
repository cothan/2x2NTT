#include "ram_util.h"

void read_ram(u24 *a, u24 *b, u24 *c, u24 *d,
              const i96 ram[DILITHIUM_N/4], const int index)
{
    (*a, *b, *c, *d) = ram[index];
    // *a = ram->vec[ram_i].coeffs[0];
    // *b = ram->vec[ram_i].coeffs[1];
    // *c = ram->vec[ram_i].coeffs[2];
    // *d = ram->vec[ram_i].coeffs[3];
}

void write_ram(i96 ram[DILITHIUM_N/4], const int index,
               const u24 a, const u24 b,
               const u24 c, const u24 d)
{
    ram[index] = (a, b, c, d);
    // ram->vec[ram_i].coeffs[0] = a;
    // ram->vec[ram_i].coeffs[1] = b;
    // ram->vec[ram_i].coeffs[2] = c;
    // ram->vec[ram_i].coeffs[3] = d;
}