#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "params.h"
#include "ntt2x2.h"
#include "consts.h"
#include "ram_util.h"
#include "address_encoder_decoder.h"
#include "ntt.h"
#include "util.h"

/* 
 * Inverse NTT Test, this function give correct result as in reference Inverse NTT 
 * Support divide by 2. Correct. Verified. 
 */
int ntt2x2_NTT(int32_t r_gold[DILITHIUM_N])
{
    bram ram;
    // Load data into BRAM, 4 coefficients per line
    reshape(&ram, r_gold);
    // Compute NTT
    forward_ntt2x2(&ram, 0, FORWARD_NTT_MODE, ENCODE_FALSE);

    // Run the reference code
    ntt(r_gold);

    // print_array(r_gold, 256, "r_gold");
    // print_reshaped_array(&ram, 64, "ram");

    int ret = compare_bram_array(&ram, r_gold, "ntt2x2_NTT", ENCODE_TRUE, 0);

    return ret;
}

/* 
 * Inverse NTT Test, this function give correct result as in reference Inverse NTT 
 * Support divide by 2. Correct. Verified. 
 */
int ntt2x2_INVNTT(int32_t r_gold[DILITHIUM_N])
{
    bram ram;
    // Load data into BRAM, 4 coefficients per line
    reshape(&ram, r_gold);
    // Compute NTT
    ntt2x2(&ram, 0, INVERSE_NTT_MODE, DECODE_FALSE);

    // Run the reference code
    invntt_tomont(r_gold);

    // print_array(r_gold, 256, "r_gold");
    // print_reshaped_array(&ram, 64, "ram");

    int ret = compare_bram_array(&ram, r_gold, "ntt2x2_INVNTT", DECODE_TRUE, 0);

    return ret;
}

/* 
 * Multiplier between two memory test.
 * Correct, verified, optimized. 
 */
int ntt2x2_MUL(int32_t r_mul[DILITHIUM_N], int32_t test_ram[DILITHIUM_N])
{
    // Compare with the reference code
    bram ram, mul_ram;

    // Load data into BRAM, 4 coefficients per line
    reshape(&ram, r_mul);
    reshape(&mul_ram, test_ram);

    // MUL Operation using NTT
    // Enable DECODE_TRUE only after NTT transform
    // This example we only do pointwise multiplication
    ntt2x2(&ram, &mul_ram, MUL_MODE, DECODE_FALSE);

    // Run the reference code
    pointwise_montgomery(r_mul, r_mul, test_ram);

    int ret = compare_bram_array(&ram, r_mul, "ntt2x2_MUL", DECODE_FALSE, 0);

    // printf("==============MUL is Correct!\n\n");
    return ret;
}

#define TESTS 1

int main()
{
    // srand(time(0));
    srand(0);
    int32_t r_invntt[DILITHIUM_N], 
            r_mul[DILITHIUM_N], 
            test_ram[DILITHIUM_N], 
            r_ntt[DILITHIUM_N];
    int32_t t1, t2, t3, t4;
    int ret = 1;

    for (int k = 0; k < TESTS; k++)
    {
        for (int i = 0; i < DILITHIUM_N; i++)
        {
            // t1 = i;
            t1 = rand() % DILITHIUM_Q;
            r_invntt[i] = t1;

            t2 = rand() % DILITHIUM_Q;
            r_mul[i] = t2;

            t3 = rand() % DILITHIUM_Q;
            test_ram[i] = t3;

            t4 = i; 
            r_ntt[i] = t4;
        }

        (void) test_ram;
        (void) r_mul;
        (void) r_ntt;
        ret &= ntt2x2_INVNTT(r_invntt);
        ret &= ntt2x2_MUL(r_mul, test_ram);
        ret &= ntt2x2_NTT(r_ntt);

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

/* 
 * Combine GS and CT butterfly  unit: DONE 
 * Replace Montgomery to Barrett reduction: DONE 
 * Divide by 2 in inverse NTT: No yet 
 * Find algorithm to compute fast bit-reverse permutation for Forward NTT: 
 * What the point of merging forward and inverse butterfly unit?
 */