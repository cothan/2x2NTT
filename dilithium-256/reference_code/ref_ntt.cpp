#include <stdio.h>
#include "ref_ntt.h"
#include "../consts.h"

void ntt(data_t a[DILITHIUM_N])
{
    unsigned int len, start, j, k;
    data_t zeta, t;
    data_t m, n;

    k = 0;
    for (len = DILITHIUM_N / 2; len > 0; len >>= 1)
    {
        for (start = 0; start < DILITHIUM_N; start = j + len)
        {
            zeta = zetas_barrett[++k];
            for (j = start; j < start + len; ++j)
            {

                t = ((data2_t)zeta * a[j + len]) % DILITHIUM_Q;
                a[j + len] = (a[j] - t) % DILITHIUM_Q;
                a[j] = (a[j] + t) % DILITHIUM_Q;

                m = a[j];
                n = a[j + len];
#if DEBUG == 5
                printf("%d: %u, %u = %u, %u | %u\n", len, j, j + len, m, n, k);
#endif
            }
        }
    }
}

void pointwise_barrett(data_t c[DILITHIUM_N],
                       const data_t a[DILITHIUM_N],
                       const data_t b[DILITHIUM_N])
{
    for (unsigned i = 0; i < DILITHIUM_N; ++i)
    {
        c[i] = ((data2_t)a[i] * b[i]) % DILITHIUM_Q;
    }
}

void invntt(data_t a[DILITHIUM_N])
{
    unsigned int start, len, j, k;
    data_t t, zeta, w;
    data_t m, n;

    const data_t f = 8347681; // pow(256, -1, 8380417)

    k = DILITHIUM_N;
    for (len = 1; len < DILITHIUM_N; len <<= 1)
    {
        for (start = 0; start < DILITHIUM_N; start = j + len)
        {
            // Plus Q so it is alway positive
            zeta = - zetas_barrett[--k];
            for (j = start; j < start + len; ++j)
            {
                t = a[j];
                a[j] = (t + a[j + len]) % DILITHIUM_Q;
                w = (t - a[j + len]) % DILITHIUM_Q;
                a[j + len] = ((data2_t)zeta * w) % DILITHIUM_Q;

                m = a[j];
                n = a[j + len];
#if DEBUG == 5
                printf("%d: %u, %u = %u, %u | %u\n", len, j, j + len, m, n, k);
#endif
            }
        }
    }

    for (j = 0; j < DILITHIUM_N; ++j)
    {
        // This work, but f is not same as in ref code
        a[j] = ((data2_t)f * a[j]) % DILITHIUM_Q;
    }
}
