#ifndef CONFIG_H
#define CONFIG_H

#include "../params.h"

#define BRAM_DEPT (FALCON_N / 4)

template <typename T>
struct BRAM
{
    T coeffs[BRAM_DEPT][4];
};

typedef BRAM<data_t> bram;

enum OPERATION
{
    FORWARD_NTT_MODE,
    INVERSE_NTT_MODE,
    MUL_MODE
};

enum MAPPING
{
    NATURAL,
    AFTER_NTT,
    AFTER_INVNTT
};

#endif
