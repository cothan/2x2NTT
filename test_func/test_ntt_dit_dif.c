#include "../ref/my_ntt.h"
#include "../ref/newhope_precomp.c"
#include <stdlib.h>
#include <stdbool.h>

// Compile Flag:
// clang -o test_ntt_dit_dif ../ref/newhope_ntt.c ../ref/my_ntt.c ../ref/newhope_reduce.c ../ref/newhope_poly.c test_ntt_dit_dif.c -Wall -Wextra -Werror -g3 -O0

/*
Ssanity check 
Test to see if my implementation of DIF give out the same output as reference implementation
Status: PASS
*/
uint16_t test1(poly *r_gold, poly *r_test_dif, poly *origin_poly)
{
    printf("TEST1\n");
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

/*
Sanity check
Test to see if reference implementation of Forward DIF and Inverse DIF back to original value
Status: PASS
*/
uint16_t test2(poly *r_test_dif, poly *origin_poly)
{
    printf("TEST2\n");
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

/*
Montgomery test
Test to see if my implementation of Forward DIF and Inverse DIF back to original value
Status: PASS
*/
uint16_t test3(poly *r_test_dif, poly *origin_poly)
{
    printf("TEST3\n");
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

    scramble(r_test_dif);
    my_poly_ntt_dif(r_test_dif);
    my_poly_invntt_dif(r_test_dif);
    full_reduce(r_test_dif);
    res = compare(r_test_dif, origin_poly, "my_poly_ntt_dif");

    copy_poly(r_test_dif, origin_poly);

    return res;
}

/*
Full reduction test
Test to see if Forward DIF and Inverse DIF back to original value
Status: PASS
*/
uint16_t test4(poly *r_test_dif, poly *origin_poly)
{
    printf("TEST4\n");
    scramble(r_test_dif);
    mul_coefficients_full_reduce(r_test_dif->coeffs, my_gammas_bitrev);
    ntt_dif_full_reduction(r_test_dif->coeffs, my_gammas_bitrev);

    scramble(r_test_dif);
    ntt_dif_full_reduction(r_test_dif->coeffs, my_omegas_inv_bitrev);
    mul_coefficients_full_reduce(r_test_dif->coeffs, my_gammas_inv);

    // printArray(r_test_dif->coeffs, NEWHOPE_N, "ntt_DIF_full_reduction test");
    uint16_t res = compare(r_test_dif, origin_poly, "ntt_dif_full_reduction + mul_coefficients_full_reduce test");
    // YES

    // Revert back to original value
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        r_test_dif->coeffs[i] = origin_poly->coeffs[i];
    }

    scramble(r_test_dif);
    my_poly_ntt_dif_full_reduction(r_test_dif);
    my_poly_invntt_dif_full_reduction(r_test_dif);
    res = compare(r_test_dif, origin_poly, "my_poly_ntt_dif_full reduction");

    copy_poly(r_test_dif, origin_poly);

    return res;
}

/*
Full reduction test
Test to see if Forward DIT and Inverse DIT back to original value
Status: PASS
*/
uint16_t test5(poly *r_test_dit, poly *origin_poly)
{
    printf("TEST5\n");
    scramble(r_test_dit);
    mul_coefficients_full_reduce(r_test_dit->coeffs, my_gammas_bitrev);
    ntt_dit_full_reduction(r_test_dit->coeffs, my_omegas);

    scramble(r_test_dit);
    ntt_dit_full_reduction(r_test_dit->coeffs, my_omegas_inv);
    mul_coefficients_full_reduce(r_test_dit->coeffs, my_gammas_inv);

    // printArray(r_test_dit->coeffs, NEWHOPE_N, "ntt_DIT_full_reduction test");
    uint16_t res = compare(r_test_dit, origin_poly, "ntt_dit_full_reduction + mul_coefficients_full_reduce test");

    // Revert back to original value
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        r_test_dit->coeffs[i] = origin_poly->coeffs[i];
    }

    scramble(r_test_dit);
    my_poly_ntt_dit_full_reduction(r_test_dit);
    my_poly_invntt_dit_full_reduction(r_test_dit);

    res = compare(r_test_dit, origin_poly, "my_poly_ntt_dit_full_reduction");

    copy_poly(r_test_dit, origin_poly);

    return res;
}

/*
Test to see if multiplication in my DIF and reference DIF are correct
Status: PASS
*/
uint16_t test6(poly *r_test_dif, poly *tmp, poly *origin_poly)
{
    printf("TEST6\n");
    poly dif_copy;

    // Phase 1
    scramble(r_test_dif);
    scramble(tmp);

    poly_ntt(r_test_dif);
    poly_ntt(tmp);

    mul_coefficients(r_test_dif->coeffs, tmp->coeffs);
    poly_invntt(r_test_dif);
    full_reduce(r_test_dif);

    copy_poly(&dif_copy, r_test_dif);
    
    // printArray(dif_copy.coeffs, NEWHOPE_N, "poly_ntt");

    // Phase 2
    // Revert to original value
    copy_poly(r_test_dif, origin_poly);
    copy_poly(tmp, origin_poly);
    // printArray(tmp->coeffs, NEWHOPE_N, "tmp after copy");

    scramble(r_test_dif);
    scramble(tmp);

    my_poly_ntt_dif(r_test_dif);
    my_poly_ntt_dif(tmp);
    
    mul_coefficients(r_test_dif->coeffs, tmp->coeffs);

    my_poly_invntt_dif(r_test_dif);
    full_reduce(r_test_dif);
    
    // printArray(r_test_dif->coeffs, NEWHOPE_N, "my_poly_ntt");
    uint16_t res = compare(r_test_dif, &dif_copy, "my_poly_ntt+mul+my_invntt vs ref poly_ntt+mul+invntt");

    // Revert back to original value
    copy_poly(r_test_dif, origin_poly);
    copy_poly(tmp, origin_poly);

    return res;
}

/*
Full reduction test
Test to see if multiplication in my DIF and full reduce DIF are correct
Status: Incorrect
*/
uint16_t test7(poly *r_test_dif, poly *tmp, poly *origin_poly)
{
    printf("TEST7\n");
    poly dif_copy;

    // Phase 1
    scramble(r_test_dif);
    scramble(tmp);

    my_poly_ntt_dif(r_test_dif);
    my_poly_ntt_dif(tmp);

    mul_coefficients(r_test_dif->coeffs, tmp->coeffs);
    my_poly_invntt_dif(r_test_dif);
    full_reduce(r_test_dif);

    copy_poly(&dif_copy, r_test_dif);
    
    // printArray(dif_copy.coeffs, NEWHOPE_N, "my_poly_ntt_dif");

    // Phase 2
    // Revert to original value
    copy_poly(r_test_dif, origin_poly);
    copy_poly(tmp, origin_poly);
    // printArray(tmp->coeffs, NEWHOPE_N, "tmp after copy");

    scramble(r_test_dif);
    scramble(tmp);

    my_poly_ntt_dif_full_reduction(r_test_dif);
    my_poly_ntt_dif_full_reduction(tmp);
    
    mul_coefficients_full_reduce(r_test_dif->coeffs, tmp->coeffs);

    my_poly_invntt_dif_full_reduction(r_test_dif);
    
    // printArray(r_test_dif->coeffs, NEWHOPE_N, "my_poly_ntt_dif_full_reduction");

    uint16_t res = compare(r_test_dif, &dif_copy, "my_poly* full reduction vs montgomery");

    // Revert back to original value
    copy_poly(r_test_dif, origin_poly);
    copy_poly(tmp, origin_poly);

    return res;
}


/*
Full reduction test 
Test to see if multiplication in DIT and DIF are correct
Status: Incorrect
*/
uint16_t test8(poly *r_test_dit, poly *r_test_dif, poly *tmp, poly *origin_poly)
{
    printf("TEST8\n");
    poly tmp_copy;

    scramble(r_test_dit);
    scramble(r_test_dif);
    scramble(tmp);

    copy_poly(&tmp_copy, tmp);

    // DIT MUL
    my_poly_ntt_dit_full_reduction(r_test_dit);
    my_poly_ntt_dit_full_reduction(&tmp_copy);
    mul_coefficients_full_reduce(r_test_dit->coeffs, tmp_copy.coeffs);
    my_poly_invntt_dit_full_reduction(r_test_dit);

    // DIF MUL
    my_poly_ntt_dif_full_reduction(r_test_dif);
    my_poly_ntt_dif_full_reduction(tmp);
    mul_coefficients_full_reduce(r_test_dif->coeffs, tmp->coeffs);
    my_poly_invntt_dif_full_reduction(r_test_dif);

    uint16_t res = compare(r_test_dit, r_test_dif, "MUL DIT vs MUL DIF");

    // Revert back to original value
    copy_poly(r_test_dit, origin_poly);
    copy_poly(r_test_dif, origin_poly);
    copy_poly(tmp, origin_poly);

    return res;
}

/*
Sanity check when all input are 0, and 1 and fix random number
Status: PASS
*/
uint16_t test9(poly *r_test_dit, poly *r_test_dif, poly *origin_poly)
{
    printf("TEST9\n");
    // Assign 0
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        r_test_dit->coeffs[i] = 0;
        r_test_dif->coeffs[i] = 0;
    }

    my_poly_invntt_dit_full_reduction(r_test_dit);
    my_poly_invntt_dif_full_reduction(r_test_dif);

    uint16_t res = compare(r_test_dit, r_test_dif, "0 DIT vs DIF");

    // Assign 1
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        r_test_dit->coeffs[i] = 1;
        r_test_dif->coeffs[i] = 1;
    }

    my_poly_invntt_dit_full_reduction(r_test_dit);
    my_poly_invntt_dif_full_reduction(r_test_dif);

    res = compare(r_test_dit, r_test_dif, "1 DIT vs DIF");

    // Assign, random number
    uint16_t t = rand() % NEWHOPE_Q;
    printf("%d: ", t);
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        r_test_dit->coeffs[i] = t;
        r_test_dif->coeffs[i] = t;
    }

    my_poly_invntt_dit_full_reduction(r_test_dit);
    my_poly_invntt_dif_full_reduction(r_test_dif);

    res = compare(r_test_dit, r_test_dif, "same value random DIT vs DIF");

    copy_poly(r_test_dit, origin_poly);
    copy_poly(r_test_dif, origin_poly);

    return res;
}

/*
Montgomery + Full reduction test 
Test if ntt_dit can perform forward DIT and invese DIT back to original version
Status: PASS
*/
uint16_t test10(poly *r_test_dit, poly *origin_poly)
{
    printf("TEST10\n");
    scramble(r_test_dit);
    my_poly_ntt_dit(r_test_dit);
    my_poly_invntt_dit(r_test_dit);
    full_reduce(r_test_dit);

    // printArray(r_test_dit->coeffs, NEWHOPE_N, "montgomery DIT");
    uint16_t res = compare(r_test_dit, origin_poly, "my_poly_ntt_dit montgomery test");

    copy_poly(r_test_dit, origin_poly);
    
    scramble(r_test_dit);
    my_poly_ntt_dit_full_reduction(r_test_dit);
    my_poly_invntt_dit_full_reduction(r_test_dit);
    res = compare(r_test_dit, origin_poly, "my_poly_ntt_dit full reduction test");

    copy_poly(r_test_dit, origin_poly);

    return res;
}

/*
Test Forward DIT/DIF and Inverse DIF/DIT to see if it's back to original version
Status: Failed
*/
uint16_t test11(poly *tmp, poly *origin_poly)
{

    scramble(tmp);
    poly_ntt(tmp);
    my_poly_invntt_dif(tmp);
    full_reduce(tmp);

    // printArray(tmp->coeffs, NEWHOPE_N, "poly_ntt-my_poly_invntt_dif array");
    uint16_t res = compare(tmp, origin_poly, "poly_ntt-my_poly_invntt_dif array");

    copy_poly(tmp, origin_poly);

    scramble(tmp);
    my_poly_ntt_dif(tmp);
    poly_invntt(tmp);
    full_reduce(tmp);

    // printArray(tmp->coeffs, NEWHOPE_N, "my_poly_ntt_dif-poly_invntt array");
    res = compare(tmp, origin_poly, "my_poly_ntt_dif-poly_invntt");

    copy_poly(tmp, origin_poly);

    // TODO: Debug this case
    scramble(tmp);
    my_poly_ntt_dit(tmp);
    my_poly_invntt_dif(tmp);
    full_reduce(tmp);

    printArray(tmp->coeffs, NEWHOPE_N, "my_poly_ntt_dit-my_poly_invntt_dif array");
    res = compare(tmp, origin_poly, "my_poly_ntt_dit-my_poly_invntt_dif test");

    copy_poly(tmp, origin_poly);

    return res;
}

int main()
{
    poly r_gold,
        r_test_dif,
        r_test_dit,
        origin_poly, 
        tmp;
    uint16_t a, b, c, d;
    srand((unsigned int)&r_gold);

    for (uint16_t i = 0; i < NEWHOPE_N; i += 4)
    {
        a = i + 0;
        b = i + 1;
        c = i + 2;
        d = i + 3;

        // a = rand() % NEWHOPE_Q;
        // b = rand() % NEWHOPE_Q;
        // c = rand() % NEWHOPE_Q;
        // d = rand() % NEWHOPE_Q;

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

        tmp.coeffs[i]     = a;
        tmp.coeffs[i + 1] = b;
        tmp.coeffs[i + 2] = c;
        tmp.coeffs[i + 3] = d;
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

    res = test6(&r_test_dif, &tmp, &origin_poly);

    res = test7(&r_test_dif, &tmp, &origin_poly);

    res = test8(&r_test_dit, &r_test_dif, &tmp, &origin_poly);

    res = test9(&r_test_dit, &r_test_dif, &origin_poly);

    // Important
    res = test10(&r_test_dit, &origin_poly);

    res = test11(&tmp, &origin_poly);

    /************************************ Below are trash ************************/
    return res;
}
