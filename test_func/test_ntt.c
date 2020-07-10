#include "../ref/newhope_ntt.h"
#include "../hls/hls_ntt_mul.h"
#include "../ref/my_ntt.h"
#include "common/utils.c"


// Compile flags
// clang -o test_ntt  ../hls/hls_ntt_mul.c ../ref/newhope_precomp.c ../ref/newhope_ntt.c ../ref/newhope_reduce.c test_ntt.c -Wall -Wextra -Werror -g3 -O0

int main()
{
    poly r_gold,
        origin_poly;

    uint64_t hls_r[NEWHOPE_N / 4],
        hls_origin_ram[NEWHOPE_N / 4];

    uint16_t a, b, c, d;

    for (uint16_t i = 0; i < NEWHOPE_N; i += 4)
    {
        a = i + 0;
        b = i + 1;
        c = i + 2;
        d = i + 3;
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

    ntt_dit(r_gold.coeffs, gammas_bitrev_montgomery);
    full_reduce(&r_gold);
    // TODO: add omega forward ram1, ram2 
    // hls_poly_ntt_mul(hls_r, , , MUL, true);

    uint16_t res = compare_poly_ram(&r_gold, hls_r, "Forward NTT NTT_DIT vs HLS_NTT");

    return res;
}