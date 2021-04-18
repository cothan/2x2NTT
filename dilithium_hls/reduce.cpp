#include "params.h"
#include "reduce.h"

u23 barrett_reduction(const u23 a, const u23 b)
{
#pragma HLS BIND_OP variable=a op=mul impl=dsp
    u46 s = (u46)a * b;
    u23 z = s(22, 0);
    u12 x = s(45, 43) + s(42, 33) + s(32, 23);
    u25 y = s(45, 43) + s(45, 33) + s(45, 23);
    i25 r = (x(11, 10) + x(9, 0), (u13) 0) - (y + x(11, 10)) + z;

    if (r < 0)
    {
        r += DILITHIUM_Q;
    }
    else if (r > DILITHIUM_Q)
    {
        r -= DILITHIUM_Q;
    }

    // Only return 23-bit unsign number
    return r(22, 0);
}
