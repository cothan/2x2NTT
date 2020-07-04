#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "newhope_params.h"

/* 
 * Elements of R_q = Z_q[X]/(X^n + 1). Represents polynomial
 * coeffs[0] + X*coeffs[1] + X^2*xoeffs[2] + ... + X^{n-1}*coeffs[n-1] 
 */
typedef struct {
  uint16_t coeffs[NEWHOPE_N];
} poly;

void poly_ntt(poly *r);
void poly_invntt(poly *r);
void poly_mul_pointwise(poly *r, const poly *a, const poly *b);


#endif
