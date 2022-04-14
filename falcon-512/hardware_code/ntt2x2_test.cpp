#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "../params.h"
#include "../reference_code/ref_ntt2x2.h"
#include "../reference_code/ref_ntt.h"
#include "../consts.h"
#include "config.h"
#include "ntt2x2.h"
#include "address_encoder_decoder.h"
#include "util.h"

/* 
 * Forward NTT Test, this function give correct result as in reference Forward NTT 
 */
int ntt2x2_NTT(data_t r_gold[FALCON_N])
{
    bram ram;
    // Load data into BRAM, 4 coefficients per line
    reshape(&ram, r_gold);
    // Compute NTT
    ntt2x2_fwdntt(&ram, FORWARD_NTT_MODE, NATURAL);

    // Run the reference code
    ntt2x2_ref(r_gold);

    // print_array(r_gold, FALCON_N, "r_gold");
    // print_reshaped_array(&ram, BRAM_DEPT, "ram");

    int ret = compare_bram_array(&ram, r_gold, "ntt2x2_NTT", AFTER_NTT, 0);

    return ret;
}

/* 
 * Inverse NTT Test, this function give correct result as in reference Inverse NTT 
 * Support divide by 2. Correct. Verified. 
 */
int ntt2x2_INVNTT(data_t r_gold[FALCON_N])
{
    bram ram;
    // Load data into BRAM, 4 coefficients per line
    reshape(&ram, r_gold);
    // Compute NTT
    ntt2x2_invntt(&ram, INVERSE_NTT_MODE, NATURAL);

    // Run the reference code
    invntt2x2_ref(r_gold);

    // print_array(r_gold, FALCON_N, "r_gold");
    // print_reshaped_array(&ram, BRAM_DEPT, "ram");

    int ret = compare_bram_array(&ram, r_gold, "ntt2x2_INVNTT", AFTER_INVNTT, 0);

    return ret;
}

/* 
 * Multiplier between two memory test.
 * Correct, verified, optimized. 
 */
int ntt2x2_MUL(data_t r_mul[FALCON_N], data_t test_ram[FALCON_N])
{
    // Compare with the reference code
    bram ram, mul_ram;

    // Load data into BRAM, 4 coefficients per line
    reshape(&ram, r_mul);
    reshape(&mul_ram, test_ram);

    // MUL Operation using NTT
    // Enable DECODE_TRUE only after NTT transform
    // This example we only do pointwise multiplication
    ntt2x2_mul(&ram, &mul_ram, NATURAL);

    // Run the reference code
    pointwise_barrett(r_mul, r_mul, test_ram);

    int ret = compare_bram_array(&ram, r_mul, "ntt2x2_MUL", NATURAL, 0);

    return ret;
}

int polymul(data_t a[FALCON_N], data_t b[FALCON_N])
{
    bram ram_a_ntt, ram_b_ntt;
    int ret = 0;
    reshape(&ram_a_ntt, a);
    reshape(&ram_b_ntt, b);

    // Test Hardware Multiplication
    ntt2x2_fwdntt(&ram_a_ntt, FORWARD_NTT_MODE, NATURAL);
    ntt2x2_fwdntt(&ram_b_ntt, FORWARD_NTT_MODE, NATURAL);

    ntt(a);
    ntt(b);
    ret |= compare_bram_array(&ram_a_ntt, a, "FORWARD_NTT_MODE A", AFTER_NTT, 0);
    ret |= compare_bram_array(&ram_b_ntt, b, "FORWARD_NTT_MODE B", AFTER_NTT, 0);

    ntt2x2_mul(&ram_a_ntt, &ram_b_ntt, NATURAL);
    pointwise_barrett(a, a, b);
    ret |= compare_bram_array(&ram_a_ntt, a, "MUL A*B", AFTER_NTT, 0);

    ntt2x2_invntt(&ram_a_ntt, INVERSE_NTT_MODE, AFTER_NTT);
    invntt(a);

    ret |= compare_bram_array(&ram_a_ntt, a, "INVERSE_NTT_MODE(A*B)", NATURAL, 0);

    // Test Software Multiplication

    return ret;
}

#define TESTS 100000

int main()
{
    printf("Test for FALCON_N = %u\n", FALCON_N);
    srand(time(0));
    data_t r_invntt[FALCON_N],
        r_mul[FALCON_N],
        test_ram[FALCON_N],
        r_ntt[FALCON_N],
        a[FALCON_N],
        b[FALCON_N];
    data_t t1, t2, t3, t4, t5;
    int ret = 0;

    for (int k = 0; k < TESTS; k++)
    {
        for (int i = 0; i < FALCON_N; i++)
        {
            // t1 = i;
            t1 = rand() % FALCON_Q;
            r_invntt[i] = t1;

            t2 = rand() % FALCON_Q;
            r_mul[i] = t2;

            t3 = rand() % FALCON_Q;
            test_ram[i] = t3;

            t4 = rand() % FALCON_Q;
            r_ntt[i] = t4;

            t5 = rand() % FALCON_Q;
            a[i] = t5 % FALCON_Q;
            b[i] = t5 * 31 % FALCON_Q;
        }

        ret |= ntt2x2_MUL(r_mul, test_ram);
        ret |= ntt2x2_NTT(r_ntt);
        ret |= ntt2x2_INVNTT(r_invntt);
        ret |= polymul(a, b);

        if (ret)
        {
            break;
        }
    }

    if (ret)
    {
        printf("ERROR\n");
    }
    else
    {
        printf("OK\n");
    }

    return ret;
}
