#include <stdio.h>
#include "util.h"
#include "params.h"
#include "ram_util.h"

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

int compare_bram_array(bram *ram, int32_t array[DILITHIUM_N], const char *string)
{
    int32_t a, b, c, d;
    int32_t ta, tb, tc, td;
    int ret = 1;
    int addr;

    for (int i = 0; i < DILITHIUM_N; i += 4)
    {
        // Get golden result
        a = array[i + 0];
        b = array[i + 1];
        c = array[i + 2];
        d = array[i + 3];

        addr = i / 4;
        read_ram(&ta, &tb, &tc, &td, ram, addr);

        // Quick xor, I hate long if-else clause
        ret &= a != ta;
        ret &= b != tb;
        ret &= c != tc;
        ret &= d != td;

        if (ret)
        {
            printf("%s Error at index: %d => %d\n", string, addr, i);
            printf("%12d | %12d | %12d | %12d\n", a, b, c, d);
            printf("%12d | %12d | %12d | %12d\n", ta, tb, tc, td);
            return 1;
        }
    }
    return 0;
}