#ifndef HLS_NTT_TEST_H
#define HLS_NTT_TEST_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../ref/newhope_params.h"
#include "../ref/newhope_poly.h"
#include "../ref/newhope_reduce.h"

void iterative_ntt(uint16_t *a);

#endif