#include "my_ntt.h"
#include "newhope_reduce.h"
#include "newhope_ntt.h"

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
                uint16_t temp = montgomery_reduce(((uint32_t)W * a[j + Distance]));
                // printf("DIT: %d - %d\n", j+Distance, j);
                a[j + Distance] = (a[j] + 3*NEWHOPE_Q - temp) % NEWHOPE_Q;
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
                a[J + Distance] = montgomery_reduce((((uint32_t)temp + 3 * NEWHOPE_Q - a[J + Distance]) * W));
            }
        }
        NumberOfProblems = NumberOfProblems * 2;
        Distance = Distance * 2;
    }
}

// Copy of NTT DIT RN BO->NO, with full reduction
void ntt_dit_full_reduction(uint16_t *a, const uint16_t *omega)
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
                a[j + Distance] = (a[j] + 3 * NEWHOPE_Q - temp) % NEWHOPE_Q;
                a[j] = (a[j] + temp) % NEWHOPE_Q;
                count++;
            }
        }
        PairsInGroup = PairsInGroup / 2;
        Distance = Distance * 2;
        // printf("-------------\n");
    }
    // printf("DIT count: %d\n", count);
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
                a[J + Distance] = (((uint32_t)temp + 3 * NEWHOPE_Q - a[J + Distance]) * W) % NEWHOPE_Q;
                count++;
            }
        }
        NumberOfProblems = NumberOfProblems * 2;
        Distance = Distance * 2;
    }
    // printf("DIF count: %d\n", count);
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
            printf("+++++++++++++++\n");
            // exit(1);
            return 1;
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


void copy_poly(poly *a, poly *b)
{
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        a->coeffs[i] = b->coeffs[i];
    }
}

void copy_ram(uint64_t *ram, uint64_t *origin_ram)
{
    for (uint16_t i = 0; i < NEWHOPE_N/4; i++)
    {
        ram[i] = origin_ram[i];
    }
}



/**** From test6 ****/
void my_poly_ntt_dif(poly *r)
{
    mul_coefficients(r->coeffs, gammas_bitrev_montgomery);
    ntt_dif((uint16_t *)r->coeffs, gammas_bitrev_montgomery);
}

void my_poly_invntt_dif(poly *r)
{
  bitrev_vector(r->coeffs);
  ntt_dif((uint16_t *)r->coeffs, omegas_inv_bitrev_montgomery);
  mul_coefficients(r->coeffs, gammas_inv_montgomery);
}
/**** ********** ****/

/**** From test7 ****/
void my_poly_ntt_dif_full_reduction(poly *r)
{
    mul_coefficients_full_reduce(r->coeffs, my_gammas_bitrev);
    ntt_dif_full_reduction((uint16_t *)r->coeffs, my_gammas_bitrev);
}

void my_poly_invntt_dif_full_reduction(poly *r)
{
  bitrev_vector(r->coeffs);
  ntt_dif_full_reduction((uint16_t *)r->coeffs, my_omegas_inv_bitrev);
  mul_coefficients_full_reduce(r->coeffs, my_gammas_inv);
}
/**** ********** ****/

/**** From test10 ****/
void my_poly_ntt_dit(poly *r)
{
    mul_coefficients(r->coeffs, gammas_bitrev_montgomery);
    ntt_dit((uint16_t *)r->coeffs, my_omegas_montgomery);
}

void my_poly_invntt_dit(poly *r)
{
  bitrev_vector(r->coeffs);
  ntt_dit((uint16_t *)r->coeffs, my_omegas_inv_montgomery);
  mul_coefficients(r->coeffs, gammas_inv_montgomery);
}
/**** ********** ****/

/**** From test8 ****/
void my_poly_ntt_dit_full_reduction(poly *r)
{
    mul_coefficients_full_reduce(r->coeffs, my_gammas_bitrev);
    ntt_dit_full_reduction((uint16_t *)r->coeffs, my_omegas);
}

void my_poly_invntt_dit_full_reduction(poly *r)
{
  bitrev_vector(r->coeffs);
  ntt_dit_full_reduction((uint16_t *)r->coeffs, my_omegas_inv);
  mul_coefficients_full_reduce(r->coeffs, my_gammas_inv);
}
/**** ********** ****/
