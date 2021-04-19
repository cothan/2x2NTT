#include <stdint.h>
#include <stdio.h>
#include "fifo.h"
#include "params.h"
#include "ntt2x2.h"

// This function will output an element when insert 1 element
int32_t FIFO(const int dept, int32_t *fifo, const int32_t new_value)
{
    int32_t out = fifo[dept - 1];
    for (int i = dept - 1; i > -1; i--)
    {
        fifo[i] = fifo[i - 1];
    }
    fifo[0] = new_value;

    return out;
}

void read_fifo(int32_t *fa, int32_t *fb,
               int32_t *fc, int32_t *fd,
               const int count, const int mode,
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

void write_fifo(int32_t fifo_a[DEPT_A], int32_t fifo_b[DEPT_B],
                int32_t fifo_c[DEPT_C], int32_t fifo_d[DEPT_D],
                const int count,
                const bram *ram, const int index)
{
    int32_t a, b, c, d;
    d = ram->vec[index].coeffs[0];
    c = ram->vec[index].coeffs[1];
    b = ram->vec[index].coeffs[2];
    a = ram->vec[index].coeffs[3];
    switch (count & 3)
    {
    case 0:
        // FIFO_D
        fifo_d[3] = a;
        fifo_d[2] = b;
        fifo_d[1] = c;
        fifo_d[0] = d;
        break;

    case 1:
        // FIFO_C
        fifo_c[3] = a;
        fifo_c[2] = b;
        fifo_c[1] = c;
        fifo_c[0] = d;
        break;

    case 2:
        // FIFO_B
        fifo_b[3] = a;
        fifo_b[2] = b;
        fifo_b[1] = c;
        fifo_b[0] = d;
        break;

    default:
        // FIFO_A
        fifo_a[3] = a;
        fifo_a[2] = b;
        fifo_a[1] = c;
        fifo_a[0] = d;
        break;
    }
}