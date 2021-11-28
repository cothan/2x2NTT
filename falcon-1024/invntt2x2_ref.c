#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "params.h"
#include "consts.h"

#define DEBUG 0

#define gsbf(a, b, z, t)               \
    t = (a + FALCON_Q - b) % FALCON_Q; \
    a = (a + b) % FALCON_Q;            \
    b = ((uint32_t)t * z) % FALCON_Q;

#define div2(t) ((t & 1) ? ((t >> 1) + (FALCON_Q + 1) / 2) : (t >> 1))

#define gsbf_div2(a, b, z, t)          \
    t = (a + FALCON_Q - b) % FALCON_Q; \
    t = div2(t);                       \
    a = (a + b) % FALCON_Q;            \
    a = div2(a);                       \
    b = ((uint32_t)t * z) % FALCON_Q;

void invntt2x2_ref(uint16_t a[FALCON_N])
{
    uint16_t len;
    uint16_t a1, b1, a2, b2;
    uint16_t t1, t2;
    uint16_t k1[2], k2;
    uint16_t zeta1[2], zeta2;

    for (int l = 0; l < FALCON_LOGN - (FALCON_LOGN & 1); l += 2)
    {
        len = 1 << l;
        for (unsigned i = 0; i < FALCON_N; i += 1 << (l + 2))
        {
            k1[0] = ((FALCON_N - i / 2) >> l) - 1;
            k1[1] = k1[0] - 1;
            k2 = ((FALCON_N - i / 2) >> (l + 1)) - 1;
            zeta1[0] = FALCON_Q - zetas_barrett[k1[0]];
            zeta1[1] = FALCON_Q - zetas_barrett[k1[1]];
            zeta2 = FALCON_Q - zetas_barrett[k2];

            for (unsigned j = i; j < i + len; j++)
            {
                a1 = a[j];
                a2 = a[j + len];
                b1 = a[j + 2 * len];
                b2 = a[j + 3 * len];

                // Left
                // a1 - a2, b1 - b2
                gsbf_div2(a1, a2, zeta1[0], t1);
                gsbf_div2(b1, b2, zeta1[1], t2);
#if DEBUG == 1
                printf("[%d]: %u, %u = %u, %u | %u\n", len, j, j + len,
                       a1, a2, k1[0]);
                printf("[%d]: %u, %u = %u, %u | %u\n", len, j + 2 * len, j + 3 * len,
                       b1, b2, k1[1]);
#endif

                // Right
                // a1 - b1, a2 - b2
                gsbf_div2(a1, b1, zeta2, t1);
                gsbf_div2(a2, b2, zeta2, t2);
#if DEBUG == 1
                printf("[%d]: %u, %u = %u, %u | %u\n", 2 * len, j, j + 2 * len,
                       a1, b1, k2);
                printf("[%d]: %u, %u = %u, %u | %u\n", 2 * len, j + len, j + 3 * len,
                       a2, b2, k2);
#endif
                a[j] = a1;
                a[j + len] = a2;
                a[j + 2 * len] = b1;
                a[j + 3 * len] = b2;
            }
        }
    }
    // End function
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

#define TESTS 100000

int main()
{
    uint16_t a[FALCON_N] = {0}, a_gold[FALCON_N] = {0};
    uint16_t tmp;
    srand(0);
    for (int j = 0; j < TESTS; j++)
    {
        // Test million times
        for (int i = 0; i < FALCON_N; i++)
        {
            tmp = rand() % 0xffff;
            a[i] = tmp;
            a_gold[i] = tmp;
        }

        invntt2x2_ref(a);
        // printf("=======\n");
        invntt(a_gold);

        for (int i = 0; i < FALCON_N; i++)
        {
            if (a_gold[i] != a[i])
            {
                printf("%d: %u != %u\n", i, a_gold[i], a[i]);
                return 1;
            }
        }
    }
    return 0;
}
// Compile flags:
// gcc -o invntt2x2_ref consts.cpp invntt2x2_ref.c -O3; ./invntt2x2_ref
