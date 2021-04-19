#ifndef RAM_UTIL_H
#define RAM_UTIL_H

#include "mytypes.h"
#include "params.h"

void read_ram(u24 *a, u24 *b, u24 *c, u24 *d,
              const u96 ram[DILITHIUM_N/4], const int index);

void write_ram(u96 ram[DILITHIUM_N/4], const int index,
               const u24 a, const u24 b,
               const u24 c, const u24 d);

void read_twiddle(u24 *a, u24 *b, u24 *c, u24 *d,
              const u24 twiddle[DILITHIUM_N],
              const int i1, const int i2, 
              const int i3, const int i4);

#endif