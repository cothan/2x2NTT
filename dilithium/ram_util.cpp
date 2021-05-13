#include <stdint.h>
#include <cstdio>
#include "ntt2x2.h"
#include "consts.h"

void read_ram(int32_t *a, int32_t *b, int32_t *c, int32_t *d,
              const bram *ram, const int ram_i)
{
    *a = ram->coeffs[ram_i][0];
    *b = ram->coeffs[ram_i][1];
    *c = ram->coeffs[ram_i][2];
    *d = ram->coeffs[ram_i][3];
}

void write_ram(bram *ram, const int ram_i,
               const int32_t a, const int32_t b,
               const int32_t c, const int32_t d)
{
    // printf("[%d] < [%d, %d, %d, %d]\n", ram_i, a, b, c, d);
    ram->coeffs[ram_i][0] = a;
    ram->coeffs[ram_i][1] = b;
    ram->coeffs[ram_i][2] = c;
    ram->coeffs[ram_i][3] = d;
}

void read_twiddle(int32_t *w1, int32_t *w2, int32_t *w3, int32_t *w4,
                    enum OPERATION mode, const int32_t tw_i[4])
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
