#ifndef BUTTERFLY_UNITS_H
#define BUTTERFLY_UNITS_H

#include "../params.h"

template <typename T2, typename T>
void butterfly(enum OPERATION mode, T *bj, T *bjlen,
               const T zeta,
               const T aj, const T ajlen)
{
    T aj1, ajlen1;
    T aj2, ajlen2;
    T aj3, ajlen3;
    T aj4, ajlen4;
    T aj5, ajlen5;

    aj1 = aj;
    ajlen1 = ajlen;

    if ((mode == INVERSE_NTT_MODE) ||
        (mode == INVERSE_NTT_MODE_BYPASS))
    {
        /* 
         * t = a[j];
         * a[j] = t + a[j + len];
         * a[j + len] = t - a[j + len];
         * a[j + len] = ((uint32_t)zeta * a[j + len]) % FALCON_Q; 
         */
        aj2 = (aj1 + ajlen1) % FALCON_Q;
        ajlen2 = (aj1 + FALCON_Q - ajlen1) % FALCON_Q;
    }
    else
    {
        aj2 = aj1;
        ajlen2 = ajlen1;
    }

    // MUL
    // t = ajlen = ((uint32_t)zeta * ajlen);
    ajlen3 = ((T2)zeta * ajlen2) % FALCON_Q;
    aj3 = aj2;

    if ((mode == FORWARD_NTT_MODE) ||
        (mode == FORWARD_NTT_MODE_BYPASS))
    {
        /* 
         * t = ((uint32_t)zeta * a[j + len]) % FALCON_Q;
         * a[j + len] = a[j] - t;
         * a[j] = a[j] + t; 
         */
        // NTT
        ajlen4 = (aj3 + FALCON_Q - ajlen3) % FALCON_Q;
        aj4 = (aj3 + ajlen3) % FALCON_Q;
    }
    else
    {
        ajlen4 = ajlen3;
        aj4 = aj3;
    }

    if ((mode == INVERSE_NTT_MODE) ||
        (mode == INVERSE_NTT_MODE_BYPASS))
    {
        if (aj4 & 1)
        {
            aj5 = (aj4 >> 1) + (FALCON_Q + 1) / 2;
        }
        else
        {
            aj5 = (aj4 >> 1);
        }

        if (ajlen4 & 1)
        {
            ajlen5 = (ajlen4 >> 1) + (FALCON_Q + 1) / 2;
        }
        else
        {
            ajlen5 = (ajlen4 >> 1);
        }
    }
    else
    {
        aj5 = aj4;
        ajlen5 = ajlen4;
    }

    *bj = aj5 % FALCON_Q;
    *bjlen = ajlen5 % FALCON_Q;
}

template <typename T2, typename T>
void buttefly_circuit(T data_out[4], const T data_in[4], const T w[4], enum OPERATION mode)
{
    // 4 pipeline stages
    T w1, w2, w3, w4;
    T save_a, save_b, save_c, save_d;
    T a0, b0, c0, d0;
    T a1, b1, c1, d1;
    T a2, b2, c2, d2;
    T a3, b3, c3, d3;

    a0 = data_in[0];
    b0 = data_in[1];
    c0 = data_in[2];
    d0 = data_in[3];

    w1 = w[0];
    w2 = w[1];
    w3 = w[2];
    w4 = w[3];

    /* For debugging purpose
    if ((ram_i < 64 || ram_i > 192) && (s > 2))
    {
        // index
        printf("%d, %d | %d\n", ram_i, ram_i + 1, i1);
        printf("%d, %d | %d\n", ram_i + 2, ram_i + 3, i2);
        // value
        printf("%d %d | %d\n", a, b, i1);
        printf("%d %d | %d\n", c, d, i2);
    } */

    // printf("buf: %u, %u, |%u| %u, %u |%u|\n", a0, b0, w1, c0, d0, w2);
    butterfly<T2, T>(mode, &a1, &b1, w1, a0, b0);
    butterfly<T2, T>(mode, &c1, &d1, w2, c0, d0);

    save_a = a1;
    save_b = b1;
    save_c = c1;
    save_d = d1;

    a2 = a1;
    c2 = b1;

    if (mode == MUL_MODE)
    {
        // switch lane A -> B, C->D
        b2 = a1;
        d2 = c1;
    }
    else
    {
        b2 = c1;
        d2 = d1;
    }

    /* For debugging purpose
    if ((ram_i < 64 || ram_i > 192) && (s > 2))
    {
        // index
        printf("%d, %d | %d\n", ram_i, ram_i + 2, i3);
        printf("%d, %d | %d\n", ram_i + 1, ram_i + 3, i4);
        // value
        printf("%d %d | %d\n", a, b, i3);
        printf("%d %d | %d\n", c, d, i4);
        printf("==============================%d %d | %d %d\n", ram_i / 4, ram_i, j, k);
    } */

    // printf("buf: %u, %u, |%u| %u, %u |%u|\n", a2, b2, w3, c2, d2, w4);
    butterfly<T>(mode, &a3, &b3, w3, a2, b2);
    butterfly<T>(mode, &c3, &d3, w4, c2, d2);

    switch (mode)
    {
    case MUL_MODE:
        data_out[0] = b3;
        data_out[1] = save_b;
        data_out[2] = d3;
        data_out[3] = save_d;
        break;
    
    case FORWARD_NTT_MODE_BYPASS:
        data_out[0] = save_a;
        data_out[1] = save_b;
        data_out[2] = save_c;
        data_out[3] = save_d;
        break;
    default:
        data_out[0] = a3;
        data_out[1] = b3;
        data_out[2] = c3;
        data_out[3] = d3;
        break;
    }
}

#endif
