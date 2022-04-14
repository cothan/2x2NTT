#include <stdio.h>
#include "ref_ntt.h"
#include "../consts.h"

void ntt(data_t a[FALCON_N])
{
    unsigned int len, start, j, k;
    data_t zeta, t;
    data_t m, n;

    k = 0;
    for (len = FALCON_N / 2; len > 0; len >>= 1)
    {
        for (start = 0; start < FALCON_N; start = j + len)
        {
            zeta = zetas_barrett[++k];
            for (j = start; j < start + len; ++j)
            {

                t = ((data2_t)zeta * a[j + len]) % FALCON_Q;
                a[j + len] = (a[j] - t) % FALCON_Q;
                a[j] = (a[j] + t) % FALCON_Q;

#if DEBUG == 5
                m = a[j];
                n = a[j + len];
                printf("%d: %u, %u = %u, %u | %u\n", len, j, j + len, m, n, k);
#endif
            }
        }
    }
}

void pointwise_barrett(data_t c[FALCON_N],
                       const data_t a[FALCON_N],
                       const data_t b[FALCON_N])
{
    for (unsigned i = 0; i < FALCON_N; ++i)
    {
        c[i] = ((data2_t)a[i] * b[i]) % FALCON_Q;
    }
}

void invntt(data_t a[FALCON_N])
{
    unsigned int start, len, j, k;
    data_t t, zeta, w;
    data_t m, n;

#if FALCON_N == 256
    const data_t f = 12241; // pow(256, -1, 12289)
#elif FALCON_N == 512
    const data_t f = 12265; // pow(512, -1, 12289)
#elif FALCON_N == 1024
    const data_t f = 12277; // pow(1024, -1, 12289)
#else
#error "See config.h, FALCON_N is not supported"
#endif

    k = FALCON_N;
    for (len = 1; len < FALCON_N; len <<= 1)
    {
        for (start = 0; start < FALCON_N; start = j + len)
        {
            // Plus Q so it is alway positive
            zeta = - zetas_barrett[--k];
            for (j = start; j < start + len; ++j)
            {
                t = a[j];
                a[j] = (t + a[j + len]) % FALCON_Q;
                w = (t - a[j + len]) % FALCON_Q;
                a[j + len] = ((data2_t)zeta * w) % FALCON_Q;

#if DEBUG == 5
                m = a[j];
                n = a[j + len];
                printf("%d: %u, %u = %u, %u | %u\n", len, j, j + len, m, n, k);
#endif
            }
        }
    }

    for (j = 0; j < FALCON_N; ++j)
    {
        // This work, but f is not same as in ref code
        a[j] = ((data2_t)f * a[j]) % FALCON_Q;
    }
}
