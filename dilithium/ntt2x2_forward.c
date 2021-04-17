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
#define DEPT_B 5
#define DEPT_C 6
#define DEPT_D 7

#define LOG_N 8 // N= 256, => log(N) = 8

enum OP
{
    NTT_MODE,
    INVNTT_MODE,
    MUL_MODE,
    DECODE_TRUE,
    DECODE_FALSE
};

static const int32_t zetas[N] = {
    0, 25847, -2608894, -518909, 237124, -777960, -876248, 466468,
    1826347, 2353451, -359251, -2091905, 3119733, -2884855, 3111497, 2680103,
    2725464, 1024112, -1079900, 3585928, -549488, -1119584, 2619752, -2108549,
    -2118186, -3859737, -1399561, -3277672, 1757237, -19422, 4010497, 280005,
    2706023, 95776, 3077325, 3530437, -1661693, -3592148, -2537516, 3915439,
    -3861115, -3043716, 3574422, -2867647, 3539968, -300467, 2348700, -539299,
    -1699267, -1643818, 3505694, -3821735, 3507263, -2140649, -1600420, 3699596,
    811944, 531354, 954230, 3881043, 3900724, -2556880, 2071892, -2797779,
    -3930395, -1528703, -3677745, -3041255, -1452451, 3475950, 2176455, -1585221,
    -1257611, 1939314, -4083598, -1000202, -3190144, -3157330, -3632928, 126922,
    3412210, -983419, 2147896, 2715295, -2967645, -3693493, -411027, -2477047,
    -671102, -1228525, -22981, -1308169, -381987, 1349076, 1852771, -1430430,
    -3343383, 264944, 508951, 3097992, 44288, -1100098, 904516, 3958618,
    -3724342, -8578, 1653064, -3249728, 2389356, -210977, 759969, -1316856,
    189548, -3553272, 3159746, -1851402, -2409325, -177440, 1315589, 1341330,
    1285669, -1584928, -812732, -1439742, -3019102, -3881060, -3628969, 3839961,
    2091667, 3407706, 2316500, 3817976, -3342478, 2244091, -2446433, -3562462,
    266997, 2434439, -1235728, 3513181, -3520352, -3759364, -1197226, -3193378,
    900702, 1859098, 909542, 819034, 495491, -1613174, -43260, -522500,
    -655327, -3122442, 2031748, 3207046, -3556995, -525098, -768622, -3595838,
    342297, 286988, -2437823, 4108315, 3437287, -3342277, 1735879, 203044,
    2842341, 2691481, -2590150, 1265009, 4055324, 1247620, 2486353, 1595974,
    -3767016, 1250494, 2635921, -3548272, -2994039, 1869119, 1903435, -1050970,
    -1333058, 1237275, -3318210, -1430225, -451100, 1312455, 3306115, -1962642,
    -1279661, 1917081, -2546312, -1374803, 1500165, 777191, 2235880, 3406031,
    -542412, -2831860, -1671176, -1846953, -2584293, -3724270, 594136, -3776993,
    -2013608, 2432395, 2454455, -164721, 1957272, 3369112, 185531, -1207385,
    -3183426, 162844, 1616392, 3014001, 810149, 1652634, -3694233, -1799107,
    -3038916, 3523897, 3866901, 269760, 2213111, -975884, 1717735, 472078,
    -426683, 1723600, -1803090, 1910376, -1667432, -1104333, -260646, -3833893,
    -2939036, -2235985, -420899, -2286327, 183443, -976891, 1612842, -3545687,
    -554416, 3919660, -48306, -1362209, 3937738, 1400424, -846154, 1976782};

// This function will output an element when insert 1 element
int32_t FIFO(const int dept, int32_t *fifo, const int32_t value)
{
    int32_t out = fifo[dept - 1];
    for (int i = dept - 1; i > -1; i--)
    {
        fifo[i] = fifo[i - 1];
    }
    // TODO Remove this
    fifo[0] = value;

    return out;
}

int32_t PISO(const int dept, int32_t *fifo, const int32_t *line)
{
    int32_t out = fifo[dept - 1];
    for (int i = dept - 1; i > 3; i--)
    {
        fifo[i] = fifo[i - 1];
    }
    for (int i = 0; i < 4; i++)
    {
        printf("%d--\n", line[i]);
    }
    fifo[3] = line[0];
    fifo[2] = line[1];
    fifo[1] = line[2];
    fifo[0] = line[3];

    return out;
}

void swap(int32_t *a, int32_t *b)
{
    int32_t tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

void butterfly(int mode, int32_t *bj, int32_t *bjlen, const int32_t zeta, int32_t aj, int32_t ajlen)
{
    static int32_t aj1, ajlen1;
    static int32_t aj2, ajlen2;
    static int32_t aj3, ajlen3;
    static int32_t aj4, ajlen4;
    
    aj1 = aj;
    ajlen1 = ajlen;
    
    if (mode == INVNTT_MODE)
    {
        /* This code copied from butterfly unit in Dilithium Inverse NTT
        t = a[j];
        a[j] = t + a[j + len];
        a[j + len] = t - a[j + len];
        a[j + len] = montgomery_reduce((int64_t)zeta * a[j + len]); */
        // INV_NTT
        aj2 = aj1 + ajlen1;
        ajlen2 = aj1 - ajlen1;
    }
    else
    {
        aj2 = aj1;
        ajlen2 = ajlen1;
    }

    // MUL
    // t = ajlen = montgomery_reduce((int64_t)zeta * ajlen);
    ajlen3 = ((int64_t)zeta * ajlen2) % Q;
    aj3 = aj2;
    
    if (mode == NTT_MODE)
    {
        /* This code copied from butterfly unit in Dilithium Forward NTT
        t = montgomery_reduce((int64_t)zeta * a[j + len]);
        a[j + len] = a[j] - t;
        a[j] = a[j] + t; */
        // NTT
        ajlen4 = aj3 - ajlen3;
        aj4 = aj3 + ajlen3;
    }
    else
    {
        ajlen4 = ajlen3;
        aj4 = aj3;
    }

    if (mode == INVNTT_MODE)
    {
      // if (aj & 1)
      // {
      //   aj = (aj >> 1) + (Q + 1) / 2;
      // }
      // else
      // {
      //   aj = (aj >> 1);
      // }

      // if (ajlen & 1)
      // {
      //   ajlen = (ajlen >> 1) + (Q + 1) / 2;
      // }
      // else
      // {
      //   ajlen = (ajlen >> 1);
      // }
    }

    *bj = aj4;
    *bjlen = ajlen4;
}

void read_fifo(int32_t *fa, int32_t *fb,
               int32_t *fc, int32_t *fd,
               const int count, const int mode,
               const int32_t fifo_a[DEPT_A],
               const int32_t fifo_b[DEPT_B],
               const int32_t fifo_c[DEPT_C],
               const int32_t fifo_d[DEPT_D])
{
    static int32_t ta, tb, tc, td;
    if (mode == INVNTT_MODE)
    {
        // Serial in Parallel out
        switch (count & 0b11)
        {
        case 0:
            ta = fifo_a[DEPT_A - 1];
            tb = fifo_a[DEPT_A - 2];
            tc = fifo_a[DEPT_A - 3];
            td = fifo_a[DEPT_A - 4];
            break;

        case 2:
            ta = fifo_b[DEPT_B - 1];
            tb = fifo_b[DEPT_B - 2];
            tc = fifo_b[DEPT_B - 3];
            td = fifo_b[DEPT_B - 4];
            break;
        case 1:
            ta = fifo_c[DEPT_C - 1];
            tb = fifo_c[DEPT_C - 2];
            tc = fifo_c[DEPT_C - 3];
            td = fifo_c[DEPT_C - 4];
            break;
        case 3:
            ta = fifo_d[DEPT_D - 1];
            tb = fifo_d[DEPT_D - 2];
            tc = fifo_d[DEPT_D - 3];
            td = fifo_d[DEPT_D - 4];
            break;

        default:
            printf("Error, suspect overflow\n");
            break;
        }
    }
    else
    {
        // MUL Mode
        ta = fifo_a[DEPT_A - 1];
        tb = fifo_b[DEPT_B - 3];
        tc = fifo_c[DEPT_C - 2];
        td = fifo_d[DEPT_D - 4];
    }

    *fa = ta;
    *fb = tb;
    *fc = tc;
    *fd = td;
}

void read_ram(int32_t *a, int32_t *b, int32_t *c, int32_t *d,
              const bram *ram, const int ram_i)
{
    *a = ram->vec[ram_i].coeffs[0];
    *b = ram->vec[ram_i].coeffs[1];
    *c = ram->vec[ram_i].coeffs[2];
    *d = ram->vec[ram_i].coeffs[3];
}

void write_ram(bram *ram, const int ram_i,
               const int32_t a, const int32_t b,
               const int32_t c, const int32_t d)
{
    ram->vec[ram_i].coeffs[0] = a;
    ram->vec[ram_i].coeffs[1] = b;
    ram->vec[ram_i].coeffs[2] = c;
    ram->vec[ram_i].coeffs[3] = d;
}


int addr_encoder(int addr_in)
{
    return -1;
}

/*
 *
 */
void write_fifo(int32_t *fifo_a, int32_t *fifo_b, int32_t *fifo_c, int32_t *fifo_d,
                const int count, const bram *ram, const int ram_i)
{

    int32_t a, b, c, d;
    a = ram->vec[ram_i].coeffs[0];
    b = ram->vec[ram_i].coeffs[1];
    c = ram->vec[ram_i].coeffs[2];
    d = ram->vec[ram_i].coeffs[3];
    switch (count & 0b11)
    {
    case 0:
        // FIFO_D
        fifo_d[3] = a;
        fifo_d[2] = b;
        fifo_d[1] = c;
        fifo_d[0] = d;
        break;

    case 1:
        // FIFO_C
        fifo_c[3] = a;
        fifo_c[2] = b;
        fifo_c[1] = c;
        fifo_c[0] = d;
        break;

    case 2:
        // FIFO_B
        fifo_b[3] = a;
        fifo_b[2] = b;
        fifo_b[1] = c;
        fifo_b[0] = d;
        break;

    default:
        // FIFO_A
        fifo_a[3] = a;
        fifo_a[2] = b;
        fifo_a[1] = c;
        fifo_a[0] = d;
        break;
    }
}

/* Pop at the end if FIFO
 * 
 */
void move_fifo(int32_t *fa, int32_t *fb, int32_t *fc,
               int32_t *fd, int32_t *fi, int32_t *fw,
               int *fifo_a, int *fifo_b,
               int *fifo_c, int *fifo_d,
               int *fifo_i, const int ram_i,
               int *fifo_w, const int32_t w)
{
    *fa = FIFO(DEPT_D, fifo_d, 0);
    *fb = FIFO(DEPT_C, fifo_c, 0);
    *fc = FIFO(DEPT_B, fifo_b, 0);
    *fd = FIFO(DEPT_A, fifo_a, 0);
    *fi = FIFO(DEPT_I, fifo_i, ram_i);
    *fw = FIFO(DEPT_I, fifo_w, w);
}

void forward_ntt2x2(bram *ram)
{
    int32_t fifo_i[DEPT_I] = {0},
            fifo_a[DEPT_A] = {0},
            fifo_b[DEPT_B] = {0},
            fifo_c[DEPT_C] = {0},
            fifo_d[DEPT_D] = {0},
            fifo_w[DEPT_I] = {0};
    int32_t a, b, c, d;
    int32_t fa, fb, fc, fd, fi, fw;
    int32_t save_a, save_b, save_c, save_d;

    int i1, i2, i3, i4;
    int ram_i, bound, addr;

    int count = 0;
    int write = 0;
    int m_counter = 0;

    const int w_m1 = 2;
    const int w_m2 = 1;

    int32_t w, w1, w2, w3, w4;

    // --------------------
    int s_array[4] = {4, 2, 0, 4};
    int start = 0;
    a = -1;
    b = -1;
    c = -1;
    d = -1;
    // --------------------

    for (int ks = 0; ks < 1; ++ks)
    {
        int s = s_array[ks];

        for (int i = 0; i < 1 << s; i++)
        {
            i1 = 1 << ks; 
            i2 = 1 << ks;
            i4 = i3 = (1 << (ks+1));
            
            i4 = (1 << (ks+1)) + 1;
            for (int j = 0; j < N / 4; j += 1 << s)
            {

                // Load then store to FIFO
                addr = i + j;

                ram_i = addr;

                move_fifo(&fa, &fb, &fc, &fd, &fi, &fw,
                            fifo_a, fifo_b, fifo_c, fifo_d, 
                            fifo_i, ram_i, fifo_w, w);
                write_fifo(fifo_a, fifo_b, fifo_c, fifo_d, count, ram, ram_i);

                if (count == DEPT_I+1)
                {
                    // FIFO_D is full
                    start = 1;
                    // count = 0;
                }
                
                    count++;
                

                // Compute

                if (start)
                {
                    printf("%d, %d, %d, %d\n", fa, fb, fc, fd);
                    printf("--------\n");
                }

                // printf("w: %2d - %2d - %2d - %2d\n", i1, i2, i3, i4);

                // print_array(fifo_d, DEPT_D, "FIFO_A");
                // print_array(fifo_c, DEPT_C, "FIFO_B");
                // print_array(fifo_b, DEPT_B, "FIFO_C");
                // print_array(fifo_a, DEPT_A, "FIFO_D");
                // printf("--------%d\n", ram_i);

                // printf("=====================\n\n");
            }
            i1 += 1; 
            i2 += 1; 
            i3 += 1; 
            i4 += 1; 



        }



    }

    for (int i = 0; i < DEPT_I+1; i++)
    {
        printf("=====END\n");
        move_fifo(&fa, &fb, &fc, &fd, &fi, &fw,
                    fifo_a, fifo_b, fifo_c, fifo_d, 
                    fifo_i, ram_i, fifo_w, w);
        printf("%d, %d, %d, %d\n", fa, fb, fc, fd);


        // print_array(fifo_d, DEPT_D, "FIFO_A");
        // print_array(fifo_c, DEPT_C, "FIFO_B");
        // print_array(fifo_b, DEPT_B, "FIFO_C");
        // print_array(fifo_a, DEPT_A, "FIFO_D");
        printf("--------%d\n", ram_i);
    }
}

int ntt2x2_NTT(int32_t r[N])
{
    bram ram;

    reshape(&ram, r);

    forward_ntt2x2(&ram);

    return 1;
}

#define TESTS 1

int main()
{
    // srand(time(0));
    srand(0);
    int32_t r[N], r_gold[N], r_gold_copy[N], r_copy[N];
    int32_t t1, t2;
    int ret = 0;

    for (int k = 0; k < TESTS; k++)
    {
        for (int i = 0; i < N; i++)
        {
            t1 = i;
            // t1 = rand() % Q;
            r[i] = t1;
            r_gold[i] = t1;

            // t2 = rand() % Q;
            // r_gold_copy[i] = t2;
            // r_copy[i] = t2;
        }

        ret |= ntt2x2_NTT(r);

        if (ret)
        {
            break;
        }
    }

    if (ret)
    {
        printf("ERROR\n");
    }
    else
    {
        printf("OK\n");
    }

    return ret;
}
