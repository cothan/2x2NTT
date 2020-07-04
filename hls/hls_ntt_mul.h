#ifndef HLS_NTT_MUL
#define HLS_NTT_MUL

#include <stdbool.h>
#include "../ref/newhope_params.h"
#include "../ref/newhope_poly.h"
#include "../ref/newhope_reduce.h"

enum STATE
{
    PSIS,
    NTT,
    MUL,
    INTT,
    IPSIS,
};
void unpack(uint64_t *ram, uint16_t index, uint16_t *A, uint16_t *B, uint16_t *C, uint16_t *D);
void pack(uint16_t A, uint16_t B, uint16_t C, uint16_t D, uint64_t *ram, uint64_t index);
void hls_poly_ntt_mul(uint64_t ram, enum STATE state);

#endif