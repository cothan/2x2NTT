#ifndef RAM_UTIL_H
#define RAM_UTIL_H

#include "config.h"

void read_ram(data_t data_out[4], const bram *ram, const unsigned ram_i);

void write_ram(bram *ram, const unsigned ram_i, const data_t data_in[4]);

void read_twiddle(data_t data_out[4], const unsigned tw_i[4]);

#endif
