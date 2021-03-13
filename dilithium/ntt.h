#ifndef NTT_H
#define NTT_H

#include <stdint.h>
#include "params.h"

#define ntt DILITHIUM_NAMESPACE(ntt)
void ntt(int32_t a[N]);

#define invntt_tomont DILITHIUM_NAMESPACE(invntt_tomont)
void invntt_tomont(int32_t a[N]);

#define pointwise_montgomery DILITHIUM_NAMESPACE(pointwise_montgomery)
void pointwise_montgomery(int32_t *c, int32_t *a, int32_t *b);

#endif
