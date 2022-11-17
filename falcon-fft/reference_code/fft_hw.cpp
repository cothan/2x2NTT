#include "fft.h"
#include "../consts.h"
#include <stddef.h>

void fwd_FFT_adj_short(fpr *f, unsigned logn)
{
    const unsigned n = 1 << logn;
    const unsigned hn = n >> 1;
    unsigned len, start, j, k;
    fpr zeta_re, zeta_im, t_re, t_im, a_re, a_im, b_re, b_im;

    /*
     * We read the twiddle table in forward order
     */
    int level = 0;
    const fpr *fpr_tab = NULL;

    for (len = hn; len > 1; len >>= 1)
    {
        fpr_tab = fpr_table[level++];
        k = 0;
        for (start = 0; start < n;)
        {
            zeta_re = fpr_tab[k];
            zeta_im = fpr_tab[k + 1];
            k += 2;

            for (j = start; j < start + len; j += 2)
            {
                a_re = f[j];
                a_im = f[j + 1];
                b_re = f[j + len];
                b_im = f[j + len + 1];

                FPC_MUL(t_re, t_im, b_re, b_im, zeta_re, zeta_im);
                FPC_SUB(f[j + len], f[j + len + 1], a_re, a_im, t_re, t_im);
                FPC_ADD(f[j], f[j + 1], a_re, a_im, t_re, t_im);

                #if DEBUG == 1
                    printf("fwd_1st: (t0, t1) = (%d, %d) * (%d, %d)\n", j + len, j + len + 1, k - 2, k - 1);
                    printf("fwd_1st: (%2d, %2d) = (%d, %d) - (t0, t1)\n", j + len, j + len + 1, j, j + 1);
                    printf("fwd_1st: (%2d, %2d) = (%d, %d) + (t0, t1)\n\n", j, j + 1, j + len, j + len + 1);
                #endif
            }

            start = j + len;
            if (start >= n)
            {
                // printf("fwd break\n");
                break;
            }

            for (j = start; j < start + len; j += 2)
            {
                a_re = f[j];
                a_im = f[j + 1];
                b_re = f[j + len];
                b_im = f[j + len + 1];

                FPC_MUL(t_re, t_im, b_re, b_im, zeta_re, zeta_im);
                FPC_SUBJ(f[j + len], f[j + len + 1], a_re, a_im, t_re, t_im);
                FPC_ADDJ(f[j], f[j + 1], a_re, a_im, t_re, t_im);

                #if DEBUG == 1
                    printf("fwd_2nd: (t0, t1) = (%d, %d) * (%d, %d)\n", j + len, j + len + 1, k - 2, k - 1);
                    printf("fwd_2nd: (%2d, %2d) = (%d, %d) - J(t0, t1)\n", j + len, j + len + 1, j, j + 1);
                    printf("fwd_2nd: (%2d, %2d) = (%d, %d) + J(t0, t1)\n\n", j, j + 1, j + len, j + len + 1);
                #endif
            }

            start = j + len;
        }
    }
}

void inv_FFT_adj_short(fpr *f, unsigned logn)
{
    const unsigned n = 1 << logn;
    unsigned len, start, j, k;
    fpr zeta_re, zeta_im, t_re, t_im, a_re, a_im, b_re, b_im;

    /*
     * This time we read the table in reverse order,
     * so the pointer point to the end of the table
     */
    int level = logn - 2;
    const fpr *fpr_tab_inv = NULL;

    for (len = 2; len < n; len <<= 1)
    {
        fpr_tab_inv = fpr_table[level--];
        k = 0;

        for (start = 0; start < n; start = j + len)
        {
            // Conjugate of zeta is embeded in MUL
            zeta_re = fpr_tab_inv[k];
            zeta_im = fpr_tab_inv[k + 1];
            k += 2;

            for (j = start; j < start + len; j += 2)
            {
                a_re = f[j];
                a_im = f[j + 1];
                b_re = f[j + len];
                b_im = f[j + len + 1];

                FPC_ADD(f[j], f[j + 1], a_re, a_im, b_re, b_im);
                FPC_SUB(t_re, t_im, a_re, a_im, b_re, b_im);
                FPC_MUL_CONJ(f[j + len], f[j + len + 1], t_re, t_im, zeta_re, zeta_im);

                #if DEBUG == 1
                    printf("inv_1st: (%2d, %2d) = (%d, %d) + (%d, %d)\n", j, j+1, j, j+1, j + len, j + len + 1);
                    printf("inv_1st: (t0, t1) = (%d, %d) - (%d, %d)\n", j, j+1, j + len, j + len + 1);
                    printf("inv_1st: (%2d, %2d) = (t0, t1) * (%d, %d)\n\n", j + len, j + len + 1, k - 2, k - 1);
                #endif
            }

            start = j + len;
            if (start >= n)
            {
                // printf("inv break\n");
                break;
            }

            for (j = start; j < start + len; j += 2)
            {
                a_re = f[j];
                a_im = f[j + 1];
                b_re = f[j + len];
                b_im = f[j + len + 1];

                /*
                 * Notice we swap the (a - b) to (b - a) in FPC_SUB
                 */
                FPC_ADD(f[j], f[j + 1], a_re, a_im, b_re, b_im);
                FPC_SUB(t_re, t_im, b_re, b_im, a_re, a_im);
                FPC_MUL_CONJ_J_m(f[j + len], f[j + len + 1], t_re, t_im, zeta_re, zeta_im);

                #if DEBUG == 1
                    printf("inv_2nd: (%2d, %2d) = (%d, %d) + (%d, %d)\n", j, j+1, j, j+1, j + len, j + len + 1);
                    printf("inv_2nd: (t0, t1) = (%d, %d) - (%d, %d)\n", j, j+1, j + len, j + len + 1);
                    printf("inv_2nd: (%2d, %2d) = (t0, t1) * - J(%d, %d)\n\n", j + len, j + len + 1, k - 2, k - 1);
                #endif
            }
        }
    }

    for (j = 0; j < n; j += 2)
    {
        f[j] *= fpr_p2_tab[logn];
        f[j + 1] *= fpr_p2_tab[logn];
    }
}
