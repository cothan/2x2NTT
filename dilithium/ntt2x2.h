#ifndef NTT2x2_H
#define NTT2x2_H

#include <stdint.h>
#include "params.h"

#include "params.h"

typedef struct
{
    int32_t coeffs[DILITHIUM_N / 4][4];
} bram;

void ntt2x2(bram *ram, bram *mul_ram, enum OPERATION mode, enum MAPPING decode);

void forward_ntt2x2(bram *ram, bram *mul_ram, enum OPERATION mode, enum MAPPING decode);

#endif