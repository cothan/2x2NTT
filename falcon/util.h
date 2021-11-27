#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include "params.h"
#include "ntt2x2.h"

void print_array(uint16_t *a, int bound, const char *string);

void print_reshaped_array(bram *ram, int bound, const char *string);

void reshape(bram *ram, const uint16_t in[FALCON_N]);

int compare_array(uint16_t *a, uint16_t *b, int bound);

int compare_bram_array(bram *ram, uint16_t array[FALCON_N], const char *string, enum MAPPING mapping, int print_out);

#endif
