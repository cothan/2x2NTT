/*
 * High-speed FFT code for arbitrary `logn`.
 *
 * =============================================================================
 * Copyright (c) 2022 by Cryptographic Engineering Research Group (CERG)
 * ECE Department, George Mason University
 * Fairfax, VA, U.S.A.
 * Author: Duc Tri Nguyen
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * =============================================================================
 * @author   Duc Tri Nguyen <dnguye69@gmu.edu>
 */

#include <assert.h>
#include "stddef.h"
#include "consts.h"
#include "fft.h"
#include <stdio.h>

#define DEBUG 0

void FFT(fpr *f, unsigned logn)
{
    const unsigned n = 1 << logn;
    const unsigned hn = n >> 1;
    const unsigned ht = n >> 2;
    unsigned len, start, j, k;
    fpr zeta_re, zeta_im, t_re, t_im, a_re, a_im, b_re, b_im;

    /* Only support logn >= 2 */
    if (logn < 2)
        return;

    /*
     * We read the twiddle table in forward order
     */
    int level = 1;
    const fpr *fpr_tab = fpr_table[0];
    zeta_re = fpr_tab[0];
    zeta_im = fpr_tab[1];

    for (j = 0; j < ht; j += 1)
    {
        a_re = f[j];
        a_im = f[j + hn];
        b_re = f[j + ht];
        b_im = f[j + ht + hn];

        FPC_MUL(t_re, t_im, b_re, b_im, zeta_re, zeta_im);
        FPC_SUB(f[j + ht], f[j + ht + hn], a_re, a_im, t_re, t_im);
        FPC_ADD(f[j], f[j + hn], a_re, a_im, t_re, t_im);
    }

    for (len = ht / 2; len > 0; len >>= 1)
    {
        fpr_tab = fpr_table[level++];
        k = 0;
        for (start = 0; start < hn; start = j + len)
        {
            zeta_re = fpr_tab[k++];
            zeta_im = fpr_tab[k++];

            for (j = start; j < start + len; j += 1)
            {
                a_re = f[j];
                a_im = f[j + hn];
                b_re = f[j + len];
                b_im = f[j + len + hn];

                FPC_MUL(t_re, t_im, b_re, b_im, zeta_re, zeta_im);
                FPC_SUB(f[j + len], f[j + len + hn], a_re, a_im, t_re, t_im);
                FPC_ADD(f[j], f[j + hn], a_re, a_im, t_re, t_im);
            }

            start = j + len;

            for (j = start; j < start + len; j += 1)
            {
                a_re = f[j];
                a_im = f[j + hn];
                b_re = f[j + len];
                b_im = f[j + len + hn];

                FPC_MUL(t_re, t_im, b_re, b_im, zeta_re, zeta_im);
                FPC_SUBJ(f[j + len], f[j + len + hn], a_re, a_im, t_re, t_im);
                FPC_ADDJ(f[j], f[j + hn], a_re, a_im, t_re, t_im);
            }
        }
    }
}

void iFFT(fpr *f, unsigned logn)
{
    const unsigned n = 1 << logn;
    const unsigned hn = n >> 1;
    const unsigned ht = n >> 2;
    unsigned len, start, j, k;
    fpr zeta_re, zeta_im, t_re, t_im, a_re, a_im, b_re, b_im;

    /* Only support logn >= 2 */
    if (logn < 2)
        return;
    /*
     * This time we read the table in reverse order,
     * so the pointer point to the end of the table
     */
    int level = logn - 2;
    const fpr *fpr_tab_inv = NULL;

    for (len = 1; len < ht; len <<= 1)
    {
        fpr_tab_inv = fpr_table[level--];
        k = 0;
        for (start = 0; start < hn; start = j + len)
        {
            // Conjugate of zeta is embeded in MUL
            zeta_re = fpr_tab_inv[k++];
            zeta_im = fpr_tab_inv[k++];

            for (j = start; j < start + len; j += 1)
            {
                a_re = f[j];
                a_im = f[j + hn];
                b_re = f[j + len];
                b_im = f[j + len + hn];

                FPC_SUB(t_re, t_im, a_re, a_im, b_re, b_im);
                FPC_ADD(f[j], f[j + hn], a_re, a_im, b_re, b_im);
                FPC_MUL_CONJ(f[j + len], f[j + len + hn], t_re, t_im, zeta_re, zeta_im);
            }

            start = j + len;

            for (j = start; j < start + len; j += 1)
            {
                a_re = f[j];
                a_im = f[j + hn];
                b_re = f[j + len];
                b_im = f[j + len + hn];

                /*
                 * Notice we swap the (a - b) to (b - a) in FPC_SUB
                 */
                FPC_SUB(t_re, t_im, b_re, b_im, a_re, a_im);
                FPC_ADD(f[j], f[j + hn], a_re, a_im, b_re, b_im);
                FPC_MUL_CONJ_J_m(f[j + len], f[j + len + hn], t_re, t_im, zeta_re, zeta_im);
            }
        }
    }

    fpr_tab_inv = fpr_table[0];
    zeta_re = (fpr_tab_inv[0] * fpr_p2_tab[logn]);
    zeta_im = (fpr_tab_inv[1] * fpr_p2_tab[logn]);
    for (j = 0; j < ht; j += 1)
    {
        a_re = f[j];
        a_im = f[j + hn];
        b_re = f[j + ht];
        b_im = f[j + ht + hn];

        FPC_SUB(t_re, t_im, a_re, a_im, b_re, b_im);
        FPC_ADD(f[j], f[j + hn], a_re, a_im, b_re, b_im);
        FPC_MUL_CONJ(f[j + ht], f[j + ht + hn], t_re, t_im, zeta_re, zeta_im);

        f[j] = (f[j] * fpr_p2_tab[logn]);
        f[j + hn] = (f[j + hn] * fpr_p2_tab[logn]);
    }
}

void fwd_FFT_short(fpr *f, unsigned logn)
{
    const unsigned n = 1 << logn;
    const unsigned hn = n >> 1;
    const unsigned ht = n >> 2;
    unsigned len, start, j, k;
    fpr zeta_re, zeta_im, t_re, t_im, a_re, a_im, b_re, b_im;

    /*
     * We read the twiddle table in forward order
     */
    int level = 0;
    const fpr *fpr_tab = NULL;

    for (len = ht; len > 0; len >>= 1)
    {
        fpr_tab = fpr_table[level++];
        k = 0;
        for (start = 0; start < hn; start = j + len)
        {
            zeta_re = fpr_tab[k];
            zeta_im = fpr_tab[k + 1];
            k += 2;

            for (j = start; j < start + len; j += 1)
            {
                a_re = f[j];
                a_im = f[j + hn];
                b_re = f[j + len];
                b_im = f[j + len + hn];

                FPC_MUL(t_re, t_im, b_re, b_im, zeta_re, zeta_im);
                FPC_SUB(f[j + len], f[j + len + hn], a_re, a_im, t_re, t_im);
                FPC_ADD(f[j], f[j + hn], a_re, a_im, t_re, t_im);
            }

            start = j + len;
            if (start >= hn)
                break;

            for (j = start; j < start + len; j += 1)
            {
                a_re = f[j];
                a_im = f[j + hn];
                b_re = f[j + len];
                b_im = f[j + len + hn];

                FPC_MUL(t_re, t_im, b_re, b_im, zeta_re, zeta_im);
                FPC_SUBJ(f[j + len], f[j + len + hn], a_re, a_im, t_re, t_im);
                FPC_ADDJ(f[j], f[j + hn], a_re, a_im, t_re, t_im);
            }
        }
    }
}

void inv_FFT_short(fpr *f, unsigned logn)
{
    const unsigned n = 1 << logn;
    const unsigned hn = n >> 1;
    unsigned len, start, j, k;
    fpr zeta_re, zeta_im, t_re, t_im, a_re, a_im, b_re, b_im;

    /*
     * This time we read the table in reverse order,
     * so the pointer point to the end of the table
     */
    int level = logn - 2;
    const fpr *fpr_tab_inv = NULL;

    for (len = 1; len < hn; len <<= 1)
    {
        fpr_tab_inv = fpr_table[level--];
        k = 0;

        for (start = 0; start < hn; start = j + len)
        {
            // Conjugate of zeta is embeded in MUL
            zeta_re = fpr_tab_inv[k];
            zeta_im = fpr_tab_inv[k + 1];
            k += 2;

            for (j = start; j < start + len; j += 1)
            {
                a_re = f[j];
                a_im = f[j + hn];
                b_re = f[j + len];
                b_im = f[j + len + hn];

                printf("1st: %d\n", j);

                FPC_ADD(f[j], f[j + hn], a_re, a_im, b_re, b_im);
                FPC_SUB(t_re, t_im, a_re, a_im, b_re, b_im);
                FPC_MUL_CONJ(f[j + len], f[j + len + hn], t_re, t_im, zeta_re, zeta_im);
            }

            start = j + len;
            if (start >= hn)
                break;

            for (j = start; j < start + len; j += 1)
            {
                a_re = f[j];
                a_im = f[j + hn];
                b_re = f[j + len];
                b_im = f[j + len + hn];

                /*
                 * Notice we swap the (a - b) to (b - a) in FPC_SUB
                 */
                FPC_ADD(f[j], f[j + hn], a_re, a_im, b_re, b_im);
                FPC_SUB(t_re, t_im, b_re, b_im, a_re, a_im);
                FPC_MUL_CONJ_J_m(f[j + len], f[j + len + hn], t_re, t_im, zeta_re, zeta_im);
            }
        }
    }

    for (j = 0; j < hn; j += 1)
    {
        f[j] *= fpr_p2_tab[logn];
        f[j + hn] *= fpr_p2_tab[logn];
    }
}

/* Another variant of FFT where real and imagine are store in adjacent */

void fwd_FFT_adj(fpr *f, unsigned logn)
{
    const unsigned n = 1 << logn;
    const unsigned hn = n >> 1;
    unsigned len, start, j, k;
    fpr zeta_re, zeta_im, t_re, t_im, a_re, a_im, b_re, b_im;

    /*
     * We read the twiddle table in forward order
     */
    int level = 1;
    const fpr *fpr_tab = NULL;

    fpr_tab = fpr_table[0];
    zeta_re = fpr_tab[0];
    zeta_im = fpr_tab[1];
    for (j = 0; j < hn; j += 2)
    {
        a_re = f[j];
        a_im = f[j + 1];
        b_re = f[j + hn];
        b_im = f[j + hn + 1];

        FPC_MUL(t_re, t_im, b_re, b_im, zeta_re, zeta_im);
        FPC_SUB(f[j + hn], f[j + hn + 1], a_re, a_im, t_re, t_im);
        FPC_ADD(f[j], f[j + 1], a_re, a_im, t_re, t_im);
    }

    for (len = hn / 2; len > 1; len >>= 1)
    {
        fpr_tab = fpr_table[level++];
        k = 0;
        for (start = 0; start < n; start = j + len)
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
            }

            start = j + len;

            for (j = start; j < start + len; j += 2)
            {
                a_re = f[j];
                a_im = f[j + 1];
                b_re = f[j + len];
                b_im = f[j + len + 1];

                FPC_MUL(t_re, t_im, b_re, b_im, zeta_re, zeta_im);
                FPC_SUBJ(f[j + len], f[j + len + 1], a_re, a_im, t_re, t_im);
                FPC_ADDJ(f[j], f[j + 1], a_re, a_im, t_re, t_im);
            }
        }
    }
}

void inv_FFT_adj(fpr *f, unsigned logn)
{
    const unsigned n = 1 << logn;
    const unsigned hn = n >> 1;
    unsigned len, start, j, k;
    fpr zeta_re, zeta_im, t_re, t_im, a_re, a_im, b_re, b_im;

    /*
     * This time we read the table in reverse order,
     * so the pointer point to the end of the table
     */
    int level = logn - 2;
    const fpr *fpr_tab_inv = NULL;

    for (len = 2; len < hn; len <<= 1)
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
            }

            start = j + len;

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
            }
        }
    }

    fpr_tab_inv = fpr_table[0];
    zeta_re = fpr_tab_inv[0] * fpr_p2_tab[logn];
    zeta_im = fpr_tab_inv[1] * fpr_p2_tab[logn];

    for (j = 0; j < hn; j += 2)
    {
        a_re = f[j];
        a_im = f[j + 1];
        b_re = f[j + hn];
        b_im = f[j + hn + 1];

        FPC_ADD(f[j], f[j + 1], a_re, a_im, b_re, b_im);
        FPC_SUB(t_re, t_im, a_re, a_im, b_re, b_im);
        FPC_MUL_CONJ(f[j + hn], f[j + hn + 1], t_re, t_im, zeta_re, zeta_im);

        f[j] *= fpr_p2_tab[logn];
        f[j + 1] *= fpr_p2_tab[logn];
    }
}




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
    const unsigned hn = n >> 1;
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
