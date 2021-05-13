#include <stdint.h>
#include <stdio.h>
#include "fifo.h"
#include "params.h"
#include "ntt2x2.h"

void read_fifo(int32_t *fa, int32_t *fb,
               int32_t *fc, int32_t *fd,
               const int count,
               const int32_t fifo_a[DEPT_A],
               const int32_t fifo_b[DEPT_B],
               const int32_t fifo_c[DEPT_C],
               const int32_t fifo_d[DEPT_D])
{
    int32_t ta, tb, tc, td;

    // Serial in Parallel out
    switch (count & 3)
    {
    case 0:
        ta = fifo_a[DEPT_A - 1];
        tb = fifo_a[DEPT_A - 2];
        tc = fifo_a[DEPT_A - 3];
        td = fifo_a[DEPT_A - 4];
        break;

    case 2:
        ta = fifo_b[DEPT_B - 1];
        tb = fifo_b[DEPT_B - 2];
        tc = fifo_b[DEPT_B - 3];
        td = fifo_b[DEPT_B - 4];
        break;
    case 1:
        ta = fifo_c[DEPT_C - 1];
        tb = fifo_c[DEPT_C - 2];
        tc = fifo_c[DEPT_C - 3];
        td = fifo_c[DEPT_C - 4];
        break;
    default:
        ta = fifo_d[DEPT_D - 1];
        tb = fifo_d[DEPT_D - 2];
        tc = fifo_d[DEPT_D - 3];
        td = fifo_d[DEPT_D - 4];
        break;
    }

    *fa = ta; //fout[0];
    *fb = tb; //fout[1];
    *fc = tc; //fout[2];
    *fd = td; //fout[3];
}

#include "util.h"


void write_fifo(int32_t data_in[4], const int32_t data_fifo[4],
                int32_t fifo_a[DEPT_A], int32_t fifo_b[DEPT_B],
                int32_t fifo_c[DEPT_C], int32_t fifo_d[DEPT_D],
                const int count)
{
    int32_t fa, fb, fc, fd;
    bool a_en = false, b_en = false, c_en = false, d_en = false;

    switch (count & 3)
    {
        // Use PISO to write
    case 0:
        // Write to FIFO_D
        d_en = true;
        break;
    case 1:
        // Write to FIFO_B
        b_en = true;
        break;
    case 2:
        // Write to FIFO_C
        c_en = true;
        break;

    default:
        // Write to FIFO_A
        a_en = true;
        break;
    }
    // a_en &= (mode == FORWARD_NTT_MODE);
    // b_en &= (mode == FORWARD_NTT_MODE);
    // c_en &= (mode == FORWARD_NTT_MODE);
    // d_en &= (mode == FORWARD_NTT_MODE);

    fd = FIFO_PISO<DEPT_A>(fifo_a, a_en, data_fifo);
    fb = FIFO_PISO<DEPT_B>(fifo_b, b_en, data_fifo);
    fc = FIFO_PISO<DEPT_C>(fifo_c, c_en, data_fifo);
    fa = FIFO_PISO<DEPT_D>(fifo_d, d_en, data_fifo);

    data_in[0] = fa;
    data_in[1] = fc;
    data_in[2] = fb;
    data_in[3] = fd;
}