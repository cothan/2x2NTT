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
 * Forward NTT Test, this function give correct result as in reference Forward NTT 
 */
int ntt2x2_NTT(uint16_t r_gold[FALCON_N])
{
    bram ram;
    // Load data into BRAM, 4 coefficients per line
    reshape(&ram, r_gold);
    // Compute NTT
    ntt2x2_fwdntt(&ram, FORWARD_NTT_MODE, ENCODE_FALSE);

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
int ntt2x2_INVNTT(uint16_t r_gold[FALCON_N])
{
    bram ram;
    // Load data into BRAM, 4 coefficients per line
    reshape(&ram, r_gold);
    // Compute NTT
    ntt2x2_invntt(&ram, INVERSE_NTT_MODE, DECODE_FALSE);

    // Run the reference code
    invntt(r_gold);

    // print_array(r_gold, 256, "r_gold");
    // print_reshaped_array(&ram, 64, "ram");

    int ret = compare_bram_array(&ram, r_gold, "ntt2x2_INVNTT", DECODE_TRUE, 0);

    return ret;
}

/* 
 * Multiplier between two memory test.
 * Correct, verified, optimized. 
 */
int ntt2x2_MUL(uint16_t r_mul[FALCON_N], uint16_t test_ram[FALCON_N])
{
    // Compare with the reference code
    bram ram, mul_ram;

    // Load data into BRAM, 4 coefficients per line
    reshape(&ram, r_mul);
    reshape(&mul_ram, test_ram);

    // MUL Operation using NTT
    // Enable DECODE_TRUE only after NTT transform
    // This example we only do pointwise multiplication
    ntt2x2_mul(&ram, &mul_ram, DECODE_FALSE);

    // Run the reference code
    pointwise_montgomery(r_mul, r_mul, test_ram);

    int ret = compare_bram_array(&ram, r_mul, "ntt2x2_MUL", DECODE_FALSE, 0);

    // printf("==============MUL is Correct!\n\n");
    return ret;
}

int polymul(uint16_t a[FALCON_N], uint16_t b[FALCON_N])
{
    bram ram_a_ntt, ram_b_ntt;
    int ret = 0;
    reshape(&ram_a_ntt, a);
    reshape(&ram_b_ntt, b);

    // Test Hardware Multiplication
    ntt2x2_fwdntt(&ram_a_ntt, FORWARD_NTT_MODE, ENCODE_FALSE);
    ntt2x2_fwdntt(&ram_b_ntt, FORWARD_NTT_MODE, ENCODE_FALSE);

    ntt(a);
    ntt(b);
    ret |= compare_bram_array(&ram_a_ntt, a, "FORWARD_NTT_MODE A", ENCODE_TRUE, 0);
    ret |= compare_bram_array(&ram_b_ntt, b, "FORWARD_NTT_MODE B", ENCODE_TRUE, 0);

    ntt2x2_mul(&ram_a_ntt, &ram_b_ntt, ENCODE_FALSE);
    pointwise_montgomery(a, a, b);
    ret |= compare_bram_array(&ram_a_ntt, a, "MUL A*B", ENCODE_TRUE, 0);

    ntt2x2_invntt(&ram_a_ntt, INVERSE_NTT_MODE, ENCODE_TRUE);
    invntt(a);

    ret |= compare_bram_array(&ram_a_ntt, a, "INVERSE_NTT_MODE(A*B)", DECODE_FALSE, 0);

    // Test Software Multiplication
    
    return ret;
}

#define TESTS 100

int main()
{
    // srand(time(0));
    srand(0);
    uint16_t r_invntt[FALCON_N], 
            r_mul[FALCON_N], 
            test_ram[FALCON_N], 
            r_ntt[FALCON_N], 
            a[FALCON_N], 
            b[FALCON_N];
    uint16_t t1, t2, t3, t4, t5;
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
            b[i] = t5*31 % FALCON_Q;
        }

        ret |= ntt2x2_MUL(r_mul, test_ram);
        ret |= ntt2x2_NTT(r_ntt);
        ret |= ntt2x2_INVNTT(r_invntt);
        // ret |= polymul(a, b);

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
