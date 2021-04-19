#ifndef NTT_H
#define NTT_H

#include <stdint.h>
#include "params.h"

void ntt(int32_t a[DILITHIUM_N]);

void invntt_tomont(int32_t a[DILITHIUM_N]);

void pointwise_montgomery(int32_t *c, int32_t *a, const int32_t *b);

#endif
