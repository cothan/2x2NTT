#include "../ref/newhope_ntt.h"
#include "../ref/newhope_poly.h"
#include "../ref/newhope_precomp.c"
#include "../ref/newhope_reduce.h"
#include <stdlib.h>
#include <stdbool.h>

#if (NEWHOPE_N == 512)

#elif (NEWHOPE_N == 1024)

#else
#error "Either 512 or 1024"
#endif

// NTT DIT RN BO->NO
void ntt_dit(uint16_t *a, const uint16_t *omega)
{
    uint16_t PairsInGroup = NEWHOPE_N / 2;

    uint16_t Distance = 1;
    for (uint16_t NumOfGroups = 1; NumOfGroups < NEWHOPE_N; NumOfGroups = NumOfGroups * 2)
    {
        uint16_t GapToNextPair = 2 * NumOfGroups;
        uint16_t GapToLastPair = GapToNextPair * (PairsInGroup - 1);
        for (uint16_t k = 0; k < NumOfGroups; k++)
        {
            uint16_t Jlast = k + GapToLastPair;
            uint16_t jTwiddle = k * PairsInGroup;
            uint16_t W = omega[jTwiddle];
            for (uint16_t j = k; j <= Jlast; j += GapToNextPair)
            {
                uint16_t temp = montgomery_reduce((uint32_t)W * a[j + Distance]);
                a[j + Distance] = (a[j] + NEWHOPE_Q - temp) % NEWHOPE_Q;
                a[j] = (a[j] + temp) % NEWHOPE_Q;
            }
        }
        PairsInGroup = PairsInGroup / 2;
        Distance = Distance * 2;
    }
}

// NTT DIF RN BO->NO
void ntt_dif(uint16_t *a, const uint16_t *omega)
{
    uint16_t NumberOfProblems = 1;

    uint16_t Distance = 1;
    for (uint16_t ProblemSize = NEWHOPE_N; ProblemSize > 1; ProblemSize = ProblemSize / 2)
    {
        for (uint16_t JFirst = 0; JFirst < NumberOfProblems; JFirst++)
        {
            uint16_t Jtwiddle = 0;
            for (uint16_t J = JFirst; J < NEWHOPE_N - 1; J += 2 * NumberOfProblems)
            {
                uint16_t W = omega[Jtwiddle++];
                uint16_t temp = a[J];
                a[J] = (temp + a[J + Distance]) % NEWHOPE_Q;
                a[J + Distance] = montgomery_reduce(((uint32_t)temp + 3 * NEWHOPE_Q - a[J + Distance]) * W);
            }
        }
        NumberOfProblems = NumberOfProblems * 2;
        Distance = Distance * 2;
    }
}

// Copy of NTT DIT RN BO->NO, with full reduction
void ntt_dit_copy_full_reduction(uint16_t *a, const uint16_t *omega)
{
    uint16_t PairsInGroup = NEWHOPE_N / 2;
    uint16_t count = 0;
    uint16_t Distance = 1;
    for (uint16_t NumOfGroups = 1; NumOfGroups < NEWHOPE_N; NumOfGroups = NumOfGroups * 2)
    {
        uint16_t GapToNextPair = 2 * NumOfGroups;
        uint16_t GapToLastPair = GapToNextPair * (PairsInGroup - 1);
        for (uint16_t k = 0; k < NumOfGroups; k++)
        {
            uint16_t JLast = k + GapToLastPair;
            uint16_t jTwiddle = k * PairsInGroup;
            uint32_t W = omega[jTwiddle];
            for (uint16_t j = k; j <= JLast; j += GapToNextPair)
            {
                uint32_t temp = (W * a[j + Distance]) % NEWHOPE_Q;
                // uint32_t alter = W;
                // alter = alter * a[j +Distance];
                // alter = alter % NEWHOPE_Q;
                // if (alter != temp)
                // {
                //     printf("[Error] Overflow %d-%d\n", alter, temp);
                // }
                a[j + Distance] = ((uint32_t)(a[j] - temp)) % NEWHOPE_Q;
                a[j] = ((uint32_t)(a[j] + temp)) % NEWHOPE_Q;
                count++;
            }
        }
        PairsInGroup = PairsInGroup / 2;
        Distance = Distance * 2;
        // printf("-------------\n");
    }
    printf("DIT count: %d\n", count);
}

// Copy of NTT DIF RN BO->NO, with full reduction
void ntt_dif_full_reduction(uint16_t *a, const uint16_t *omega)
{
    uint16_t NumberOfProblems = 1;
    uint16_t count = 0;
    uint16_t Distance = 1;
    for (uint16_t ProblemSize = NEWHOPE_N; ProblemSize > 1; ProblemSize = ProblemSize / 2)
    {
        for (uint16_t JFirst = 0; JFirst < NumberOfProblems; JFirst++)
        {
            uint16_t Jtwiddle = 0;
            for (uint16_t J = JFirst; J < NEWHOPE_N - 1; J += 2 * NumberOfProblems)
            {
                uint32_t W = omega[Jtwiddle++];
                uint16_t temp = a[J];
                a[J] = (temp + a[J + Distance]) % NEWHOPE_Q;
                a[J + Distance] = (((uint32_t)temp + 1 * NEWHOPE_Q - a[J + Distance]) * W) % NEWHOPE_Q;
                count++;
            }
        }
        NumberOfProblems = NumberOfProblems * 2;
        Distance = Distance * 2;
    }
    printf("DIF count: %d\n", count);
}

void scramble(poly *a)
{
    bitrev_vector(a->coeffs);
    return;
}

int compare(poly *r, poly *r_test, const char *string)
{
    printf("%s:\n", string);
    uint16_t a[4];
    uint16_t b[4];
    bool error = false;
    for (uint16_t i = 0; i < NEWHOPE_N; i += 4)
    {
        a[0] = r->coeffs[i];
        a[1] = r->coeffs[i + 1];
        a[2] = r->coeffs[i + 2];
        a[3] = r->coeffs[i + 3];

        b[0] = r_test->coeffs[i];
        b[1] = r_test->coeffs[i + 1];
        b[2] = r_test->coeffs[i + 2];
        b[3] = r_test->coeffs[i + 3];

        for (uint16_t j = 0; j < 4; j++)
        {
            if (a[j] != b[j])
            {
                printf("[%u] %u != %u\n", i + j, a[j], b[j]);
                error = true;
            }
        }
        if (error)
        {
            printf("Result: Failed!\n");
            exit(1);
        }
    }
    printf("Result: Success!\n---------------------\n");
    return 0;
}

void printArray(uint16_t *sipo, int length, char const *string)
{
    printf("%s: [", string);
    for (int i = 0; i < length; i++)
    {
        printf("%5d,", sipo[i]);
    }
    printf("]\n");
}

void full_reduce(poly *a)
{
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        a->coeffs[i] = a->coeffs[i] % NEWHOPE_Q;
    }
}

void mul_coefficients_full_reduce(uint16_t *r, uint16_t *a)
{
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        uint32_t temp = r[i];
        temp = (temp * a[i]) % NEWHOPE_Q;
        r[i] = temp;
    }
}

int main()
{
    poly r_gold,
        r_test_dif,
        r_test_dif_copy,
        r_test_dit_copy,
        origin_poly;
    uint16_t a, b, c, d;
    srand((unsigned int)&r_gold);

    for (uint16_t i = 0; i < NEWHOPE_N; i += 4)
    {
        // TODO: User rand() here
        a = i + 0;
        b = i + 1;
        c = i + 2;
        d = i + 3;
        // a = rand() % NEWHOPE_Q;
        // b = rand() % NEWHOPE_Q;
        // c = rand() % NEWHOPE_Q;
        // d = rand() % NEWHOPE_Q;
        r_gold.coeffs[i] = a;
        r_gold.coeffs[i + 1] = b;
        r_gold.coeffs[i + 2] = c;
        r_gold.coeffs[i + 3] = d;

        r_test_dif.coeffs[i] = a;
        r_test_dif.coeffs[i + 1] = b;
        r_test_dif.coeffs[i + 2] = c;
        r_test_dif.coeffs[i + 3] = d;

        r_test_dif_copy.coeffs[i] = a;
        r_test_dif_copy.coeffs[i + 1] = b;
        r_test_dif_copy.coeffs[i + 2] = c;
        r_test_dif_copy.coeffs[i + 3] = d;

        r_test_dit_copy.coeffs[i] = a;
        r_test_dit_copy.coeffs[i + 1] = b;
        r_test_dit_copy.coeffs[i + 2] = c;
        r_test_dit_copy.coeffs[i + 3] = d;

        origin_poly.coeffs[i] = a;
        origin_poly.coeffs[i + 1] = b;
        origin_poly.coeffs[i + 2] = c;
        origin_poly.coeffs[i + 3] = d;
    }
    mul_coefficients(r_gold.coeffs, gammas_bitrev_montgomery);
    ntt_copy(r_gold.coeffs, gammas_bitrev_montgomery);
    // NTT DIF: BO-> NO
    mul_coefficients(r_test_dif.coeffs, gammas_bitrev_montgomery);
    ntt_dif(r_test_dif.coeffs, gammas_bitrev_montgomery);

    full_reduce(&r_gold);
    full_reduce(&r_test_dif);

    uint16_t res = compare(&r_gold, &r_test_dif, "NEWHOPE DIF vs MY DIF");

    /*********************poly_ntt test */
    // Revert back to original value
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        r_test_dif.coeffs[i] = origin_poly.coeffs[i];
    }
    scramble(&r_test_dif);
    poly_ntt(&r_test_dif);

    poly_invntt(&r_test_dif);
    full_reduce(&r_test_dif);
    res = compare(&r_test_dif, &origin_poly, "poly_ntt test");
    // YES, only after full_reduce

    // Revert back to original value
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        r_test_dif.coeffs[i] = origin_poly.coeffs[i];
    }

    /********* ntt_dif + mul_coefficient test ************/

    // NTT DIF: BO-> NO
    // Forward NTT, input is stored in BO
    scramble(&r_test_dif);
    mul_coefficients(r_test_dif.coeffs, gammas_bitrev_montgomery);
    ntt_dif(r_test_dif.coeffs, gammas_bitrev_montgomery);

    // Inverse NTT, input is stored in NO, output is in NO
    // Now the output is stored at NO, convert to BO
    scramble(&r_test_dif);
    ntt_dif(r_test_dif.coeffs, omegas_inv_bitrev_montgomery);
    mul_coefficients(r_test_dif.coeffs, gammas_inv_montgomery);

    full_reduce(&r_test_dif);

    res = compare(&r_test_dif, &origin_poly, "ntt_dif + mul_coefficients test");
    // YES

    // Revert back to original value
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        r_test_dif.coeffs[i] = origin_poly.coeffs[i];
    }

    printf("======= Full reduction test ==============\n");
    
    scramble(&r_test_dif);
    mul_coefficients_full_reduce(r_test_dif.coeffs, my_gammas_bitrev);
    ntt_dif_full_reduction(r_test_dif.coeffs, my_gammas_bitrev);

    scramble(&r_test_dif);
    ntt_dif_full_reduction(r_test_dif.coeffs, my_omegas_inv_bitrev);
    mul_coefficients_full_reduce(r_test_dif.coeffs, my_gammas_inv);
    

    res = compare(&r_test_dif, &origin_poly, "ntt_dif_full_reduction + mul_coefficients_full_reduce test");
    // YES

    // Revert back to original value
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        r_test_dif.coeffs[i] = origin_poly.coeffs[i];
    }

    /************************************ Below are trash ************************/

    
    // printArray(r_test_dif.coeffs, NEWHOPE_N, "ntt_dit test");
    // ntt_dif_copy(r_test_dif_copy.coeffs, omega_bitrev_order, 8);
    // ntt_dif_copy(r_test_dif_copy.coeffs, gammas_bitrev_full_reduction);

    // NTT DIT: BO-> NO
    // ntt_dit_copy(r_test_dit_copy.coeffs, omega_natural_order);
    // ntt_dit_copy(r_test_dit_copy.coeffs, bla, N);
    // mul_coefficients(r_test_dit_copy.coeffs, dit_gammas);
    // ntt_dit_copy_full_reduction(r_test_dit_copy.coeffs, dit_omegas);

    // // full_reduce(&r_test_dit_copy);
    // // full_reduce(&r_test_dif_copy);

    // printArray(r_test_dif_copy.coeffs, NEWHOPE_N, "[GOLD]");
    // printArray(r_test_dit_copy.coeffs, NEWHOPE_N, "[TEST]");

    // res = compare(&r_test_dif_copy, &r_test_dit_copy, "my DIF vs my DIT");

    return res;
}

// TODO: Another test: 1. Forward NTT. 2. Scramble. 3. Inverse NTT.