#ifndef NTT2x2_H
#define NTT2x2_H

#include <stdint.h>
#include "params.h"

#include "params.h"

typedef struct
{
    int32_t coeffs[DILITHIUM_N / 4][4];
} bram;

void update_indexes(int tw_i[4], const int tw_base_i[4],
                    const int s, enum OPERATION mode);

void ntt2x2_fwdntt(bram *ram, enum OPERATION mode, enum MAPPING mapping);

void ntt2x2_mul(bram *ram, const bram *mul_ram, enum MAPPING mapping);

void ntt2x2_invntt(bram *ram, enum OPERATION mode, enum MAPPING mapping);

#endif