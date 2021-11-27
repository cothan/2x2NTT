#ifndef NTT2x2_H
#define NTT2x2_H

#include <stdint.h>
#include "params.h"

#include "params.h"

typedef struct
{
    uint16_t coeffs[FALCON_N / 4][4];
} bram;

void update_indexes(uint16_t tw_i[4], const uint16_t tw_base_i[4],
                    const uint16_t s, enum OPERATION mode);

void ntt2x2_fwdntt(bram *ram, enum OPERATION mode, enum MAPPING mapping);

void ntt2x2_mul(bram *ram, const bram *mul_ram, enum MAPPING mapping);

void ntt2x2_invntt(bram *ram, enum OPERATION mode, enum MAPPING mapping);

#endif
