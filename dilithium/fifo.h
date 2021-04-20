#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>
#include "ntt2x2.h"

// Don't change this
#define DEPT_W 4
#define DEPT_I 3
#define DEPT_A 4
#define DEPT_B 6
#define DEPT_C 5
#define DEPT_D 7

int32_t FIFO(const int dept, int32_t *fifo,
             const int32_t new_value);

void read_fifo(int32_t *fa, int32_t *fb,
               int32_t *fc, int32_t *fd,
               const int count, enum OPERATION mode,
               const int32_t fifo_a[DEPT_A],
               const int32_t fifo_b[DEPT_B],
               const int32_t fifo_c[DEPT_C],
               const int32_t fifo_d[DEPT_D]);

void PIPO(int32_t out[4], const int dept,
          int32_t *fifo4, const int32_t w[4]);

void write_fifo(int32_t *a, int32_t *b, int32_t *c, int32_t *d,
                int32_t fifo_a[DEPT_A], int32_t fifo_b[DEPT_B],
                int32_t fifo_c[DEPT_C], int32_t fifo_d[DEPT_D],
                const int count,
                const bram *ram, const int index);

#endif