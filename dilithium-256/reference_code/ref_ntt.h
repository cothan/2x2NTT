#ifndef REF_NTT_H
#define REF_NTT_H

#include <stdint.h>
#include "../params.h"

void ntt(data_t a[DILITHIUM_N]);

void pointwise_barrett(data_t c[DILITHIUM_N],
                       const data_t a[DILITHIUM_N],
                       const data_t b[DILITHIUM_N]);

void invntt(data_t a[DILITHIUM_N]);

#endif
