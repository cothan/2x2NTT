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
