#ifndef MY_NTT_H
#define MY_NTT_H

#include <stdbool.h>
#include <stdlib.h>
#include "newhope_poly.h"


void ntt_dit(uint16_t *a, const uint16_t *omega);
void ntt_dif(uint16_t *a, const uint16_t *omega);

void ntt_dit_full_reduction(uint16_t *a, const uint16_t *omega);
void ntt_dif_full_reduction(uint16_t *a, const uint16_t *omega);

void scramble(poly *a);
void full_reduce(poly *a);
void mul_coefficients_full_reduce(uint16_t *r, uint16_t *a);

int compare(poly *r, poly *r_test, const char *string);
void printArray(uint16_t *sipo, int length, char const *string);

void copy_poly(poly *a, poly *b);

void my_poly_ntt_dif(poly *r);
void my_poly_invntt_dif(poly *r);

void my_poly_invntt_dif_full_reduction(poly *r);
void my_poly_ntt_dif_full_reduction(poly *r);


#endif