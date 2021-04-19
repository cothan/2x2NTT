#ifndef BUTTERFLY_UNIT_H
#define BUTTERFLY_UNIT_H

#include "mytypes.h"

void butterfly(int mode, u24 *bj, u24 *bjlen, 
                const u24 zeta, const u24 aj, const u24 ajlen);

void buttefly_circuit(u24 *a, u24 *b, u24 *c, u24 *d,
                      const u24 w1, const u24 w2,
                      const u24 w3, const u24 w4,
                      const int mode);

#endif