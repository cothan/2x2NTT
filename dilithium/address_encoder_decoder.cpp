#include "params.h"
#include <stdio.h>
#include <stdint.h>

/* Lazy, avoid transpose the matrix */
int addr_decoder(int addr_in)
{
    /* For Inverse NTT
    [ 0  4  8 12] <= [  0* 1  2  3 ]
    [16 20 24 28] <= [  4  5  6  7 ]
    [32 36 40 44] <= [  8  9 10 11 ]
    [48 52 56 60] <= [ 12 13 14 15 ]

    [ 1  5  9 13] <= [ 16 17  18 19 ]
    [17 21 25 29] <= [ 20 21* 22 23 ]
    [33 37 41 45] <= [ 24 25  26 27 ]
    [49 53 57 61] <= [ 28 29  30 31 ]

    [ 2  6 10 14] <= [ 32 33 34  35 ]
    [18 22 26 30] <= [ 36 37 38  39 ]
    [34 38 42 46] <= [ 40 41 42* 43 ]
    [50 54 58 62] <= [ 44 45 46  47 ]

    [ 3  7 11 15] <= [ 48 49 50 51 ]
    [19 23 27 31] <= [ 52 53 54 55 ]
    [35 39 43 47] <= [ 56 57 58 59 ]
    [51 55 59 63] <= [ 60 61 62 63*] 
    */
    const int invntt_map[DILITHIUM_N / 4] = {
        0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60,
        1, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61,
        2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62,
        3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 59, 63};
    return invntt_map[addr_in];
}

int addr_encoder(int addr_in)
{
    /* For Forward NTT
    [  0  16  32  48 ] <= [  0* 1    2   3 ]
    [  1  17  33  49 ] <= [  4  5    6   7 ]
    [  2  18  34  50 ] <= [  8  9   10  11 ]
    [  3  19  35  51 ] <= [ 12  13  14  15 ]
    [  4  20  36  52 ] <= [ 16  17  18  19 ]
    [  5  21  37  53 ] <= [ 20  21* 22  23 ]
    [  6  22  38  54 ] <= [ 24  25  26  27 ]
    [  7  23  39  55 ] <= [ 28  29  30  31 ]
    [  8  24  40  56 ] <= [ 32  33  34  35 ]
    [  9  25  41  57 ] <= [ 36  37  38  39 ]
    [ 10  26  42  58 ] <= [ 40  41  42* 43 ]
    [ 11  27  43  59 ] <= [ 44  45  46  47 ]
    [ 12  28  44  60 ] <= [ 48  49  50  51 ]
    [ 13  29  45  61 ] <= [ 52  53  54  55 ]
    [ 14  30  46  62 ] <= [ 56  57  58  59 ]
    [ 15  31  47  63 ] <= [ 60  61  62  63*]
     */
    const int fwdntt_map[DILITHIUM_N / 4] = {
        0, 16, 32, 48, 1, 17, 33, 49, 2, 18, 34, 50, 3, 19, 35, 51,
        4, 20, 36, 52, 5, 21, 37, 53, 6, 22, 38, 54, 7, 23, 39, 55,
        8, 24, 40, 56, 9, 25, 41, 57, 10, 26, 42, 58, 11, 27, 43, 59,
        12, 28, 44, 60, 13, 29, 45, 61, 14, 30, 46, 62, 15, 31, 47, 63};
    return fwdntt_map[addr_in];
}

/* 
 * Figure out how to compute address decoder/encoder on fly. 
 * However, for N=256, it's better to use table-based approach since it's fit in 1 LUT. 
 * The on ly computation should take more than LUT. 
 * Modulo can be replace by AND operation, divide can be replace by right shift as well. 
 */
int resolve_address(enum MAPPING mapping, int addr)
{
    int ram_i;
    switch (mapping)
    {
    case DECODE_TRUE:
        // ram_i = addr_decoder(addr);
        ram_i = (addr % 16)*4 + addr/16;
        break;

    case ENCODE_TRUE:
        // ram_i = addr_encoder(addr);
        ram_i = (addr % 4)*16 + addr/4;
        break;

    default:
        ram_i = addr;
        break;
    }
    return ram_i;
}

void resolve_twiddle(int32_t tw_i[4], int *last, int32_t tw_base_i[4],
                    const int k, const int s, enum OPERATION mode)
{
    int l1, l2, l3, l4;
    int l1_base, l2_base, l3_base, l4_base;
    int t_last = *last;
    if (mode == INVERSE_NTT_MODE)
    {
        l1_base = (DILITHIUM_N >> s) - 1;
        l2_base = (DILITHIUM_N >> s) - 2;
        l3_base = l4_base = (DILITHIUM_N >> (s + 1)) - 1;

        // INVERSE_NTT_MODE
        // Layer s
        l1 = l1_base;
        l2 = l2_base;
        // Layer s + 1
        l3 = l3_base;
        l4 = l4_base;

        if (k == 0)
        {
            tw_i[0] = l1;
            tw_i[1] = l2;
            tw_i[2] = l3;
            tw_i[3] = l4;

            tw_base_i[0] = l1_base;
            tw_base_i[1] = l2_base;
            tw_base_i[2] = l3_base;
            tw_base_i[3] = l4_base;
        }
    }
    else if (mode == FORWARD_NTT_MODE)
    {
        l1_base = l2_base = 1 << s;
        l3_base = (1 << (s + 1));
        l4_base = (1 << (s + 1)) + 1;

        // FORWARD_NTT_MODE
        // Layer s
        l1 = l1_base;
        l2 = l2_base;
        // Layer s + 1
        l3 = l3_base;
        l4 = l4_base;

        if (s < 6 && k == 0)
        {
            tw_i[0] = l1;
            tw_i[1] = l2;
            tw_i[2] = l3;
            tw_i[3] = l4;

            tw_base_i[0] = l1_base;
            tw_base_i[1] = l2_base;
            tw_base_i[2] = l3_base;
            tw_base_i[3] = l4_base;
        }
        // FORWARD_NTT_MODE
        else if (s >= 6 && !t_last)
        {
            *last = 1;

            tw_i[0] = l1;
            tw_i[1] = l2;
            tw_i[2] = l3;
            tw_i[3] = l4;

            tw_base_i[0] = l1_base;
            tw_base_i[1] = l2_base;
            tw_base_i[2] = l3_base;
            tw_base_i[3] = l4_base;
        }
    }
}