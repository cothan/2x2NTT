#include "params.h"
#include "butterfly_unit.h"
#include "reduce.h"

void butterfly(int mode, u24 *bj, u24 *bjlen,
               const u24 zeta, const u24 aj, const u24 ajlen)
{
#pragma HLS INLINE
#pragma HLS PIPELINE II=1
    static u24 aj1, ajlen1;
    static u24 aj2, ajlen2;
    static u24 aj3, ajlen3;
    static u24 aj4, ajlen4;
    static u24 aj5, ajlen5;
    static u24 aj6, ajlen6;

    aj1 = aj;
    ajlen1 = ajlen;

    if (mode == INVERSE_NTT_MODE)
    {
        // INVERSE_NTT
        aj2 = aj1 + ajlen1;
        ajlen2 = aj1 - ajlen1;
    }
    else
    {
        // Forward
        aj2 = aj1;
        ajlen2 = ajlen1;
    }

    // MUL
    ajlen3 = barrett_reduction(zeta, ajlen2);
    aj3 = aj2;

    if (mode == FORWARD_NTT_MODE)
    {
        // FORWARD_NTT
        ajlen4 = aj3 - ajlen3;
        aj4 = aj3 + ajlen3;
    }
    else
    {
        // Forward
        ajlen4 = ajlen3;
        aj4 = aj3;
    }

    if (mode == INVERSE_NTT_MODE)
    {
        // Divide by two, only in INVERSE_NTT_MODE
        if (aj4.test(0))
        {
            // Odd number
            aj5 = (aj4 >> 1) + (DILITHIUM_Q + 1) / 2;
        }
        else
        {
            // Even number
            aj5 = (aj4 >> 1);
        }

        if (ajlen4.test(0))
        {
            // Odd number
            ajlen5 = (ajlen4 >> 1) + (DILITHIUM_Q + 1) / 2;
        }
        else
        {
            // Even number
            ajlen5 = (ajlen4 >> 1);
        }
    }
    else
    {
        // Forward
        aj5 = aj4;
        ajlen5 = ajlen4;
    }

    aj6 = (aj5 > DILITHIUM_Q) ? aj5 -= DILITHIUM_Q : aj5;
    ajlen6 = (ajlen5 > DILITHIUM_Q) ? ajlen5 -= DILITHIUM_Q : ajlen5;

    *bj = aj6;
    *bjlen = ajlen6;
}

void buttefly_circuit(u24 *a, u24 *b, u24 *c, u24 *d,
                      const u24 w1, const u24 w2,
                      const u24 w3, const u24 w4,
                      const int mode)
{
#pragma HLS INLINE
#pragma HLS PIPELINE II=1
    // 4 pipeline stages
    static u24 save_b, save_d;
    static u24 a0, b0, c0, d0;
    static u24 a1, b1, c1, d1;
    static u24 a2, b2, c2, d2;
    static u24 a3, b3, c3, d3;
    static u24 tw1, tw2, tw3, tw4;

    a0 = *a;
    b0 = *b;
    c0 = *c;
    d0 = *d;

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
        b2 = a2;
        d2 = c1;
    }
    else
    {
        b2 = c1;
        d2 = d1;
    }

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