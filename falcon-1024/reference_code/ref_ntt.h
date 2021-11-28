#ifndef REF_NTT_H
#define REF_NTT_H

#include <stdint.h>
#include "../params.h"

void ntt(data_t a[FALCON_N]);

void pointwise_barrett(data_t c[FALCON_N],
                       const data_t a[FALCON_N],
                       const data_t b[FALCON_N]);

void invntt(data_t a[FALCON_N]);

#endif
