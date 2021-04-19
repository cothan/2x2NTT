#include <stdio.h>
#include "util.h"
#include "params.h"
#include "ram_util.h"
#include "address_encoder_decoder.h"

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

void print_index_reshaped_array(bram *ram, int index)
{
    int32_t a, b, c, d;
    read_ram(&a, &b, &c, &d, ram, index);
    printf("[%d]: [%d, %d, %d, %d]\n", index, a, b, c, d);
    printf("--------------\n");
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

int compare_bram_array(bram *ram, int32_t array[DILITHIUM_N], const char *string, enum MAPPING decode, int print_out)
{
    int32_t a, b, c, d;
    int32_t ta, tb, tc, td;
    int ret = 0;
    int addr;

    for (int i = 0; i < DILITHIUM_N; i += 4)
    {
        // Get golden result
        a = (array[i + 0] + DILITHIUM_Q) % DILITHIUM_Q;
        b = (array[i + 1] + DILITHIUM_Q) % DILITHIUM_Q;
        c = (array[i + 2] + DILITHIUM_Q) % DILITHIUM_Q;
        d = (array[i + 3] + DILITHIUM_Q) % DILITHIUM_Q;

        addr = i / 4;
        if (print_out) printf("%d: %d, %d %d, %d\n", addr, a, b, c, d);
        if (decode == DECODE_TRUE)
        {
            addr = addr_decoder(addr);
        }
        if (print_out) print_index_reshaped_array(ram, addr);
        read_ram(&ta, &tb, &tc, &td, ram, addr);

        ta = ta % DILITHIUM_Q;
        tb = tb % DILITHIUM_Q;
        tc = tc % DILITHIUM_Q;
        td = td % DILITHIUM_Q;

        // Quick xor, I hate long if-else clause

        if ( (ta != a) || (tb != b) || (tc != c) || (td != d) )
        {
            printf("%s Error at index: %d => %d\n", string, addr, i);
            printf("%12d | %12d | %12d | %12d\n", a, b, c, d);
            printf("%12d | %12d | %12d | %12d\n", ta, tb, tc, td);
            ret = 1;
            // return 1;
        }
    }
    if (ret)
    {
        return 1;
    }
    return 0;
}