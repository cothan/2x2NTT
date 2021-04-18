#include "fifo.h"

void write_fifo(u24 fifo_a[DEPT_A], u24 fifo_b[DEPT_B],
                u24 fifo_c[DEPT_C], u24 fifo_d[DEPT_D],
                const int count,
                const u96 ram,
                const int index)
{
    u24 a, b, c, d;
    (d, c, b, a) = ram[index];
    // d = ram->vec[index].coeffs[0];
    // c = ram->vec[index].coeffs[1];
    // b = ram->vec[index].coeffs[2];
    // a = ram->vec[index].coeffs[3];
    switch (count & 0b11)
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

void read_fifo(u24 *fa, u24 *fb,
               u24 *fc, u24 *fd,
               const int count, const int mode,
               const u24 fifo_a[DEPT_A],
               const u24 fifo_b[DEPT_B],
               const u24 fifo_c[DEPT_C],
               const u24 fifo_d[DEPT_D])
{
    u24 ta, tb, tc, td;
    if (mode == INVNTT_MODE)
    {
        // Serial in Parallel out
        switch (count & 0b11)
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

template <int DEPT>
u24 FIFO(u24 fifo[DEPT], const u24 value)
{
    u24 out = fifo[DEPT - 1];
    for (int i = DEPT - 1; i > -1; i--)
    {
#pragma HLS UNROLL
        fifo[i] = fifo[i - 1];
    }
    fifo[0] = value;

    return out;
}
