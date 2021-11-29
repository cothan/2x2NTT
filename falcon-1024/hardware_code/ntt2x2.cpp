#include "../params.h"
#include "ntt.h"
#include "ntt2x2.h"
#include "util.h"
#include "fifo.h"
#include "consts.h"
#include "address_encoder_decoder.h"
#include "ram_util.h"
#include "butterfly_unit.h"
#include <cstdio>
#include <cstring>


template <typename T>
const T MAX(const T a, const T b)
{
    return (a < b) ? b : a; // or: return comp(a,b)?b:a; for version (2)
}

void update_indexes(unsigned tw_i[4],
                    const unsigned tw_base_i[4],
                    const unsigned s, enum OPERATION mode)
{
    unsigned mask1, mask2;
    const unsigned w_m1 = 2;
    const unsigned w_m2 = 1;
    unsigned l1, l2, l3, l4;

    mask1 = (2 << s) - 1;
    mask2 = (2 << (s + 1)) - 1;

    l1 = tw_i[0];
    l2 = tw_i[1];
    l3 = tw_i[2];
    l4 = tw_i[3];

    // Adjust address
    if (mode == INVERSE_NTT_MODE)
    {
        // Only adjust omega in NTT mode
        l1 -= w_m1;
        l2 -= w_m1;
        l3 -= w_m2;
        l4 -= w_m2;
    }
    else if (mode == FORWARD_NTT_MODE)
    {
        if (s < (FALCON_LOGN - 2))
        {
            l1 = MAX<unsigned>(tw_base_i[0], (l1 + 1) & mask1);
            l2 = MAX<unsigned>(tw_base_i[1], (l2 + 1) & mask1);
            l3 = MAX<unsigned>(tw_base_i[2], (l3 + 2) & mask2);
            l4 = MAX<unsigned>(tw_base_i[3], (l4 + 2) & mask2);
        }
        else
        {
            l1 += w_m2;
            l2 += w_m2;
            l3 += w_m1;
            l4 += w_m1;
        }
    }
    tw_i[0] = l1;
    tw_i[1] = l2;
    tw_i[2] = l3;
    tw_i[3] = l4;
}
