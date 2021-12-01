#ifndef REF_NTT_2x2_H
#define REF_NTT_2x2_H

#include <stdint.h>
#include "../params.h"


void ntt2x2_ref(data_t a[FALCON_N]);

void invntt2x2_ref(data_t a[FALCON_N]);


#endif