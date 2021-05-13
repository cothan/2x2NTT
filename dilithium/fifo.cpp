#include <stdint.h>
#include <stdio.h>
#include "fifo.h"
#include "params.h"
#include "ntt2x2.h"

/* This function will output an element when insert 1 element
 */

int32_t FIFO_I(const int dept, int32_t *fifo,
               const int32_t new_value, enum OPERATION mode)
{
    int32_t out;
    if (mode == FORWARD_NTT_MODE)
    {
        out = fifo[dept - 1];
    }
    else
    {
        out = fifo[dept - 2];
    }

    for (int i = dept - 1; i > 0; i--)
    {
        fifo[i] = fifo[i - 1];
    }
    fifo[0] = new_value;

    return out;
}

template <int DEPT>
void read_last_fifo(int32_t fout[4], const int32_t fifo[DEPT])
{
    for (auto j = 0; j < 4; ++j)
    {
#pragma HLS UNROLL
        fout[j] = fifo[DEPT - 1 - j];
    }
}

void read_fifo(int32_t *fa, int32_t *fb,
               int32_t *fc, int32_t *fd,
               const int count,
               const int32_t fifo_a[DEPT_A],
               const int32_t fifo_b[DEPT_B],
               const int32_t fifo_c[DEPT_C],
               const int32_t fifo_d[DEPT_D])
{
    int32_t ta, tb, tc, td;
    // int32_t fout[4];

    // Serial in Parallel out
    switch (count & 3)
    {
    case 0:
        ta = fifo_a[DEPT_A - 1];
        tb = fifo_a[DEPT_A - 2];
        tc = fifo_a[DEPT_A - 3];
        td = fifo_a[DEPT_A - 4];
        // read_last_fifo<DEPT_A>(fout, fifo_a);
        break;

    case 2:
        ta = fifo_b[DEPT_B - 1];
        tb = fifo_b[DEPT_B - 2];
        tc = fifo_b[DEPT_B - 3];
        td = fifo_b[DEPT_B - 4];
        // read_last_fifo<DEPT_B>(fout, fifo_b);
        break;
    case 1:
        ta = fifo_c[DEPT_C - 1];
        tb = fifo_c[DEPT_C - 2];
        tc = fifo_c[DEPT_C - 3];
        td = fifo_c[DEPT_C - 4];
        // read_last_fifo<DEPT_C>(fout, fifo_c);
        break;
    case 3:
        ta = fifo_d[DEPT_D - 1];
        tb = fifo_d[DEPT_D - 2];
        tc = fifo_d[DEPT_D - 3];
        td = fifo_d[DEPT_D - 4];
        // read_last_fifo<DEPT_D>(fout, fifo_d);
        break;

    default:
        printf("Error, suspect overflow\n");
        break;
    }

    *fa = ta; //fout[0];
    *fb = tb; //fout[1];
    *fc = tc; //fout[2];
    *fd = td; //fout[3];
}

#include "util.h"

void write_fifo(int32_t *a, int32_t *b, int32_t *c, int32_t *d,
                int32_t fifo_a[DEPT_A], int32_t fifo_b[DEPT_B],
                int32_t fifo_c[DEPT_C], int32_t fifo_d[DEPT_D],
                const int count, enum OPERATION mode,
                const bram *ram, const int index)
{
    int32_t fa, fb, fc, fd;
    int32_t ta, tb, tc, td;
    int32_t a_en = 0, b_en = 0, c_en = 0, d_en = 0;
    const int32_t *line = ram->coeffs[index];

    ta = *a;
    tb = *b;
    tc = *c;
    td = *d;

    switch (count & 3)
    {
        // Use PISO to write
    case 0:
        // Write to FIFO_D
        d_en = 1;
        break;
    case 1:
        // Write to FIFO_B
        b_en = 1;
        break;
    case 2:
        // Write to FIFO_C
        c_en = 1;
        break;

    default:
        // Write to FIFO_A
        a_en = 1;
        break;
    }
    a_en &= (mode == FORWARD_NTT_MODE);
    b_en &= (mode == FORWARD_NTT_MODE);
    c_en &= (mode == FORWARD_NTT_MODE);
    d_en &= (mode == FORWARD_NTT_MODE);

    fd = FIFO_PISO<DEPT_A>(fifo_a, a_en, ta, line);
    fb = FIFO_PISO<DEPT_B>(fifo_b, b_en, tb, line);
    fc = FIFO_PISO<DEPT_C>(fifo_c, c_en, tc, line);
    fa = FIFO_PISO<DEPT_D>(fifo_d, d_en, td, line);

    *a = fa;
    *b = fc;
    *c = fb;
    *d = fd;
}

void write_fifo(int32_t data_fifo[4], const int32_t data_out[4],
                int32_t fifo_a[DEPT_A], int32_t fifo_b[DEPT_B],
                int32_t fifo_c[DEPT_C], int32_t fifo_d[DEPT_D],
                const int count, enum OPERATION mode,
                const bram *ram, const int index)
{
    int32_t fa, fb, fc, fd;
    int32_t ta, tb, tc, td;
    bool a_en = false, b_en = false, c_en = false, d_en = false;
    const int32_t *line = ram->coeffs[index];

    ta = data_out[0];
    tb = data_out[1];
    tc = data_out[2];
    td = data_out[3];

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
    a_en &= (mode == FORWARD_NTT_MODE);
    b_en &= (mode == FORWARD_NTT_MODE);
    c_en &= (mode == FORWARD_NTT_MODE);
    d_en &= (mode == FORWARD_NTT_MODE);

    fd = FIFO_PISO<DEPT_A>(fifo_a, a_en, ta, line);
    fb = FIFO_PISO<DEPT_B>(fifo_b, b_en, tb, line);
    fc = FIFO_PISO<DEPT_C>(fifo_c, c_en, tc, line);
    fa = FIFO_PISO<DEPT_D>(fifo_d, d_en, td, line);

    data_fifo[0] = fa;
    data_fifo[1] = fc;
    data_fifo[2] = fb;
    data_fifo[3] = fd;
}