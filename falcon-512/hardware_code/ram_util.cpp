#include "consts_hw.h"
#include "config.h"
#include <stdio.h>

void read_ram(data_t data_out[4], const bram *ram, const unsigned ram_i)
{
    data_out[0] = ram->coeffs[ram_i][0];
    data_out[1] = ram->coeffs[ram_i][1];
    data_out[2] = ram->coeffs[ram_i][2];
    data_out[3] = ram->coeffs[ram_i][3];
    // printf("[%d, %d, %d, %d] < [%d]\n",
    // data_out[0],
    // data_out[1],
    // data_out[2],
    // data_out[3], ram_i);
}

void write_ram(bram *ram, const unsigned ram_i, const data_t data_in[4])
{
    // printf("[%d] < [%d, %d, %d, %d]\n", ram_i,
    // data_in[0],
    // data_in[1],
    // data_in[2],
    // data_in[3]);
    ram->coeffs[ram_i][0] = data_in[0];
    ram->coeffs[ram_i][1] = data_in[1];
    ram->coeffs[ram_i][2] = data_in[2];
    ram->coeffs[ram_i][3] = data_in[3];
}

static unsigned int scale_twiddle(int level)
{
    const unsigned bar[] = {
        0,                                         // 0 - 1
        (1 << 0),                                  // 2 - 3
        (1 << 2) + (1 << 0),                       // 4 - 5
        (1 << 4) + (1 << 2) + (1 << 0),            // 6 - 7
        (1 << 6) + (1 << 4) + (1 << 2) + (1 << 0), // 8 - 9
    };
    return bar[level >> 1];
}

void get_twiddle_factors(data_t data_out[4], int i, int level, OPERATION mode)
{
    // Initialize to 0 just to slient compiler warnings
    unsigned i1 = 0, i2 = 0, i3 = 0, i4 = 0;
    unsigned index = 0, bar = 0, mask = 0;

    switch (mode)
    {
    case FORWARD_NTT_MODE:
        mask = (1 << level) - 1;
        bar = scale_twiddle(level);
        index = bar + (i & mask);

        i1 = i2 = 0;
        i3 = 1;
        i4 = 2;
        break;

    case INVERSE_NTT_MODE:
        mask = (1 << (FALCON_LOGN - 2 - level)) - 1;
        bar = scale_twiddle(FALCON_LOGN - 2 - level);
        index = bar + ((BRAM_DEPT - 1 - i) & mask);

        i1 = 2;
        i2 = 1;
        i3 = i4 = 0;
        break;

    case FORWARD_NTT_MODE_BYPASS:
        mask = (1 << level) - 1;
        bar = scale_twiddle(level);
        index = bar + (i & mask);

        i1 = i2 = 2; // 0 value
        i3 = 0;
        i4 = 1;
        break;

    case INVERSE_NTT_MODE_BYPASS:
        mask = (1 << (FALCON_LOGN - 2 - level)) - 1;
        bar = scale_twiddle(FALCON_LOGN - level);
        index = bar + ((BRAM_DEPT - 1 - i) & mask);

        i1 = 1;
        i2 = 0;
        i3 = i4 = 2; // 0 value
        break;

    default:
        break;
    }

    data_out[0] = zetas_barrett_hw[index][i1];
    data_out[1] = zetas_barrett_hw[index][i2];
    data_out[2] = zetas_barrett_hw[index][i3];
    data_out[3] = zetas_barrett_hw[index][i4];
}
