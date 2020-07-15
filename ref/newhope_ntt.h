#ifndef NTT_H
#define NTT_H

#include "inttypes.h"

// DIT Full reduce 
extern uint16_t my_omegas[];
extern uint16_t my_omegas_inv[];

// DIT Montgomery 
extern uint16_t my_omegas_montgomery[];
extern uint16_t my_omegas_inv_montgomery[];

// DIF Full reduce
extern uint16_t my_omegas_bitrev[];
extern uint16_t my_omegas_inv_bitrev[];

// DIF Montgomery
extern uint16_t my_omegas_bitrev_montgomery[];
extern uint16_t my_omegas_inv_bitrev_montgomery[];

// Scaling Montgomery
extern uint16_t my_gammas_bitrev_montgomery[];
extern uint16_t my_gammas_inv_montgomery[];

// Scaling Full reduce
extern uint16_t my_gammas_bitrev[];
extern uint16_t my_gammas_inv[];

// Default constants
extern uint16_t omegas_inv_bitrev_montgomery[];
extern uint16_t gammas_inv_montgomery[];
extern uint16_t gammas_bitrev_montgomery[];

void bitrev_vector(uint16_t* poly);
void mul_coefficients(uint16_t* poly, const uint16_t* factors);
void ntt(uint16_t* poly, const uint16_t* omegas);
void ntt_copy(uint16_t* poly, const uint16_t* omegas);

#endif
