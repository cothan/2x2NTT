#include <stdio.h>
#include "config.h"
#include "ram_util.h"
#include "address_encoder_decoder.h"

void print_reshaped_array(bram *ram, int bound, const char *string)
{
    data_t coeffs[4];

    printf("%s :\n", string);
    for (int i = 0; i < bound; i++)
    {
        read_ram(coeffs, ram, i);

        for (int j = 0; j < 4; j++)
        {
            printf("%u, ", coeffs[j]);
        }
    }
    printf("\n");
}

void print_index_reshaped_array(bram *ram, int index)
{
    data_t coeffs[4];

    read_ram(coeffs, ram, index);

    printf("[%d]: ", index);
    for (int j = 0; j < 4; j++)
    {
        printf("%u, ", coeffs[j]);
    }
    printf("\n");
}

// Store 4 coefficients per line
void reshape(bram *ram, const data_t in[FALCON_N])
{
    data_t coeffs[4];
    for (int i = 0; i < BRAM_DEPT; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            coeffs[j] = in[4 * i + j];
        }
        write_ram(ram, i, coeffs);
    }
}

// Compare array
int compare_array(data_t *a, data_t *b, int bound)
{
    for (int i = 0; i < bound; i++)
    {
        if (a[i] != b[i])
            return 1;
    }
    return 0;
}

int compare_bram_array(bram *ram, data_t array[FALCON_N],
                       const char *string,
                       enum MAPPING mapping, int print_out)
{
    data_t a, b, c, d;
    data_t ta, tb, tc, td, t[4];
    int error = 0;
    int addr;

    for (int i = 0; i < FALCON_N; i += 4)
    {
        // Get golden result
        a = (array[i + 0] + FALCON_Q) % FALCON_Q;
        b = (array[i + 1] + FALCON_Q) % FALCON_Q;
        c = (array[i + 2] + FALCON_Q) % FALCON_Q;
        d = (array[i + 3] + FALCON_Q) % FALCON_Q;

        addr = i / 4;
        if (print_out)
        {
            printf("%d: %d, %d, %d, %d\n", addr, a, b, c, d);
        }
        addr = resolve_address(mapping, addr);

        read_ram(t, ram, addr);

        ta = t[0] = (t[0] + FALCON_Q) % FALCON_Q;
        tb = t[1] = (t[1] + FALCON_Q) % FALCON_Q;
        tc = t[2] = (t[2] + FALCON_Q) % FALCON_Q;
        td = t[3] = (t[3] + FALCON_Q) % FALCON_Q;
        if (print_out)
        {
            printf("[%d]: |%d, %d, %d, %d|\n", i, ta, tb, tc, td);
        }

        // Quick xor, I hate long if-else clause
        if (print_out)
        {
            printf("--------------\n");
        }

        if ((ta != a) || (tb != b) || (tc != c) || (td != d))
        {
            printf("%s Error at index: %d => %d\n", string, i, addr);
            printf("gold: %12u | %12u | %12u | %12u [*]\n", a, b, c, d);
            printf("test: %12u | %12u | %12u | %12u\n", ta, tb, tc, td);
            error = 1;
            break;
        }
    }
    if (error)
    {
        return 1;
    }
    return 0;
}
