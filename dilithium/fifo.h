#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>
#include <cstdio>
#include "params.h"

// Don't change this
#define DEPT_I 3
#define DEPT_W 4
#define DEPT_A 4
#define DEPT_B 6
#define DEPT_C 5
#define DEPT_D 7

/* Serial in, serial out: This function receive 1 elements at the begin of FIFO. 
 */
template <int DEPT>
int32_t FIFO(int32_t fifo[DEPT], const int32_t new_value)
{
    int32_t out = fifo[DEPT - 1];
    for (auto i = DEPT - 1; i > 0; i--)
    {
        // printf("%d <= %d\n", i, i-1);
        fifo[i] = fifo[i - 1];
    }
    fifo[0] = new_value;

    return out;
}

/* Parallel in, serial out: This function receive 4 elements at the begin of FIFO. 
 */
template <int DEPT>
int32_t PISO(int32_t fifo[DEPT], const int32_t line[4])
{
    int32_t out = fifo[DEPT - 1];
    for (int i = DEPT - 1; i > 3; i--)
    {
        fifo[i] = fifo[i - 1];
    }
    fifo[3] = line[0];
    fifo[2] = line[1];
    fifo[1] = line[2];
    fifo[0] = line[3];

    return out;
}

/* Parallel in, parallel out: This function receive 4 elements at the begin of FIFO. 
 */
template <int DEPT>
void PIPO(int32_t out[4], int32_t fifo[DEPT][DEPT],
          const int32_t w1, const int32_t w2,
          const int32_t w3, const int32_t w4)
{
    out[0] = fifo[DEPT - 1][0];
    out[1] = fifo[DEPT - 1][1];
    out[2] = fifo[DEPT - 1][2];
    out[3] = fifo[DEPT - 1][3];

    for (auto i = DEPT - 1; i > 0; i--)
    {
        // printf("%d <= %d\n", i, i - 1);
        for (auto j = 0; j < DEPT; j++)
        {
#pragma HLS LOOP_FLATTEN
#pragma HLS UNROLL
            fifo[i][j] = fifo[i - 1][j];
        }
    }
    fifo[0][0] = w1;
    fifo[0][1] = w2;
    fifo[0][2] = w3;
    fifo[0][3] = w4;
}

int32_t FIFO_I(const int dept, int32_t *fifo,
               const int32_t new_value, enum OPERATION mode);

template <int DEPT>
int32_t FIFO_PISO(int32_t fifo[DEPT], const int piso_en,
                  const int32_t line[4])
{
    int32_t out = fifo[DEPT - 1];
    for (int i = DEPT - 1; i > 3; i--)
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
        fifo[0] = 0;
    }
    return out;
}

void read_fifo(int32_t *fa, int32_t *fb,
               int32_t *fc, int32_t *fd,
               const int count,
               const int32_t fifo_a[DEPT_A],
               const int32_t fifo_b[DEPT_B],
               const int32_t fifo_c[DEPT_C],
               const int32_t fifo_d[DEPT_D]);

void write_fifo(int32_t data_in[4], const int32_t data_fifo[4],
                int32_t fifo_a[DEPT_A], int32_t fifo_b[DEPT_B],
                int32_t fifo_c[DEPT_C], int32_t fifo_d[DEPT_D],
                const int count);

#endif