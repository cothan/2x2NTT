#ifndef NTT_H
#define NTT_H

#include "mytypes.h"
#include "params.h"

void ntt2x2(u96 ram[DILITHIUM_N / 4], u96 mul_ram[DILITHIUM_N / 4], enum OPERATION mode, enum MAPPING decode);

#endif 
