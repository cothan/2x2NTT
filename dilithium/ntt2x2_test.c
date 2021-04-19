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

int ntt2x2_INVNTT(int32_t r_gold[DILITHIUM_N], int32_t r[DILITHIUM_N])
{
    bram ram, mul_ram;
    // Load data into BRAM, 4 coefficients per line
    reshape(&ram, r);
    reshape(&mul_ram, MUL_RAM_barret);
    // Compute NTT
    ntt2x2(&ram, &mul_ram, INVERSE_NTT_MODE, DECODE_FALSE);

    // Enable DECODE_TRUE only after NTT transform
    ntt2x2(&ram, &mul_ram, MUL_MODE, DECODE_TRUE);

    // Run the reference code
    invntt_tomont(r_gold);

    // Compare with the reference code
    int32_t a, b, c, d;
    int32_t ta, tb, tc, td;

    int addr;
    int ret = 0;

    // print_array(r_gold, 16, "r_gold");
    // print_reshaped_array(&ram, 4, "ram");

    for (int i = 0; i < DILITHIUM_N; i += 4)
    {
        // Get golden result
        a = r_gold[i + 0];
        b = r_gold[i + 1];
        c = r_gold[i + 2];
        d = r_gold[i + 3];

        addr = addr_decoder(i / 4);
        read_ram(&ta, &tb, &tc, &td, &ram, addr);
        // Comapre with reference code

        // Quick xor, I hate long if-else clause
        ret |= a != ta;
        ret |= b != tb;
        ret |= c != tc;
        ret |= d != td;

        if (ret)
        {
            printf("INVTNTT Error at index: %d => %d\n", addr, i);
            printf("%12d | %12d | %12d | %12d\n", a, b, c, d);
            printf("%12d | %12d | %12d | %12d\n", ta, tb, tc, td);
            return 1;
        }
    }
    printf("==============INV_NTT is Correct!\n\n");
    return 0;
}

int ntt2x2_MUL(int32_t r_gold[DILITHIUM_N], int32_t r[DILITHIUM_N])
{
    bram ram, mul_ram;

    // Load data into BRAM, 4 coefficients per line
    reshape(&ram, r);
    reshape(&mul_ram, MUL_test_RAM);

    // MUL Operation using NTT
    // Enable DECODE_TRUE only after NTT transform
    // This example we only do pointwise multiplication
    ntt2x2(&ram, &mul_ram, MUL_MODE, DECODE_FALSE);

    // Run the reference code
    pointwise_montgomery(r_gold, r_gold, MUL_test_RAM);

    // Compare with the reference code
    int32_t a, b, c, d;
    int32_t ta, tb, tc, td;

    int addr;
    int ret = 0;

    // print_array(r_gold, 32, "gold");
    // print_reshaped_array(&ram, 8, "first 8");

    for (int i = 0; i < DILITHIUM_N; i += 4)
    {
        // Get golden result
        a = r_gold[i + 0];
        b = r_gold[i + 1];
        c = r_gold[i + 2];
        d = r_gold[i + 3];

        addr = i / 4;
        read_ram(&ta, &tb, &tc, &td, &ram, addr);

        // Comapre with reference code

        // Quick xor, I hate long if-else clause
        ret |= a != ta;
        ret |= b != tb;
        ret |= c != tc;
        ret |= d != td;

        if (ret)
        {
            printf("MUL Error at index: %d => %d\n", addr, i);
            printf("%12d | %12d | %12d | %12d\n", a, b, c, d);
            printf("%12d | %12d | %12d | %12d\n", ta, tb, tc, td);
            return 1;
        }
    }
    printf("==============MUL is Correct!\n\n");
    return 0;
}

#define TESTS 10

int main()
{
    srand(time(0));
    // srand(0);
    int32_t r[DILITHIUM_N], r_gold[DILITHIUM_N], r_gold_copy[DILITHIUM_N], r_copy[DILITHIUM_N];
    int32_t t1, t2;
    int ret = 0;

    for (int k = 0; k < TESTS; k++)
    {
        for (int i = 0; i < DILITHIUM_N; i++)
        {
            // t1 = i;
            t1 = rand() % DILITHIUM_Q;
            r[i] = t1;
            r_gold[i] = t1;

            // t2 = i + 2;
            t2 = rand() % DILITHIUM_Q;
            r_gold_copy[i] = t2;
            r_copy[i] = t2;
        }

        ret |= ntt2x2_INVNTT(r_gold, r);
        ret |= ntt2x2_MUL(r_gold_copy, r_copy);

        if (ret)
        {
            break;
        }
    }

    // ntt(r_gold);

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