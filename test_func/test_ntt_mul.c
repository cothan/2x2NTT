#include "../ref/newhope_ntt.h"
#include "../hls/hls_ntt_mul.h"

int compare(poly *r, uint64_t hls_r)
{
    uint16_t a[4];
    uint16_t b[4];
    bool error = false;
    for (uint16_t i = 0; i < NEWHOPE_N; i += 4)
    {
        a[0] = r->coeffs[i];
        a[1] = r->coeffs[i + 1];
        a[2] = r->coeffs[i + 2];
        a[3] = r->coeffs[i + 3];

        unpack(hls_r, i / 4, &b[0], &b[1], &b[2], &b[3]);

        for (uint16_t j = 0; j < 4; j++)
        {
            if (a[j] != b[j])
            {
                printf("[%u] %u != %u", i + j, a[j], b[j]);
                error = true;
            }
        }
        if (error)
            return 1;
    }
    return 0;
}

int main()
{
    poly r_gold;
    uint64_t hls_r[NEWHOPE_N / 4];
    for (uint16_t i = 0; i < NEWHOPE_N; i += 4)
    {
        r_gold.coeffs[i] = i;
        r_gold.coeffs[i + 1] = i + 1;
        r_gold.coeffs[i + 2] = i + 2;
        r_gold.coeffs[i + 3] = i + 3;

        pack(i, i + 1, i + 2, i + 3, hls_r, i / 4);
    }
    mul_coefficients(r_gold, gammas_bitrev_montgomery);
    hls_poly_ntt_mul(hls_r, PSIS);

    uint16_t res = compare(r_gold, hls_r);

    return res;
}