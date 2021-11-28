#ifndef REF_NTT_H
#define REF_NTT_H

#include <stdint.h>
#include "../params.h"

void ntt(uint16_t a[FALCON_N]);

void pointwise_barrett(uint16_t c[FALCON_N],
                       const uint16_t a[FALCON_N],
                       const uint16_t b[FALCON_N]);

void invntt(uint16_t a[FALCON_N]);

#endif
