#ifndef NTT2x2_H
#define NTT2x2_H

#include <stdint.h>
#include "config.h"

void update_indexes(unsigned tw_i[4],
                    const unsigned tw_base_i[4],
                    const unsigned s, enum OPERATION mode);

void ntt2x2_fwdntt(bram *ram, enum OPERATION mode, enum MAPPING mapping);

void ntt2x2_mul(bram *ram, const bram *mul_ram, enum MAPPING mapping);

void ntt2x2_invntt(bram *ram, enum OPERATION mode, enum MAPPING mapping);

#endif
