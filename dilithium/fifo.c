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

void write_fifo(int32_t *a, int32_t *b, int32_t *c, int32_t *d,
                int32_t fifo_a[DEPT_A], int32_t fifo_b[DEPT_B],
                int32_t fifo_c[DEPT_C], int32_t fifo_d[DEPT_D],
                const int count,
                const bram *ram, const int index)
{
    int32_t fa, fb, fc, fd;
    switch (count & 3)
    {
        // Use PISO to write
    case 0:
        // Write to FIFO_D
        fa = PISO(DEPT_D, fifo_d, ram->vec[index].coeffs);
        fb = FIFO(DEPT_B, fifo_b, -1);
        fc = FIFO(DEPT_C, fifo_c, -1);
        fd = FIFO(DEPT_A, fifo_a, -1);
        break;
    case 1:
        // Write to FIFO_B
        fb = PISO(DEPT_B, fifo_b, ram->vec[index].coeffs);
        fa = FIFO(DEPT_D, fifo_d, -1);
        fc = FIFO(DEPT_C, fifo_c, -1);
        fd = FIFO(DEPT_A, fifo_a, -1);
        break;
    case 2:
        // Write to FIFO_C
        fc = PISO(DEPT_C, fifo_c, ram->vec[index].coeffs);
        fa = FIFO(DEPT_D, fifo_d, -1);
        fb = FIFO(DEPT_B, fifo_b, -1);
        fd = FIFO(DEPT_A, fifo_a, -1);
        break;

    default:
        // Write to FIFO_A
        fd = PISO(DEPT_A, fifo_a, ram->vec[index].coeffs);
        fa = FIFO(DEPT_D, fifo_d, -1);
        fb = FIFO(DEPT_B, fifo_b, -1);
        fc = FIFO(DEPT_C, fifo_c, -1);
        break;
    }

    *a = fa;
    *b = fc;
    *c = fb;
    *d = fd;
}
