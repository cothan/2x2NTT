#ifndef FIFO_H
#define FIFO_H

#include "mytypes.h"

#define DEPT_I 3
#define DEPT_A 4
#define DEPT_B 6
#define DEPT_C 5
#define DEPT_D 7


void write_fifo(u24 fifo_a[DEPT_A], u24 fifo_b[DEPT_B], 
                u24 fifo_c[DEPT_C], u24 fifo_d[DEPT_D],
                const int count, const u96 ram, const int index);

void read_fifo(u24 *fa, u24 *fb, u24 *fc, u24 *fd,
               const int count, const int mode,
               const u24 fifo_a[DEPT_A],
               const u24 fifo_b[DEPT_B],
               const u24 fifo_c[DEPT_C],
               const u24 fifo_d[DEPT_D]);

template <int DEPT>
u24 FIFO(u24 fifo[DEPT], const u24 value);

#endif 