#include <stdio.h>
#include "ref_ntt2x2.h"
#include "../consts.h"

#define DEBUG 0

// ================ FORWARD NTT 2x2 ========================

#define ctbf(a, b, z, t)               \
    t = ((data2_t)b * z) % FALCON_Q;  \
    b = (a + FALCON_Q - t) % FALCON_Q; \
    a = (a + t) % FALCON_Q;

void ntt2x2_ref(data_t a[FALCON_N])
{
    data_t len;
    data_t zeta1, zeta2[2];
    data_t a1, b1, a2, b2;
    data_t t1, t2;
    data_t k1, k2[2];

    for (int l = FALCON_LOGN; l > 0; l -= 2)
    {
        len = 1 << (l - 2);
        for (unsigned i = 0; i < FALCON_N; i += 1 << l)
        {
            k1 = (FALCON_N + i) >> l;
            k2[0] = (FALCON_N + i) >> (l - 1);
            k2[1] = k2[0] + 1;
            zeta1 = zetas_barrett[k1];
            zeta2[0] = zetas_barrett[k2[0]];
            zeta2[1] = zetas_barrett[k2[1]];

            for (unsigned j = i; j < i + len; j++)
            {
                a1 = a[j];
                a2 = a[j + len];
                b1 = a[j + 2 * len];
                b2 = a[j + 3 * len];

                // Left
                // a1 - b1, a2 - b2
                ctbf(a1, b1, zeta1, t1);
                ctbf(a2, b2, zeta1, t2);
#if DEBUG == 1
                printf("[%d]: %u, %u = %u, %u | %u\n", 2 * len, j, j + 2 * len,
                       a1, b1, k1);
                printf("[%d]: %u, %u = %u, %u | %u\n", 2 * len, j + len, j + 3 * len,
                       a2, b2, k1);
#endif

                // Right
                // a1 - a2, b1 - b2
                ctbf(a1, a2, zeta2[0], t1);
                ctbf(b1, b2, zeta2[1], t2);

#if DEBUG == 2
                printf("[%d]: %u, %u = %u, %u | %u\n", len, j, j + len,
                       a1, a2, k2[0]);
                printf("[%d]: %u, %u = %u, %u | %u\n", len, j + 2 * len, j + 3 * len,
                       b1, b2, k2[1]);
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

// ================ INVERSE NTT 2x2 ========================

#define gsbf(a, b, z, t)               \
    t = (a + FALCON_Q - b) % FALCON_Q; \
    a = (a + b) % FALCON_Q;            \
    b = ((data2_t)t * z) % FALCON_Q;

#define div2(t) ((t & 1) ? ((t >> 1) + (FALCON_Q + 1) / 2) : (t >> 1))

#define gsbf_div2(a, b, z, t)          \
    t = (a + FALCON_Q - b) % FALCON_Q; \
    t = div2(t);                       \
    a = (a + b) % FALCON_Q;            \
    a = div2(a);                       \
    b = ((data2_t)t * z) % FALCON_Q;

void invntt2x2_ref(data_t a[FALCON_N])
{
    data_t len;
    data_t a1, b1, a2, b2;
    data_t t1, t2;
    data_t k1[2], k2;
    data_t zeta1[2], zeta2;

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
#if DEBUG == 3
                printf("[%d]: %u, %u = %u, %u | %u\n", len, j, j + len,
                       a1, a2, k1[0]);
                printf("[%d]: %u, %u = %u, %u | %u\n", len, j + 2 * len, j + 3 * len,
                       b1, b2, k1[1]);
#endif

                // Right
                // a1 - b1, a2 - b2
                gsbf_div2(a1, b1, zeta2, t1);
                gsbf_div2(a2, b2, zeta2, t2);
#if DEBUG == 4
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
