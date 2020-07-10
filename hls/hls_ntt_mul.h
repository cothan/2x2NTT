#ifndef HLS_NTT_MUL
#define HLS_NTT_MUL

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../ref/newhope_ntt.h"
#include "../ref/newhope_poly.h"
#include "../ref/newhope_ntt.h"

extern uint16_t hls_ram1_gammas_bitrev_montgomery[];
extern uint16_t hls_ram2_gammas_bitrev_montgomery[];
extern uint16_t hls_ram1_my_omegas_montgomery[];
extern uint16_t hls_ram2_my_omegas_montgomery[];
extern uint16_t hls_ram1_my_omegas_inv_montgomery[];
extern uint16_t hls_ram2_my_omegas_inv_montgomery[];
extern uint16_t hls_ram1_gammas_inv_montgomery[];
extern uint16_t hls_ram2_gammas_inv_montgomery[];

extern uint16_t hls_ram1_my_gammas_bitrev[];
extern uint16_t hls_ram2_my_gammas_bitrev[];
extern uint16_t hls_ram1_my_omegas[];
extern uint16_t hls_ram2_my_omegas[];
extern uint16_t hls_ram1_my_omegas_inv[];
extern uint16_t hls_ram2_my_omegas_inv[];
extern uint16_t hls_ram1_my_gammas_inv[];
extern uint16_t hls_ram2_my_gammas_inv[];

enum STATE
{
    PSIS,
    NTT,
    MUL,
    INTT,
    IPSIS,
};
void unpack(uint64_t *ram, uint16_t index, uint16_t *A, uint16_t *B, uint16_t *C, uint16_t *D);
void pack(uint16_t A, uint16_t B, uint16_t C, uint16_t D, uint64_t *ram, uint16_t index, bool debug);
void hls_poly_ntt_mul(uint64_t *ram, uint16_t *ram1, uint16_t *ram2, enum STATE state, bool debug);

#endif