#ifndef NTT_H
#define NTT_H

#include <stdint.h>
#include "params.h"

void ntt(uint16_t a[FALCON_N]);

void invntt(uint16_t a[FALCON_N]);

void pointwise_montgomery(uint16_t *c, uint16_t *a, const uint16_t *b);

#endif
