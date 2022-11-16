#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "fft.h"
#include "params.h"

#define ITERATIONS 20000

double drand(double low, double high)
{
    return ((double)rand() * (high - low)) / (double)RAND_MAX + low;
}

void combine(fpr *out, fpr *in, unsigned logn)
{
    const unsigned n = 1 << logn;
    const unsigned hn = n >> 1;

    int i_out = 0;
    fpr re, im;
    for (int i = 0; i < hn; i++)
    {
        re = in[i];
        im = in[i + hn];
        out[i_out++] = re;
        out[i_out++] = im;
    }
}

void split(fpr *out, fpr *in, unsigned logn)
{
    const unsigned n = 1 << logn;
    const unsigned hn = n >> 1;

    int i_out = 0;
    fpr re, im;
    for (int i = 0; i < n; i += 2)
    {
        re = in[i];
        im = in[i + 1];
        out[i_out] = re;
        out[i_out + hn] = im;
        i_out += 1;
    }
}

void print_double(fpr *f, unsigned logn, const char *string)
{
    const unsigned n = 1 << logn;

    printf("%s:\n", string);
    for (int i = 0; i < n; i += 2)
    {
        printf("%.1f, %.1f, ", f[i], f[i + 1]);
    }
    printf("\n");
}

fpr fabs(fpr a)
{
    if (a < 0)
    {
        return -a;
    }
    else
    {
        return a;
    }
}

int cmp_double(fpr *f, fpr *g, unsigned logn)
{
    const unsigned n = 1 << logn;

    for (int i = 0; i < n; i++)
    {
        if (fabs(f[i] - g[i]) > 0.000001)
        {
            printf("[%d]: %.2f != %.2f \n", i, f[i], g[i]);
            printf("ERROR\n");
            return 1;
        }
    }
    return 0;
}

int test_with_adj_FFT(unsigned logn, unsigned tests)
{
    fpr f[1024], g[1024], tmp[1024];
    const unsigned n = 1 << logn;

    for (int j = 0; j < tests; j++)
    {
        for (int i = 0; i < n; i++)
        {
            f[i] = drand(-12289.0, 12289);
        }
        combine(g, f, logn);

        FFT(f, logn);
        fwd_FFT_adj(g, logn);

        split(tmp, g, logn);
        if (cmp_double(f, tmp, logn))
        {
            return 1;
        }

        iFFT(f, logn);
        inv_FFT_adj(g, logn);

        split(tmp, g, logn);
        if (cmp_double(f, tmp, logn))
        {
            return 1;
        }
    }

    return 0;
}

int test_with_adj_short_FFT(unsigned logn, unsigned tests)
{
    fpr f[1024] = {0}, g[1024] = {0};
    const unsigned n = 1 << logn;

    for (int j = 0; j < tests; j++)
    {
        for (int i = 0; i < n; i++)
        {
            f[i] = drand(-12289.0, 12289);
            g[i] = f[i];
        }
        
        fwd_FFT_adj_short(f, logn);
        fwd_FFT_adj(g, logn);

        if (cmp_double(f, g, 10))
        {
            printf("%d: Forward FFT is wrong\n", logn);
            return 1;
        }

        inv_FFT_adj_short(f, logn);
        inv_FFT_adj(g, logn);

        if (cmp_double(f, g, 10))
        {
            printf("%d: Inverse FFT is wrong\n", logn);
            return 1;
        }
    }

    return 0;
}

int test_variant_fft(unsigned logn, unsigned tests)
{
    fpr f[1024], g[1024];
    const unsigned n = 1 << logn;

    for (int j = 0; j < tests; j++)
    {
        for (int i = 0; i < n; i++)
        {
            f[i] = drand(-12289.0, 12289);
            g[i] = f[i];
        }

        FFT(f, logn);
        fwd_FFT_short(g, logn);

        if (cmp_double(f, g, logn))
        {
            return 1;
        }

        iFFT(f, logn);
        inv_FFT_short(g, logn);

        if (cmp_double(f, g, logn))
        {
            return 1;
        }
    }

    return 0;
}

int main(void)
{

    // printf("\ntest_with_adj_FFT: ");
    // printf("\nCompare split FFT versus adjacent FFT setting\n");
    // for (int logn = 2; logn < 11; logn++)
    // {
    //     if (test_with_adj_FFT(logn, ITERATIONS))
    //     {
    //         printf("Error at LOGN = %d\n", logn);
    //         return 1;
    //     }
    // }
    // printf("OK\n");

    // printf("\ntest_variant_fft: ");
    // printf("\nCompare my (loop separated) FFT versus my (without loop separateed) FFT code\n");
    // for (int logn = 2; logn < 11; logn++)
    // {
    //     if (test_variant_fft(logn, ITERATIONS))
    //     {
    //         printf("Error at LOGN = %d\n", logn);
    //         return 1;
    //     }
    // }
    // printf("OK\n");

    printf("\ntest_with_adj_short_FFT: ");
    printf("\nCompare adjacent FFT setting versus short adjacent FFT\n");
    for (int logn = 2; logn < 11; logn++)
    {
        if (test_with_adj_short_FFT(logn, ITERATIONS))
        // if (test_with_adj_short_FFT(logn, 1))
        {
            printf("Error at LOGN = %d\n", logn);
            printf("=====================\n");
            return 1;
        }
        printf("OK %d\n", logn);

    }
    printf("=====================\n");

    return 0;
}

// Compile flags:
// make test_fft_m1; sudo ./test_fft_m1
// make test_fft_a72; sudo ./test_fft_a72