#include <stdint.h>
#include <stdio.h>
#include "fifo.h"
#include "params.h"
#include "ntt2x2.h"

/* This function will output an element when insert 1 element
 */
int32_t FIFO(const int dept, int32_t *fifo, const int32_t new_value)
{
    int32_t out = fifo[dept - 1];
    for (int i = dept - 1; i > 0; i--)
    {
        fifo[i] = fifo[i - 1];
    }
    fifo[0] = new_value;

    return out;
}

int32_t FIFO_PISO(const int dept, int32_t *fifo, const int piso_en,
                  const int32_t new_value, const int32_t line[4])
{
    int32_t out = fifo[dept - 1];
    for (int i = dept - 1; i > 3; i--)
    {
        fifo[i] = fifo[i - 1];
    }
    if (piso_en)
    {
        fifo[3] = line[0];
        fifo[2] = line[1];
        fifo[1] = line[2];
        fifo[0] = line[3];
    }
    else
    {
        fifo[3] = fifo[2];
        fifo[2] = fifo[1];
        fifo[1] = fifo[0];
        fifo[0] = new_value;
    }
    return out;
}

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

/* Parallel in, serial out: This function receive 4 elements at the begin of FIFO. 
 */
static int32_t PISO(const int dept, int32_t *fifo, const int32_t *line)
{
    int32_t out = fifo[dept - 1];
    for (int i = dept - 1; i > 3; i--)
    {
        fifo[i] = fifo[i - 1];
    }
    fifo[3] = line[0];
    fifo[2] = line[1];
    fifo[1] = line[2];
    fifo[0] = line[3];

    return out;
}

void PIPO(int32_t out[4], const int dept, int32_t *fifo4, const int32_t w[4])
{
    out[0] = fifo4[dept - 1];
    out[1] = fifo4[dept - 2];
    out[2] = fifo4[dept - 3];
    out[3] = fifo4[dept - 4];

    for (int i = dept - 1; i > 3; i--)
    {
        // printf("%d <= %d\n", i, i - 1);
        fifo4[i] = fifo4[i - 4];
    }
    fifo4[3] = w[0];
    fifo4[2] = w[1];
    fifo4[1] = w[2];
    fifo4[0] = w[3];
}

void read_fifo(int32_t *fa, int32_t *fb,
               int32_t *fc, int32_t *fd,
               const int count, enum OPERATION mode,
               const int32_t fifo_a[DEPT_A],
               const int32_t fifo_b[DEPT_B],
               const int32_t fifo_c[DEPT_C],
               const int32_t fifo_d[DEPT_D])
{
    static int32_t ta, tb, tc, td;
    if (mode == INVERSE_NTT_MODE)
    {
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
        case 3:
            ta = fifo_d[DEPT_D - 1];
            tb = fifo_d[DEPT_D - 2];
            tc = fifo_d[DEPT_D - 3];
            td = fifo_d[DEPT_D - 4];
            break;

        default:
            printf("Error, suspect overflow\n");
            break;
        }
    }
    else
    {
        // MUL Mode
        ta = fifo_a[DEPT_A - 1];
        tb = fifo_b[DEPT_B - 3];
        tc = fifo_c[DEPT_C - 2];
        td = fifo_d[DEPT_D - 4];
    }

    *fa = ta;
    *fb = tb;
    *fc = tc;
    *fd = td;
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
    const int32_t *line = ram->vec[index].coeffs;

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

    fd = FIFO_PISO(DEPT_A, fifo_a, a_en, ta, line);
    fb = FIFO_PISO(DEPT_B, fifo_b, b_en, tb, line);
    fc = FIFO_PISO(DEPT_C, fifo_c, c_en, tc, line);
    fa = FIFO_PISO(DEPT_D, fifo_d, d_en, td, line);

    *a = fa;
    *b = fc;
    *c = fb;
    *d = fd;
}
