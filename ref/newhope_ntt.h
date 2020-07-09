#ifndef NTT_H
#define NTT_H

#include "inttypes.h"

extern uint16_t omegas_inv_bitrev_montgomery[];
extern uint16_t gammas_bitrev_montgomery[];
extern uint16_t gammas_inv_montgomery[];

extern uint16_t my_omegas_montgomery[];
extern uint16_t my_omegas_inv_montgomery[];

extern uint16_t my_gammas_bitrev[];
extern uint16_t my_gammas_inv[];

extern uint16_t my_omegas_inv[];
extern uint16_t my_omegas[];

extern uint16_t my_omegas_inv_bitrev[];

void bitrev_vector(uint16_t* poly);
void mul_coefficients(uint16_t* poly, const uint16_t* factors);
void ntt(uint16_t* poly, const uint16_t* omegas);
void ntt_copy(uint16_t* poly, const uint16_t* omegas);

#endif
