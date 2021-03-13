#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "params.h"
#include "ntt.h"
#include "ntt2x2.h"
#include "util.h"
#include "reduce.h"

// Don't change this 
#define DEPT_I 3
#define DEPT_A 4
#define DEPT_B 6
#define DEPT_C 5
#define DEPT_D 7

#define LOG_N  8 // N= 256, => log(N) = 8

enum OP {NTT_MODE, MUL_MODE, DECODE_TRUE, DECODE_FALSE};

static const int32_t zetas[N] = {
         0,    25847, -2608894,  -518909,   237124,  -777960,  -876248,   466468,
   1826347,  2353451,  -359251, -2091905,  3119733, -2884855,  3111497,  2680103,
   2725464,  1024112, -1079900,  3585928,  -549488, -1119584,  2619752, -2108549,
  -2118186, -3859737, -1399561, -3277672,  1757237,   -19422,  4010497,   280005,
   2706023,    95776,  3077325,  3530437, -1661693, -3592148, -2537516,  3915439,
  -3861115, -3043716,  3574422, -2867647,  3539968,  -300467,  2348700,  -539299,
  -1699267, -1643818,  3505694, -3821735,  3507263, -2140649, -1600420,  3699596,
    811944,   531354,   954230,  3881043,  3900724, -2556880,  2071892, -2797779,
  -3930395, -1528703, -3677745, -3041255, -1452451,  3475950,  2176455, -1585221,
  -1257611,  1939314, -4083598, -1000202, -3190144, -3157330, -3632928,   126922,
   3412210,  -983419,  2147896,  2715295, -2967645, -3693493,  -411027, -2477047,
   -671102, -1228525,   -22981, -1308169,  -381987,  1349076,  1852771, -1430430,
  -3343383,   264944,   508951,  3097992,    44288, -1100098,   904516,  3958618,
  -3724342,    -8578,  1653064, -3249728,  2389356,  -210977,   759969, -1316856,
    189548, -3553272,  3159746, -1851402, -2409325,  -177440,  1315589,  1341330,
   1285669, -1584928,  -812732, -1439742, -3019102, -3881060, -3628969,  3839961,
   2091667,  3407706,  2316500,  3817976, -3342478,  2244091, -2446433, -3562462,
    266997,  2434439, -1235728,  3513181, -3520352, -3759364, -1197226, -3193378,
    900702,  1859098,   909542,   819034,   495491, -1613174,   -43260,  -522500,
   -655327, -3122442,  2031748,  3207046, -3556995,  -525098,  -768622, -3595838,
    342297,   286988, -2437823,  4108315,  3437287, -3342277,  1735879,   203044,
   2842341,  2691481, -2590150,  1265009,  4055324,  1247620,  2486353,  1595974,
  -3767016,  1250494,  2635921, -3548272, -2994039,  1869119,  1903435, -1050970,
  -1333058,  1237275, -3318210, -1430225,  -451100,  1312455,  3306115, -1962642,
  -1279661,  1917081, -2546312, -1374803,  1500165,   777191,  2235880,  3406031,
   -542412, -2831860, -1671176, -1846953, -2584293, -3724270,   594136, -3776993,
  -2013608,  2432395,  2454455,  -164721,  1957272,  3369112,   185531, -1207385,
  -3183426,   162844,  1616392,  3014001,   810149,  1652634, -3694233, -1799107,
  -3038916,  3523897,  3866901,   269760,  2213111,  -975884,  1717735,   472078,
   -426683,  1723600, -1803090,  1910376, -1667432, -1104333,  -260646, -3833893,
  -2939036, -2235985,  -420899, -2286327,   183443,  -976891,  1612842, -3545687,
   -554416,  3919660,   -48306, -1362209,  3937738,  1400424,  -846154,  1976782
};

int32_t MUL_RAM[N] = {
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
    41978, 41978, 41978, 41978, 41978, 41978, 41978, 41978, 
};

// This function will output an element when insert 1 element
int32_t FIFO(const int dept, int32_t *fifo, const int32_t new)
{
    int32_t out = fifo[dept - 1];
    for (int i = dept - 1; i > -1; i--)
    {
        fifo[i] = fifo[i - 1];
    }
    fifo[0] = new;

    return out;
}

void print_reshaped_array(bram *ram, int bound, const char *string)
{
    printf("%s :\n", string);
    for (int i = 0; i < bound; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%d, ", ram->vec[i].coeffs[j]);
        }
    }
    printf("\n");
}

void swap(int32_t *a, int32_t *b)
{
    int32_t tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}


/* This code copied from butterfly unit in Dilithium Forward NTT
t = montgomery_reduce((int64_t)zeta * a[j + len]);
a[j + len] = a[j] - t;
a[j] = a[j] + t; */
void forward_butterfly(int32_t *bj, int32_t *bjlen, const int32_t zeta, int32_t aj, int32_t ajlen)
{
    int32_t t;

    t = montgomery_reduce((int64_t)zeta * ajlen);
    ajlen = aj - t;
    aj = aj + t;
    *bj = aj;
    *bjlen = ajlen;
}


/* This code copied from butterfly unit in Dilithium Inverse NTT
t = a[j];
a[j] = t + a[j + len];
a[j + len] = t - a[j + len];
a[j + len] = montgomery_reduce((int64_t)zeta * a[j + len]); */
void inverse_butterfly(int mode, int32_t *bj, int32_t *bjlen, const int32_t zeta, int32_t aj, int32_t ajlen)
{
    int32_t t;

    if (mode == NTT_MODE)
    {
        // NTT 
        t = aj; 
    }
    else{
        // MUL
        t = ajlen << 1;
    }

    aj = t + ajlen;
    ajlen = t - ajlen;
    ajlen = montgomery_reduce((int64_t) zeta * ajlen);

    *bj = aj;
    *bjlen = ajlen;
}

// Store 4 coefficients per line
void reshape(bram *ram, int32_t in[N])
{
    for (int i = 0; i < N / 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            ram->vec[i].coeffs[j] = in[4 * i + j];
        }
    }
}

/* Lazy, avoid transpose the matrix */
int addr_decoder(int addr_in)
{
    /* 
    [ 0  4  8 12] <= [  0* 1  2  3 ]
    [16 20 24 28] <= [  4  5  6  7 ]
    [32 36 40 44] <= [  8  9 10 11 ]
    [48 52 56 60] <= [ 12 13 14 15 ]

    [ 1  5  9 13] <= [ 16 17  18 19 ]
    [17 21 25 29] <= [ 20 21* 22 23 ]
    [33 37 41 45] <= [ 24 25  26 27 ]
    [49 53 57 61] <= [ 28 29  30 31 ]

    [ 2  6 10 14] <= [ 32 33 34  35 ]
    [18 22 26 30] <= [ 36 37 38  39 ]
    [34 38 42 46] <= [ 40 41 42* 43 ]
    [50 54 58 62] <= [ 44 45 46  47 ]

    [ 3  7 11 15] <= [ 48 49 50 51 ]
    [19 23 27 31] <= [ 52 53 54 55 ]
    [35 39 43 47] <= [ 56 57 58 59 ]
    [51 55 59 63] <= [ 60 61 62 63*] 
    */
    const int my_map[N / 4] = {0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60,
                               1, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61,
                               2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62,
                               3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 59, 63};
    return my_map[addr_in];
}

void ntt2x2(bram *ram, int32_t *mul_ram, int mode, int decode)
{
    int32_t fifo_i[DEPT_I] = {0},
            fifo_a[DEPT_A] = {0},
            fifo_b[DEPT_B] = {0},
            fifo_c[DEPT_C] = {0},
            fifo_d[DEPT_D] = {0};
    int32_t a, b, c, d;
    int32_t fa, fb, fc, fd, fi;
    int32_t save_a, save_b, save_c, save_d;

    int i1, i2, i3, i4;
    int ram_i, bound, addr;

    int count = 0;
    int write = 0;
    int m_counter = 0;

    const int w_m1 = 2;
    const int w_m2 = 1;

    int32_t w1, w2, w3, w4;
    

    if (mode == NTT_MODE)
    {
        bound = LOG_N;
    }
    else{
        bound = 1;
    }

    // iterate 2 levels at a time
    for (int s = 0; s < bound; s += 2)
    {
        for (int j = 0; j < (1 << s); j++)
        {
            if (mode == NTT_MODE)
            {
                // NTT mode
                // Layer s 
                i1 = (N >> s) - 1;
                i2 = (N >> s) - 2;
                // Layer s + 1
                i4 = i3 = (N >> (s+1)) - 1;
            }
            

            for (int k = 0; k < N/4; k += 1 << s)
            {
                if (mode == MUL_MODE)
                {
                    // MUL mode 
                    i1 = m_counter++;
                    i2 = m_counter++;
                    i3 = m_counter++;
                    i4 = m_counter++;
                }

                addr = k + j; 
                if (decode == DECODE_TRUE)
                {
                    ram_i = addr_decoder(addr);
                }
                else
                {
                    ram_i = addr;
                }

                if (mode == NTT_MODE)
                {
                    w1 = -zetas[i1];
                    w2 = -zetas[i2];
                    w3 = -zetas[i3];
                    w4 = -zetas[i4];
                }
                else{
                    w1 = mul_ram[i1];
                    w2 = mul_ram[i2];
                    w3 = mul_ram[i3];
                    w4 = mul_ram[i4];
                }

                a = ram->vec[ram_i].coeffs[0];
                b = ram->vec[ram_i].coeffs[1];
                c = ram->vec[ram_i].coeffs[2];
                d = ram->vec[ram_i].coeffs[3];

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
                save_a = a; 
                save_c = c;

                inverse_butterfly(mode, &a, &b, w1, a, b);
                inverse_butterfly(mode, &c, &d, w2, c, d);

                if (mode == MUL_MODE)
                {
                    // bypass to FIFO
                    save_b = b; 
                    save_d = d;
                }

                swap(&b, &c);

                if (mode == MUL_MODE)
                {
                    // switch lane A -> B, C->D 
                    b = save_a;
                    d = save_c;
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

                inverse_butterfly(mode, &a, &b, w3, a, b);
                inverse_butterfly(mode, &c, &d, w4, c, d);

                if (mode == MUL_MODE)
                {
                    // switch lane again, B->A, D->C
                    a = b; 
                    c = d; 
                    b = save_b;
                    d = save_d;
                }

                fi = FIFO(DEPT_I, fifo_i, ram_i);
                fa = FIFO(DEPT_A, fifo_a, a);
                fb = FIFO(DEPT_B, fifo_b, b);
                fc = FIFO(DEPT_C, fifo_c, c);
                fd = FIFO(DEPT_D, fifo_d, d);

                if (count == DEPT_I)
                {
                    // FIFO_A is full
                    write = 1;
                    count = 0;
                }
                else
                {
                    count++;
                }

                if (write)
                {
                    if (mode == NTT_MODE)
                    {
                        // Serial in Parallel out
                        switch (count & 0b11)
                        {
                        case 0:
                            fa = fifo_a[DEPT_A - 1];
                            fb = fifo_a[DEPT_A - 2];
                            fc = fifo_a[DEPT_A - 3];
                            fd = fifo_a[DEPT_A - 4];
                            break;

                        case 2:
                            fa = fifo_b[DEPT_B - 1];
                            fb = fifo_b[DEPT_B - 2];
                            fc = fifo_b[DEPT_B - 3];
                            fd = fifo_b[DEPT_B - 4];
                            break;
                        case 1:
                            fa = fifo_c[DEPT_C - 1];
                            fb = fifo_c[DEPT_C - 2];
                            fc = fifo_c[DEPT_C - 3];
                            fd = fifo_c[DEPT_C - 4];
                            break;
                        case 3:
                            fa = fifo_d[DEPT_D - 1];
                            fb = fifo_d[DEPT_D - 2];
                            fc = fifo_d[DEPT_D - 3];
                            fd = fifo_d[DEPT_D - 4];
                            break;

                        default:
                            printf("Error, suspect overflow\n");
                            break;
                        }
                    }
                    else{
                        // MUL Mode 
                        fa = fifo_a[DEPT_A-1];
                        fb = fifo_b[DEPT_B-3];
                        fc = fifo_c[DEPT_C-2];
                        fd = fifo_d[DEPT_D-4];
                    }

                    ram->vec[fi].coeffs[0] = fa;
                    ram->vec[fi].coeffs[1] = fb;
                    ram->vec[fi].coeffs[2] = fc;
                    ram->vec[fi].coeffs[3] = fd;
                    // writeback
                    // printf("[%d] <= (%d, %d, %d, %d)\n", fi, fa, fb, fc, fd);
                }
                if (mode == NTT_MODE)
                {
                    // Only adjust omega in NTT mode
                    i1 -= w_m1;
                    i2 -= w_m1;
                    i3 -= w_m2;
                    i4 -= w_m2;
                }
            }
        }
        // Decode only happen once
        decode = 0;
    }
    // Write back left over coefficients in FIFO
    for (int i = 0; i < DEPT_I; i++)
    {
        fi = FIFO(DEPT_I, fifo_i, 0);
        fa = FIFO(DEPT_A, fifo_a, 0);
        fb = FIFO(DEPT_B, fifo_b, 0);
        fc = FIFO(DEPT_C, fifo_c, 0);
        fd = FIFO(DEPT_D, fifo_d, 0);
        count++;

        if (mode == NTT_MODE)
        {
            // Serial in Parallel out
            switch (count & 0b11)
            {
            case 0:
                fa = fifo_a[DEPT_A - 1];
                fb = fifo_a[DEPT_A - 2];
                fc = fifo_a[DEPT_A - 3];
                fd = fifo_a[DEPT_A - 4];
                break;

            case 2:
                fa = fifo_b[DEPT_B - 1];
                fb = fifo_b[DEPT_B - 2];
                fc = fifo_b[DEPT_B - 3];
                fd = fifo_b[DEPT_B - 4];
                break;
            case 1:
                fa = fifo_c[DEPT_C - 1];
                fb = fifo_c[DEPT_C - 2];
                fc = fifo_c[DEPT_C - 3];
                fd = fifo_c[DEPT_C - 4];
                break;
            case 3:
                fa = fifo_d[DEPT_D - 1];
                fb = fifo_d[DEPT_D - 2];
                fc = fifo_d[DEPT_D - 3];
                fd = fifo_d[DEPT_D - 4];
                break;

            default:
                printf("Error, suspect overflow\n");
                break;
            }
        }
        else{
            // MUL Mode 
            fa = fifo_a[DEPT_A-1];
            fb = fifo_b[DEPT_B-3];
            fc = fifo_c[DEPT_C-2];
            fd = fifo_d[DEPT_D-4];
        }

        ram->vec[fi].coeffs[0] = fa;
        ram->vec[fi].coeffs[1] = fb;
        ram->vec[fi].coeffs[2] = fc;
        ram->vec[fi].coeffs[3] = fd;

        // writeback
        // printf("[%d] <= (%d, %d, %d, %d)\n", fi, fa, fb, fc, fd);
    }
    /* For debugging purpose 
    print_array(fifo_i, DEPT_I, "fifo_i");
    print_array(fifo_a, DEPT_A, "fifo_a");
    print_array(fifo_b, DEPT_B, "fifo_b");
    print_array(fifo_c, DEPT_C, "fifo_c");
    print_array(fifo_d, DEPT_D, "fifo_d"); 
    */
}

int ntt2x2_NTT(int32_t r_gold[N], int32_t r[N])
{
    bram ram;
    // Load data into BRAM, 4 coefficients per line
    reshape(&ram, r);
    // Compute NTT
    ntt2x2(&ram, MUL_RAM, NTT_MODE, DECODE_FALSE);
    
    // Enable DECODE_TRUE only after NTT transform
    ntt2x2(&ram, MUL_RAM, MUL_MODE, DECODE_TRUE);


    // Run the reference code
    invntt_tomont(r_gold);

    // Compare with the reference code
    int32_t a, b, c, d;
    int32_t ta, tb, tc, td;

    int addr;
    int ret = 0;

    // print_array(r_gold, 16, "r_gold");
    // print_reshaped_array(&ram, 4, "ram");

    for (int i = 0; i < N; i += 4)
    {
        // Get golden result
        a = r_gold[i + 0];
        b = r_gold[i + 1];
        c = r_gold[i + 2];
        d = r_gold[i + 3];

        addr = addr_decoder( i / 4 );
        ta = ram.vec[addr].coeffs[0];
        tb = ram.vec[addr].coeffs[1];
        tc = ram.vec[addr].coeffs[2];
        td = ram.vec[addr].coeffs[3];

        // Comapre with reference code

        // Quick xor, I hate long if-else clause
        ret |= a ^ ta;
        ret |= b ^ tb;
        ret |= c ^ tc;
        ret |= d ^ td;

        if (ret)
        {
            printf("Error at index: %d => %d\n", addr, i);
            printf("%12d | %12d | %12d | %12d\n", a, b, c, d);
            printf("%12d | %12d | %12d | %12d\n", ta, tb, tc, td);
            return 1;
        }
    }
    // printf("==============NTT is Correct!\n\n");
    return 0;
}

int ntt2x2_MUL(int32_t r_gold[N], int32_t r[N])
{
    bram ram;
    // Load data into BRAM, 4 coefficients per line
    reshape(&ram, r);


    // MUL Operation using NTT
    // Enable DECODE_TRUE only after NTT transform
    // This example we only do pointwise multiplication
    ntt2x2(&ram, MUL_RAM, MUL_MODE, DECODE_FALSE);
    
    // Run the reference code
    pointwise_montgomery(r_gold, r_gold, MUL_RAM);

    // Compare with the reference code
    int32_t a, b, c, d;
    int32_t ta, tb, tc, td;

    int addr;
    int ret = 0;

    // print_array(r_gold, 32, "gold");
    // print_reshaped_array(&ram, 8, "first 8");

    for (int i = 0; i < N; i += 4)
    {
        // Get golden result
        a = r_gold[i + 0];
        b = r_gold[i + 1];
        c = r_gold[i + 2];
        d = r_gold[i + 3];

        addr = i / 4;
        ta = ram.vec[addr].coeffs[0];
        tb = ram.vec[addr].coeffs[1];
        tc = ram.vec[addr].coeffs[2];
        td = ram.vec[addr].coeffs[3];

        // Comapre with reference code

        // Quick xor, I hate long if-else clause
        ret |= a ^ ta;
        ret |= b ^ tb;
        ret |= c ^ tc;
        ret |= d ^ td;

        if (ret)
        {
            printf("Error at index: %d => %d\n", addr, i);
            printf("%12d | %12d | %12d | %12d\n", a, b, c, d);
            printf("%12d | %12d | %12d | %12d\n", ta, tb, tc, td);
            return 1;
        }
    }
    // printf("==============MUL is Correct!\n\n");
    return 0;
}

#define TESTS 1000

int main()
{
    srand(time(0));
    int32_t r[N], r_gold[N], r_gold_copy[N], r_copy[N];
    int32_t t1, t2;
    int ret = 0;

    for (int k = 0; k < TESTS; k++)
    {
        for (int i = 0; i < N; i++)
        {
            // t1 = i;
            t1 = rand() % Q;
            r[i] = t1;
            r_gold[i] = t1;
            
            t2 = rand() % Q;
            r_gold_copy[i] = t2; 
            r_copy[i] = t2;
        }

        ret |= ntt2x2_NTT(r_gold, r);
        ret |= ntt2x2_MUL(r_gold_copy, r_copy);

        if (ret)
        {
           break;
        }
    }

    if (ret)
    {
        printf("ERROR\n");
    }
    printf("OK\n");

    return ret;
}