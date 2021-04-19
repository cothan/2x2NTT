#include "ram_util.h"

void read_ram(u24 *a, u24 *b, u24 *c, u24 *d,
              const u96 ram[DILITHIUM_N/4], const int index)
{
    (*a, *b, *c, *d) = ram[index];
    // *a = ram->vec[ram_i].coeffs[0];
    // *b = ram->vec[ram_i].coeffs[1];
    // *c = ram->vec[ram_i].coeffs[2];
    // *d = ram->vec[ram_i].coeffs[3];
}

void write_ram(u96 ram[DILITHIUM_N/4], const int index,
               const u24 a, const u24 b,
               const u24 c, const u24 d)
{
    ram[index] = (a, b, c, d);
    // ram->vec[ram_i].coeffs[0] = a;
    // ram->vec[ram_i].coeffs[1] = b;
    // ram->vec[ram_i].coeffs[2] = c;
    // ram->vec[ram_i].coeffs[3] = d;
}

void read_twiddle(u24 *a, u24 *b, u24 *c, u24 *d,
              const u24 twiddle[DILITHIUM_N],
              const int i1, const int i2, 
              const int i3, const int i4)
{
    *a = twiddle[i1];
    *b = twiddle[i2];
    *c = twiddle[i3];
    *d = twiddle[i4];
}