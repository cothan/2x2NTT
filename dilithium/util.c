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

