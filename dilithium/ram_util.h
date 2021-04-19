#ifndef RAM_UTIL_H
#define RAM_UTIL_H

#include <stdint.h>
#include "ntt2x2.h"

void read_ram(int32_t *a, int32_t *b, 
              int32_t *c, int32_t *d,
              const bram *ram, const int ram_i);

void write_ram(bram *ram, const int ram_i,
               const int32_t a, const int32_t b,
               const int32_t c, const int32_t d);

#endif 