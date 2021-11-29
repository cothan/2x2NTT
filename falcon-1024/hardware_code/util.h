#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdio.h>
#include "config.h"

template <typename T>
void print_array(T *a, int bound, const char *string)
{
    printf("%s :", string);
    for (int i = 0; i < bound; i++)
    {
        printf("%3u, ", a[i]);
    }
    printf("\n");
}

void print_reshaped_array(bram *ram, int bound, const char *string);

void print_index_reshaped_array(bram *ram, int index);

void reshape(bram *ram, const data_t in[FALCON_N]);

int compare_array(data_t *a, data_t *b, int bound);

int compare_bram_array(bram *ram, data_t array[FALCON_N],
                       const char *string,
                       enum MAPPING mapping, int print_out);

#endif
