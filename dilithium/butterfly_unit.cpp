#include "params.h"
#include <stdint.h>

static 
void butterfly(int mode, int32_t *bj, int32_t *bjlen, const int32_t zeta,
                      const int32_t aj, const int32_t ajlen)
{
    static int32_t aj1, ajlen1;
    static int32_t aj2, ajlen2;
    static int32_t aj3, ajlen3;
    static int32_t aj4, ajlen4;
    static int32_t aj5, ajlen5;

    aj1 = aj;
    ajlen1 = ajlen;

    if (mode == INVERSE_NTT_MODE)
    {
        /* This code copied from butterfly unit in Dilithium Inverse NTT
        t = a[j];
        a[j] = t + a[j + len];
        a[j + len] = t - a[j + len];
        a[j + len] = montgomery_reduce((int64_t)zeta * a[j + len]); */
        // INV_NTT
        aj2 = aj1 + ajlen1;
        ajlen2 = aj1 - ajlen1;
    }
    else
    {
        aj2 = aj1;
        ajlen2 = ajlen1;
    }

    // MUL
    // t = ajlen = montgomery_reduce((int64_t)zeta * ajlen);
    ajlen3 = ((int64_t)zeta * ajlen2) % DILITHIUM_Q;
    aj3 = aj2;

    if (mode == FORWARD_NTT_MODE)
    {
        /* This code copied from butterfly unit in Dilithium Forward NTT
        t = montgomery_reduce((int64_t)zeta * a[j + len]);
        a[j + len] = a[j] - t;
        a[j] = a[j] + t; */
        // NTT
        ajlen4 = aj3 - ajlen3;
        aj4 = aj3 + ajlen3;
    }
    else
    {
        ajlen4 = ajlen3;
        aj4 = aj3;
    }

    if (mode == INVERSE_NTT_MODE)
    {
        if (aj4 & 1)
        {
          aj5 = (aj4 >> 1) + (DILITHIUM_Q + 1) / 2;
        }
        else
        {
          aj5 = (aj4 >> 1);
        }

        if (ajlen4 & 1)
        {
          ajlen5 = (ajlen4 >> 1) + (DILITHIUM_Q + 1) / 2;
        }
        else
        {
          ajlen5 = (ajlen4 >> 1);
        }
    }
    else
    {
        aj5 = aj4;
        ajlen5 = ajlen4;
    }

    *bj = (aj5 < 0) ? aj5 + DILITHIUM_Q : aj5;
    *bjlen = (ajlen5 < 0) ? ajlen5 + DILITHIUM_Q : ajlen5;
}

void buttefly_circuit(int32_t *a, int32_t *b,
                      int32_t *c, int32_t *d,
                      const int32_t w_fwd[4],
                      const int32_t w_imul[4],
                      const int mode)
{
    // 4 pipeline stages
    static int32_t save_b, save_d;
    static int32_t a0, b0, c0, d0;
    static int32_t a1, b1, c1, d1;
    static int32_t a2, b2, c2, d2;
    static int32_t a3, b3, c3, d3;
    static int32_t tw1, tw2, tw3, tw4;
    int32_t w1, w2, w3, w4;

    a0 = *a;
    b0 = *b;
    c0 = *c;
    d0 = *d;

    if (mode == FORWARD_NTT_MODE)
    {
        w1 = w_fwd[0];
        w2 = w_fwd[1];
        w3 = w_fwd[2];
        w4 = w_fwd[3];
    }
    else
    {
        w1 = w_imul[0];
        w2 = w_imul[1];
        w3 = w_imul[2];
        w4 = w_imul[3];
    }

    /* For debugging purpose
    if ((ram_i < 64 || ram_i > 192) && (s > 2))
    {
        // index
        printf("%d, %d | %d\n", ram_i, ram_i + 1, i1);
        printf("%d, %d | %d\n", ram_i + 2, ram_i + 3, i2);
        // value
        printf("%d %d | %d\n", a, b, i1);
        printf("%d %d | %d\n", c, d, i2);
    } */
    if (mode == MUL_MODE)
    {
        tw1 = w2;
        tw2 = w4;
    }
    else
    {
        tw1 = w1;
        tw2 = w2;
    }
    butterfly(mode, &a1, &b1, tw1, a0, b0);
    butterfly(mode, &c1, &d1, tw2, c0, d0);

    save_b = b1;
    save_d = d1;

    a2 = a1;
    c2 = b1;

    if (mode == MUL_MODE)
    {
        // switch lane A -> B, C->D
        b2 = a1;
        d2 = c1;
    }
    else
    {
        b2 = c1;
        d2 = d1;
    }

    /* For debugging purpose
    if ((ram_i < 64 || ram_i > 192) && (s > 2))
    {
        // index
        printf("%d, %d | %d\n", ram_i, ram_i + 2, i3);
        printf("%d, %d | %d\n", ram_i + 1, ram_i + 3, i4);
        // value
        printf("%d %d | %d\n", a, b, i3);
        printf("%d %d | %d\n", c, d, i4);
        printf("==============================%d %d | %d %d\n", ram_i / 4, ram_i, j, k);
    } */

    if (mode == MUL_MODE)
    {
        tw3 = w1;
        tw4 = w3;
    }
    else
    {
        tw3 = w3;
        tw4 = w4;
    }
    butterfly(mode, &a3, &b3, tw3, a2, b2);
    butterfly(mode, &c3, &d3, tw4, c2, d2);

    if (mode == MUL_MODE)
    {
        // switch lane again, B->A, D->C
        *a = b3;
        *b = save_b;
        *c = d3;
        *d = save_d;
    }
    else
    {
        // NTT Mode
        *a = a3;
        *b = b3;
        *c = c3;
        *d = d3;
    }
}
