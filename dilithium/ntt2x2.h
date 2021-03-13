#ifndef NTT2x2_H
#define NTT2x2_H

#include <stdint.h>
#include "params.h"


typedef struct
{
    int32_t coeffs[4];
} line;

typedef struct
{
    line vec[N / 4];
} bram;


#endif