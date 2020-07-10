#include "../ref/newhope_ntt.h"
#include "../hls/hls_ntt_mul.h"
#include "../ref/my_ntt.h"
#include "common/utils.c"
// Compile flags
// clang -o test_ntt_mul ../hls/hls_const.c ../hls/hls_ntt_mul.c ../ref/newhope_precomp.c ../ref/newhope_ntt.c ../ref/newhope_reduce.c ../ref/my_ntt.c test_ntt_mul.c -Wall -Wextra -Werror -g3 -O0

// Status: PASSED

int main()
{
    poly r_gold,
        origin_poly;

    uint64_t hls_r[NEWHOPE_N / 4],
        hls_origin_ram[NEWHOPE_N / 4];

    uint16_t a, b, c, d, res;

    for (uint16_t i = 0; i < NEWHOPE_N; i += 4)
    {
        a = rand();
        b = rand();
        c = rand();
        d = rand();
        r_gold.coeffs[i] = a;
        r_gold.coeffs[i + 1] = b;
        r_gold.coeffs[i + 2] = c;
        r_gold.coeffs[i + 3] = d;

        origin_poly.coeffs[i] = a;
        origin_poly.coeffs[i + 1] = b;
        origin_poly.coeffs[i + 2] = c;
        origin_poly.coeffs[i + 3] = d;


        pack(a, b, c, d, hls_r, i / 4, false);
        pack(a, b, c, d, hls_origin_ram, i / 4, false);
    }

    for (uint16_t test=0; test< 100; test++)
    {
        mul_coefficients(r_gold.coeffs, gammas_bitrev_montgomery);
        full_reduce(&r_gold);
        hls_poly_ntt_mul(hls_r, hls_ram1_gammas_bitrev_montgomery, hls_ram2_gammas_bitrev_montgomery, PSIS, false);

        res = compare_poly_ram(&r_gold, hls_r, "Test HLS PSIS MUL");

        // Revert back to original
        copy_poly(&r_gold, &origin_poly);
        copy_ram(hls_r, hls_origin_ram);
        // printArray(r_gold.coeffs, NEWHOPE_N, "r_gold");


        mul_coefficients(r_gold.coeffs, gammas_inv_montgomery);
        full_reduce(&r_gold);
        hls_poly_ntt_mul(hls_r, hls_ram1_gammas_inv_montgomery, hls_ram2_gammas_inv_montgomery, IPSIS, false);

        res = compare_poly_ram(&r_gold, hls_r, "Test HLS IPSIS MUL");
        // Revert back to original
        copy_poly(&r_gold, &origin_poly);
        copy_ram(hls_r, hls_origin_ram);
    }

    return res;
}