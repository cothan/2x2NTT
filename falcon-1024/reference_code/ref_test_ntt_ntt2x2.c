#include <stdlib.h>
#include <stdio.h>
#include "ref_ntt.h"
#include "ref_ntt2x2.h"

#define TESTS 100000

int compare_array(uint16_t *a_gold, uint16_t *a)
{
    for (int i = 0; i < FALCON_N; i++)
    {
        if (a_gold[i] != a[i])
        {
            printf("%d: %u != %u\n", i, a_gold[i], a[i]);
            return 1;
        }
    }
    return 0;
}

int main()
{
    uint16_t a[FALCON_N] = {0}, a_gold[FALCON_N] = {0};
    uint16_t tmp;
    srand(0);

    printf("Test Forward NTT = %u :", TESTS);
    for (int j = 0; j < TESTS; j++)
    {
        // Test million times
        for (int i = 0; i < FALCON_N; i++)
        {
            tmp = rand() % 0xffff;
            a[i] = tmp;
            a_gold[i] = tmp;
        }

        ntt2x2_ref(a);
        // printf("=======\n");
        ntt(a_gold);

        if (compare_array(a_gold, a))
        {
            return 1;
        }
    }
    printf("OK\n");

    printf("Test Inverse NTT = %u :", TESTS);
    for (int j = 0; j < TESTS; j++)
    {
        // Test million times
        for (int i = 0; i < FALCON_N; i++)
        {
            tmp = rand() % 0xffff;
            a[i] = tmp;
            a_gold[i] = tmp;
        }

        invntt2x2_ref(a);
        // printf("=======\n");
        invntt(a_gold);

        if (compare_array(a_gold, a))
        {
            return 1;
        }
    }
    printf("OK\n");
    return 0;
}

// Compile flags
// gcc -o ntt2x2_ref consts.cpp ntt2x2_ref.c; ./ntt2x2_ref
