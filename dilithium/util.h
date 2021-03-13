#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include "params.h"
#include "ntt2x2.h"

void print_array(int32_t *a, int bound, const char *string);

void print_reshaped_array(bram *ram, int bound, const char *string);

void reshape(bram *ram, int32_t in[N]);

#endif