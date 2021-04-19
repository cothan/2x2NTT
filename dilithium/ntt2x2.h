#ifndef NTT2x2_H
#define NTT2x2_H

#include <stdint.h>
#include "params.h"

#include "params.h"

typedef struct
{
    int32_t coeffs[4];
} line;

typedef struct
{
    line vec[DILITHIUM_N / 4];
} bram;

void ntt2x2(bram *ram, bram *mul_ram, enum OPERATION mode, enum MAPPING decode);

#endif