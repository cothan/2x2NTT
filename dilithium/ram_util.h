#ifndef RAM_UTIL_H
#define RAM_UTIL_H

#include "ntt2x2.h"
#include "consts.h"

template <typename T>
void read_ram(T *a, T *b, T *c, T *d,
              const bram *ram, const int ram_i)
{
    *a = ram->coeffs[ram_i][0];
    *b = ram->coeffs[ram_i][1];
    *c = ram->coeffs[ram_i][2];
    *d = ram->coeffs[ram_i][3];
}

template <typename T>
void write_ram(bram *ram, const int ram_i,
               const T a, const T b,
               const T c, const T d)
{
    // printf("[%d] < [%d, %d, %d, %d]\n", ram_i, a, b, c, d);
    ram->coeffs[ram_i][0] = a;
    ram->coeffs[ram_i][1] = b;
    ram->coeffs[ram_i][2] = c;
    ram->coeffs[ram_i][3] = d;
}

template <typename T>
void read_twiddle(T *w1, T *w2, T *w3, T *w4,
                    enum OPERATION mode, const int tw_i[4])
{
    auto i1 = tw_i[0];
    auto i2 = tw_i[1];
    auto i3 = tw_i[2];
    auto i4 = tw_i[3];
    switch (mode)
    {
    case FORWARD_NTT_MODE:
        *w1 = zetas_barret[i1];
        *w2 = zetas_barret[i2];
        *w3 = zetas_barret[i3];
        *w4 = zetas_barret[i4];
        break;

    case INVERSE_NTT_MODE:
        *w1 = -zetas_barret[i1];
        *w2 = -zetas_barret[i2];
        *w3 = -zetas_barret[i3];
        *w4 = -zetas_barret[i4];
        break;

    default:
        // Not support
        break;
    }
}

#endif