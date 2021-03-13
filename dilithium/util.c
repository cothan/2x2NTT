#include <stdio.h>
#include "util.h"

void print_array(int32_t *a, int bound, const char *string)
{
    printf("%s :\n", string);
    for (int i = 0; i < bound; i++)
    {
        printf("%d, ", a[i]);
    }
    printf("\n");
}

void print_reshaped_array(bram *ram, int bound, const char *string)
{
    printf("%s :\n", string);
    for (int i = 0; i < bound; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%d, ", ram->vec[i].coeffs[j]);
        }
    }
    printf("\n");
}

// Store 4 coefficients per line
void reshape(bram *ram, int32_t in[N])
{
    for (int i = 0; i < N / 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            ram->vec[i].coeffs[j] = in[4 * i + j];
        }
    }
}
