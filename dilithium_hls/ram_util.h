#ifndef RAM_UTIL_H
#define RAM_UTIL_H

#include "mytypes.h"
#include "params.h"

void read_ram(u24 *a, u24 *b, u24 *c, u24 *d,
              const i96 ram[DILITHIUM_N/4], const int index);

void write_ram(i96 ram[DILITHIUM_N/4], const int index,
               const u24 a, const u24 b,
               const u24 c, const u24 d);

#endif