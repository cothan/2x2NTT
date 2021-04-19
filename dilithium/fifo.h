#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>

// Don't change this 
#define DEPT_I 3
#define DEPT_A 4
#define DEPT_B 6
#define DEPT_C 5
#define DEPT_D 7


void read_fifo(int32_t *fa, int32_t *fb,
               int32_t *fc, int32_t *fd,
               const int count, const int mode,
               const int32_t fifo_a[DEPT_A],
               const int32_t fifo_b[DEPT_B],
               const int32_t fifo_c[DEPT_C],
               const int32_t fifo_d[DEPT_D]);

int32_t FIFO(const int dept, int32_t *fifo, 
            const int32_t new_value);

#endif