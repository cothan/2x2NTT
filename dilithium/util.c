#include <stdio.h>
#include "util.h"
#include "params.h"

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
void reshape(bram *ram, const int32_t in[DILITHIUM_N])
{
    for (int i = 0; i <  DILITHIUM_N/ 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            ram->vec[i].coeffs[j] = in[4 * i + j];
        }
    }
}


// Compare array 
int compare_array(int32_t *a, int32_t *b, int bound)
{
    for (int i = 0; i < bound; i++)
    {
        if (a[i] != b[i])
            return 1;
    }
    return 0;
}