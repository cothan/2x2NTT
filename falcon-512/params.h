#ifndef PARAMS_H
#define PARAMS_H

#include <stdint.h>

// Define size of falcon
typedef uint16_t data_t;
typedef uint32_t data2_t;

#define FALCON_Q 12289 // 2**13 + 2**12 + 1
#define FALCON_Q0I 12287

// This directory support FALCON_MODE == [1]
#define FALCON_MODE 1


#if FALCON_MODE == 0
#define FALCON_LOGN 8
#define FALCON_N 256

#elif FALCON_MODE == 1
#define FALCON_LOGN 9
#define FALCON_N 512

#elif FALCON_MODE == 5
#define FALCON_LOGN 10
#define FALCON_N 1024

#endif


#endif
