#include <stdio.h>
#include "ref_ntt.h"
#include "../consts.h"

void ntt(uint16_t a[FALCON_N])
{
    unsigned int len, start, j, k;
    uint16_t zeta, t;
    uint16_t m, n;

    k = 0;
    for (len = FALCON_N / 2; len > 0; len >>= 1)
    {
        for (start = 0; start < FALCON_N; start = j + len)
        {
            zeta = zetas_barrett[++k];
            for (j = start; j < start + len; ++j)
            {

                t = ((uint32_t)zeta * a[j + len]) % FALCON_Q;
                a[j + len] = (a[j] + FALCON_Q - t) % FALCON_Q;
                a[j] = (a[j] + t) % FALCON_Q;

                m = a[j];
                n = a[j + len];
#if DEBUG == 5
                printf("%d: %u, %u = %u, %u | %u\n", len, j, j + len, m, n, k);
#endif
            }
        }
    }
}

void pointwise_barrett(uint16_t c[FALCON_N],
                       const uint16_t a[FALCON_N],
                       const uint16_t b[FALCON_N])
{
    for (unsigned i = 0; i < FALCON_N; ++i)
    {
        c[i] = ((uint32_t)a[i] * b[i]) % FALCON_Q;
    }
}

void invntt(uint16_t a[FALCON_N])
{
    unsigned int start, len, j, k;
    uint16_t t, zeta, w;
    uint16_t m, n;

#if FALCON_N == 256
    const uint32_t f = 12241; // pow(256, -1, 12289)
#elif FALCON_N == 512
    const uint32_t f = 12265; // pow(512, -1, 12289)
#elif FALCON_N == 1024
    const uint32_t f = 12277; // pow(1024, -1, 12289)
#else
#error "See config.h, FALCON_N is not supported"
#endif

    k = FALCON_N;
    for (len = 1; len < FALCON_N; len <<= 1)
    {
        for (start = 0; start < FALCON_N; start = j + len)
        {
            // Plus Q so it is alway positive
            zeta = FALCON_Q - zetas_barrett[--k];
            for (j = start; j < start + len; ++j)
            {
                t = a[j];
                a[j] = (t + a[j + len]) % FALCON_Q;
                w = (t + FALCON_Q - a[j + len]) % FALCON_Q;
                a[j + len] = ((uint32_t)zeta * w) % FALCON_Q;

                m = a[j];
                n = a[j + len];
#if DEBUG == 5
                printf("%d: %u, %u = %u, %u | %u\n", len, j, j + len, m, n, k);
#endif
            }
        }
    }

    for (j = 0; j < FALCON_N; ++j)
    {
        // This work, but f is not same as in ref code
        a[j] = ((uint32_t)f * a[j]) % FALCON_Q;
    }
}
