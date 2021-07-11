#include <stdio.h>
#include "util.h"
#include "params.h"
#include "ram_util.h"
#include "address_encoder_decoder.h"

void print_array(int32_t *a, int bound, const char *string)
{
    printf("%s :", string);
    for (int i = 0; i < bound; i++)
    {
        printf("%3d, ", a[i]);
    }
    printf("\n");
}

void print_reshaped_array(bram *ram, int bound, const char *string)
{
    printf("%s :\n", string);
    int32_t a, b, c, d;
    for (int i = 0; i < bound; i++)
    {
        read_ram(&a, &b, &c, &d, ram, i);
        printf("%d, %d, %d, %d, ", a, b, c, d);
    }
    printf("\n");
}

void print_index_reshaped_array(bram *ram, int index)
{
    int32_t a, b, c, d;
    read_ram(&a, &b, &c, &d, ram, index);
    printf("[%d]: [%d, %d, %d, %d]\n", index, a, b, c, d);
    
}

// Store 4 coefficients per line
void reshape(bram *ram, const int32_t in[DILITHIUM_N])
{
    for (int i = 0; i <  DILITHIUM_N/ 4; i++)
    {
        write_ram(ram, i, in[4*i], in[4*i +1], in[4*i + 2], in[4*i + 3]);
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

int compare_bram_array(bram *ram, int32_t array[DILITHIUM_N], const char *string, enum MAPPING mapping, int print_out)
{
    int32_t a, b, c, d;
    int32_t ta, tb, tc, td;
    int error = 0;
    int addr;

    for (int i = 0; i < DILITHIUM_N; i += 4)
    {
        // Get golden result
        a = (array[i + 0] + DILITHIUM_Q) % DILITHIUM_Q;
        b = (array[i + 1] + DILITHIUM_Q) % DILITHIUM_Q;
        c = (array[i + 2] + DILITHIUM_Q) % DILITHIUM_Q;
        d = (array[i + 3] + DILITHIUM_Q) % DILITHIUM_Q;

        addr = i / 4;
        if (print_out) printf("%d: %d, %d, %d, %d\n", addr, a, b, c, d);
        addr = resolve_address(mapping, addr);
        
        read_ram(&ta, &tb, &tc, &td, ram, addr);

        ta = (ta + DILITHIUM_Q) % DILITHIUM_Q;
        tb = (tb + DILITHIUM_Q) % DILITHIUM_Q;
        tc = (tc + DILITHIUM_Q) % DILITHIUM_Q;
        td = (td + DILITHIUM_Q) % DILITHIUM_Q;
        if (print_out) printf("[%d]: |%d, %d, %d, %d|\n", i, ta, tb, tc, td);
        // if (print_out) print_index_reshaped_array(ram, addr);
        
        // Quick xor, I hate long if-else clause
        if (print_out) printf("--------------\n");
        
        if ( (ta != a) || (tb != b) || (tc != c) || (td != d) )
        {
            printf("%s Error at index: %d => %d\n", string, i, addr);
            printf("%12d | %12d | %12d | %12d [*]\n", a, b, c, d);
            printf("%12d | %12d | %12d | %12d\n", ta, tb, tc, td);
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