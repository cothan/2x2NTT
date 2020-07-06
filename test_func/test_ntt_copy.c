#include "../ref/newhope_ntt.h"
#include "../ref/newhope_poly.h"
#include "../ref/newhope_precomp.c"
#include <stdlib.h>
#include <stdbool.h>


int compare(poly *r, poly *r_test)
{
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
            return 1;
    }
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

int main()
{
    poly r_gold;
    poly r_test;
    uint16_t a, b, c, d;
    srand( (unsigned int) &r_gold);

    for (uint16_t i = 0; i < NEWHOPE_N; i += 4)
    {
        a = i;
        b = i + 1;
        c = i + 2;
        d = i + 3;
        r_gold.coeffs[i] = a;
        r_gold.coeffs[i + 1] = b;
        r_gold.coeffs[i + 2] = c;
        r_gold.coeffs[i + 3] = d;

        r_test.coeffs[i] = a;
        r_test.coeffs[i + 1] = b;
        r_test.coeffs[i + 2] = c;
        r_test.coeffs[i + 3] = d;
    }
    ntt(r_gold.coeffs, gammas_bitrev_montgomery);
    ntt_copy(r_test.coeffs, gammas_bitrev_montgomery);
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        r_gold.coeffs[i] = r_gold.coeffs[i] % NEWHOPE_Q;
        r_test.coeffs[i] = r_test.coeffs[i] % NEWHOPE_Q;
    }

    // printArray(r_gold.coeffs, NEWHOPE_N, "[GOLD]");
    // printArray(r_test.coeffs, NEWHOPE_N, "[TEST]");

    uint16_t res = compare(&r_gold, &r_test);

    return res;
}