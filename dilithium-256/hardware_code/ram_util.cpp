#include "../consts.h"
#include "config.h"
#include <stdio.h>

void read_ram(data_t data_out[4], const bram *ram, const unsigned ram_i)
{
    data_out[0] = ram->coeffs[ram_i][0];
    data_out[1] = ram->coeffs[ram_i][1];
    data_out[2] = ram->coeffs[ram_i][2];
    data_out[3] = ram->coeffs[ram_i][3];
}

void write_ram(bram *ram, const unsigned ram_i, const data_t data_in[4])
{
    // printf("[%d] < [%d, %d, %d, %d]\n", ram_i, a, b, c, d);
    ram->coeffs[ram_i][0] = data_in[0];
    ram->coeffs[ram_i][1] = data_in[1];
    ram->coeffs[ram_i][2] = data_in[2];
    ram->coeffs[ram_i][3] = data_in[3];
}

void read_twiddle(data_t data_out[4], enum OPERATION mode, const unsigned tw_i[4])
{
    unsigned i1 = tw_i[0];
    unsigned i2 = tw_i[1];
    unsigned i3 = tw_i[2];
    unsigned i4 = tw_i[3];
    switch (mode)
    {
    case FORWARD_NTT_MODE:
        data_out[0] = zetas_barrett[i1];
        data_out[1] = zetas_barrett[i2];
        data_out[2] = zetas_barrett[i3];
        data_out[3] = zetas_barrett[i4];
        break;

    case INVERSE_NTT_MODE:
        data_out[0] = -zetas_barrett[i1];
        data_out[1] = -zetas_barrett[i2];
        data_out[2] = -zetas_barrett[i3];
        data_out[3] = -zetas_barrett[i4];
        break;

    default:
        printf("Not supported\n");
        break;
    }
}