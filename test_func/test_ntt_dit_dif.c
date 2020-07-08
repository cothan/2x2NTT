#include "../ref/my_ntt.h"
#include "../ref/newhope_precomp.c"
#include <stdlib.h>
#include <stdbool.h>

// Compile Flag:
// clang -o test_ntt_dit_dif ../ref/newhope_ntt.c ../ref/my_ntt.c ../ref/newhope_reduce.c ../ref/newhope_poly.c test_ntt_dit_dif.c -Wall -Wextra -Werror -g3 -O0


uint16_t test1(poly *r_gold, poly *r_test_dif, poly *origin_poly)
{
    mul_coefficients(r_gold->coeffs, gammas_bitrev_montgomery);
    ntt_copy(r_gold->coeffs, gammas_bitrev_montgomery);
    // NTT DIF: BO-> NO
    mul_coefficients(r_test_dif->coeffs, gammas_bitrev_montgomery);
    ntt_dif(r_test_dif->coeffs, gammas_bitrev_montgomery);

    full_reduce(r_gold);
    full_reduce(r_test_dif);

    uint16_t res = compare(r_gold, r_test_dif, "NEWHOPE DIF vs MY DIF");

    // Revert back to original value
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        r_test_dif->coeffs[i] = origin_poly->coeffs[i];
    }

    return res;
}

uint16_t test2(poly *r_test_dif, poly *origin_poly)
{
    scramble(r_test_dif);
    poly_ntt(r_test_dif);

    poly_invntt(r_test_dif);
    full_reduce(r_test_dif);
    uint16_t res = compare(r_test_dif, origin_poly, "poly_ntt test");
    // YES, only after full_reduce

    // Revert back to original value
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        r_test_dif->coeffs[i] = origin_poly->coeffs[i];
    }
    return res;
}

uint16_t test3(poly *r_test_dif, poly *origin_poly)
{
        // NTT DIF: BO-> NO
    // Forward NTT, input is stored in BO
    scramble(r_test_dif);
    mul_coefficients(r_test_dif->coeffs, gammas_bitrev_montgomery);
    ntt_dif(r_test_dif->coeffs, gammas_bitrev_montgomery);

    // Inverse NTT, input is stored in NO, output is in NO
    // Now the output is stored at NO, convert to BO
    scramble(r_test_dif);
    ntt_dif(r_test_dif->coeffs, omegas_inv_bitrev_montgomery);
    mul_coefficients(r_test_dif->coeffs, gammas_inv_montgomery);

    full_reduce(r_test_dif);

    uint16_t res = compare(r_test_dif, origin_poly, "ntt_dif + mul_coefficients test");
    // YES

    // Revert back to original value
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        r_test_dif->coeffs[i] = origin_poly->coeffs[i];
    }
    return res;
}

uint16_t test4(poly *r_test_dif, poly *origin_poly)
{

    scramble(r_test_dif);
    mul_coefficients_full_reduce(r_test_dif->coeffs, my_gammas_bitrev);
    ntt_dif_full_reduction(r_test_dif->coeffs, my_gammas_bitrev);

    scramble(r_test_dif);
    ntt_dif_full_reduction(r_test_dif->coeffs, my_omegas_inv_bitrev);
    mul_coefficients_full_reduce(r_test_dif->coeffs, my_gammas_inv);

    uint16_t res = compare(r_test_dif, origin_poly, "ntt_dif_full_reduction + mul_coefficients_full_reduce test");
    // YES

    // Revert back to original value
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        r_test_dif->coeffs[i] = origin_poly->coeffs[i];
    }

    return res;
}

uint16_t test5(poly *r_test_dit, poly *origin_poly)
{
    scramble(r_test_dit);
    mul_coefficients_full_reduce(r_test_dit->coeffs, my_gammas_bitrev);
    ntt_dit_full_reduction(r_test_dit->coeffs, my_omegas);

    scramble(r_test_dit);
    ntt_dit_full_reduction(r_test_dit->coeffs, my_omegas_inv);
    mul_coefficients_full_reduce(r_test_dit->coeffs, my_gammas_inv);

    // printArray(r_test_dif.coeffs, NEWHOPE_N, "ntt_dit_full_reduction test");

    uint16_t res = compare(r_test_dit, origin_poly, "ntt_dit_full_reduction + mul_coefficients_full_reduce test");

    // Revert back to original value
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        r_test_dit->coeffs[i] = origin_poly->coeffs[i];
    }

    return res;

}

int main()
{
    poly r_gold,
        r_test_dif,
        r_test_dit,
        origin_poly;
    uint16_t a, b, c, d;
    srand((unsigned int)&r_gold);

    for (uint16_t i = 0; i < NEWHOPE_N; i += 4)
    {
        a = rand() % NEWHOPE_Q;
        b = rand() % NEWHOPE_Q;
        c = rand() % NEWHOPE_Q;
        d = rand() % NEWHOPE_Q;

        r_gold.coeffs[i] = a;
        r_gold.coeffs[i + 1] = b;
        r_gold.coeffs[i + 2] = c;
        r_gold.coeffs[i + 3] = d;

        r_test_dif.coeffs[i] = a;
        r_test_dif.coeffs[i + 1] = b;
        r_test_dif.coeffs[i + 2] = c;
        r_test_dif.coeffs[i + 3] = d;

        r_test_dit.coeffs[i] = a;
        r_test_dit.coeffs[i + 1] = b;
        r_test_dit.coeffs[i + 2] = c;
        r_test_dit.coeffs[i + 3] = d;

        origin_poly.coeffs[i] = a;
        origin_poly.coeffs[i + 1] = b;
        origin_poly.coeffs[i + 2] = c;
        origin_poly.coeffs[i + 3] = d;
    }

    printf("Testing N = %d\n", NEWHOPE_N);

    uint16_t res = test1(&r_gold, &r_test_dif, &origin_poly);

    /*********************poly_ntt test */
    
    res = test2(&r_test_dif, &origin_poly);


    /********* ntt_dif + mul_coefficient test ************/

    res = test3(&r_test_dif, &origin_poly);

    printf("======= Full reduction test ==============\n");

    res = test4(&r_test_dif, &origin_poly);

    // Important 
    res = test5(&r_test_dit, &origin_poly);
    
    /************************************ Below are trash ************************/
    return res;
}

