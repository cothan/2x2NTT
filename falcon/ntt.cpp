#include <stdint.h>
#include <stdio.h>
#include "params.h"
#include "ntt.h"
#include "consts.h"

extern inline uint16_t
mq_sub_test(uint16_t x, uint16_t y)
{
    int16_t d;
    d = x - y;
    if (d < 0)
        d += FALCON_Q;
    return d;
}

static inline uint16_t
barret_mul(uint16_t x, uint16_t y)
{
    uint32_t z;
    z = x * y;
    z = z % FALCON_Q;
    return z;
}

/*************************************************
* Name:        ntt
*
* Description: Forward NTT, in-place. No modular reduction is performed after
*              additions or subtractions. Output vector is in bitreversed order.
*
* Arguments:   - uint16_t p[N]: input/output coefficient array
**************************************************/
void ntt(uint16_t a[FALCON_N])
{
    unsigned int len, start, j, k;
    uint16_t zeta, t;

    k = 0;
    for (len = FALCON_N / 2; len > 0; len >>= 1)
    {
        for (start = 0; start < FALCON_N; start = j + len)
        {
            zeta = zetas_barrett[++k];
            for (j = start; j < start + len; ++j)
            {
                t = barret_mul(zeta, a[j + len]);
                a[j + len] = mq_sub_test(a[j], t);
                a[j] = (a[j] + t) % FALCON_Q;
            }
        }
    }
}

/*************************************************+
* Name:        invntt_tomont
*
* Description: Inverse NTT and multiplication by Montgomery factor 2^32.
*              In-place. No modular reductions after additions or
*              subtractions; input coefficients need to be smaller than
*              Q in absolute value. Output coefficient are smaller than Q in
*              absolute value.
*
* Arguments:   - uint32_t p[N]: input/output coefficient array
**************************************************/
void invntt(uint16_t a[FALCON_N])
{
    unsigned int start, len, j, k;
    uint16_t t, zeta, w;

#if FALCON_N == 256
    const uint16_t f = 256;
#elif FALCON_N == 512
    const uint16_t f = 128;
#elif FALCON_N == 1024
    const uint16_t f = 64;
#else
#error "See config.h, FALCON_N is not supported"
#endif

    k = FALCON_N;
    for (len = 1; len < FALCON_N; len <<= 1)
    {
        for (start = 0; start < FALCON_N; start = j + len)
        {
            // Plus Q so it is alway positive
            zeta = FALCON_Q -zetas_barrett[--k];
            for (j = start; j < start + len; ++j)
            {
                t = a[j];
                a[j] = (t + a[j + len]) % FALCON_Q;
                // w = mq_sub_test(t, a[j + len]);
                w = (t + FALCON_Q - a[j + len]) % FALCON_Q;
                a[j + len] = barret_mul(zeta, w);
            }
        }
        // if (len == 2)
        // {
        //     for (int i = 0; i < 16; i++)
        //     {
        //         printf("%u, ", a[i]);
        //     }
        //     printf("\n");
        // }
    }

    // f is multiple of 2, so shift and reduction
    // for (j = 0; j < FALCON_N; ++j)
    // {
    //     a[j] = barret_mul(f, a[j]);
    // }
}

/*************************************************
* Name:        pointwise_montgomery
*
* Description: Pointwise multiplication of polynomials in NTT domain
*              representation and multiplication of resulting polynomial
*              by 2^{-32}.
*
* Arguments:   - poly *c: pointer to output polynomial
*              - const poly *a: pointer to first input polynomial
*              - const poly *b: pointer to second input polynomial
**************************************************/
void pointwise_montgomery(uint16_t *c, uint16_t *a, const uint16_t *b)
{
    unsigned int i;
    for (i = 0; i < FALCON_N; ++i)
        c[i] = ((uint32_t)a[i] * b[i]) % FALCON_Q;
}
