#include "hls_ntt_mul.h"
#include "../ref/newhope_ntt.h"
#include "../ref/newhope_reduce.h"
#include "../ref/newhope_params.h"
#include "../ref/newhope_poly.h"

void unpack(uint64_t *ram, uint16_t index, uint16_t *A, uint16_t *B, uint16_t *C, uint16_t *D)
{
    // Memory style: D|C|B|A
    uint64_t tmp = ram[index];
    *A = tmp & 0xffff;
    *B = (tmp >> 16) & 0xffff;
    *C = (tmp >> 32) & 0xffff;
    *D = (tmp >> 48) & 0xffff;
}

void pack(uint16_t A, uint16_t B, uint16_t C, uint16_t D, uint64_t *ram, uint16_t index, bool debug)
{
    if (debug)
    {
        printf("ram[%d] <= [%5d ,%5d ,%5d ,%5d]\n", index, A, B, C, D);
    }
    // Memory style D|C|B|A
    uint64_t tmp = 0;
    tmp = D;
    tmp = tmp << 16;
    tmp = tmp | C;
    tmp = tmp << 16;
    tmp = tmp | B;
    tmp = tmp << 16;
    tmp = tmp | A;

    ram[index] = tmp;
}

void SIPO(uint16_t *sipo, int length)
{
    // Shift left by one
    for (int i = length - 1; i > 0; i--)
    {
        // printf("SIPO: sipo[%d] <= sipo[%d] (%d <= %d)\n", i, i-1, sipo[i], sipo[i-1]);
        sipo[i] = sipo[i - 1];
    }
}

void printSIPO(uint16_t *sipo, int length, char const *string)
{
    printf("%s: [", string);
    for (int i = 0; i < length; i++)
    {
        printf("%5d,", sipo[i]);
    }
    printf("]\n");
}

void hls_ntt(uint64_t *ram, uint16_t *ram1, uint16_t *ram2, enum STATE state, bool debug)
{
    // Define Signal
    bool s = false,
         last_layer = false,
         enable_write = false;
    uint16_t mem = 0; // Optimize for memory space for NTT/INTT
    // End Signal

    // Define SIPO
    uint16_t sipo_a[4] = {0},
             sipo_b[5] = {0},
             sipo_c[6] = {0},
             sipo_d[7] = {0},
             sipo_idx[4] = {0},
             out_1 = 0, out_2 = 0, out_3 = 0, out_4 = 0,
             addr_writeback = 0;
    // End SIPO

    // Select state to operate
    uint16_t layers = 0;
    switch (state)
    {
    case PSIS:
        s = false;
        layers = 2;
        mem = 1;
        break;
    case MUL:
        s = false;
        layers = 2;
        mem = 1;
        break;
    case IPSIS:
        s = false;
        layers = 2;
        mem = 1;
        break;

    case NTT:
        s = true;
        layers = NEWHOPE_LOGN;
        mem = 0;
        break;
    case INTT:
        s = true;
        layers = NEWHOPE_LOGN;
        mem = 0;
        break;
    default:
        printf("[Error] Invalid STATE\n");
        return;
    }
    // NTT Variables
    uint16_t A = 0,
             B = 0,
             C = 0,
             D = 0,

             B_save = 0,
             D_save = 0,

             A_hat = 0,
             B_hat = 0,
             C_hat = 0,
             D_hat = 0,

             A_mid = 0,
             B_mid = 0,
             C_mid = 0,
             D_mid = 0,

             A_prime = 0,
             B_prime = 0,
             C_prime = 0,
             D_prime = 0;

    uint16_t idx1 = 0,
             idx2 = 0,
             idx3 = 0,
             idx4 = 0,
             counter = 0,
             sipo_counter = 0;
    // End Variables

    for (uint16_t l = 0; l < layers; l += 2)
    {
        uint16_t omega_idx1 = NEWHOPE_N >> (l + 1);
        uint16_t omega_idx2 = NEWHOPE_N >> (l + 2);
        uint16_t NTT_idx = 0, NTT_idx3 = 0;
        uint16_t NTT_idx4 = NEWHOPE_N / 4;
        if ((l + 1) == NEWHOPE_LOGN)
        {
            last_layer = true;
        }

        uint16_t m = 1 << l;
        for (uint16_t j = 0; j < m; j += 1)
        {
            for (uint16_t k = 0; k < NEWHOPE_N / 4; k += m)
            {
                // TODO: rename b to ram_idx
                uint16_t b = k + j;

                // Memory style: D|C|B|A
                unpack(ram, b, &A, &B, &C, &D);
                if (debug)
                {
                    printf("|%3d| %4d %4d %4d %4d\n", b, A, B, C, D);
                }

                // 1st layer

                if (s)
                {
                    idx1 = NTT_idx;
                    idx2 = idx1;
                }
                else
                {
                    idx1 = ((b << 2) + 1) >> mem;
                    idx2 = ((b << 2) + 3) >> mem;
                }

                if (debug)
                {
                    printf("|%3d| %4d * ram1[%4d] (%4d)\n", b, B, idx1, ram1[idx1]);
                    printf("|%3d| %4d * ram1[%4d] (%4d)\n", b, D, idx2, ram1[idx2]);
                }

                B = montgomery_reduce(((uint32_t)B * ram1[idx1])) % NEWHOPE_Q;
                D = montgomery_reduce(((uint32_t)D * ram1[idx2])) % NEWHOPE_Q;
                B_save = B;
                D_save = D;

                A_hat = (A + B) % NEWHOPE_Q;
                B_hat = (A + 3 * NEWHOPE_Q - B) % NEWHOPE_Q;
                C_hat = (C + D) % NEWHOPE_Q;
                D_hat = (C + 3 * NEWHOPE_Q - D) % NEWHOPE_Q;

                // Middle
                // Style A | A^ | A'
                A_mid = s ? A_hat : A;
                B_mid = C_hat;
                C_mid = B_hat;
                D_mid = s ? D_hat : C;

                A_prime = A_mid;
                B_prime = s ? B_mid : A_mid;
                C_prime = C_mid;
                D_prime = D_mid;

                // 2nd Layer

                if (s)
                {
                    idx3 = NTT_idx3;
                    idx4 = NTT_idx4;
                }
                else
                {
                    idx3 = ((b << 2)) >> mem;
                    idx4 = ((b << 2) + 2) >> mem;
                }

                if (debug)
                {
                    printf("|%3d| %4d * ram2[%4d] (%4d)\n", b, B_prime, idx3, ram2[idx3]);
                    printf("|%3d| %4d * ram2[%4d] (%4d)\n", b, D_prime, idx4, ram2[idx4]);
                }

                A_prime = s ? A_prime : 0;
                B_prime = montgomery_reduce(((uint32_t)B_prime * ram2[idx3]));
                C_prime = s ? C_prime : 0;
                D_prime = montgomery_reduce(((uint32_t)D_prime * ram2[idx4]));

                A_prime = (A_prime + B_prime) % NEWHOPE_Q;
                B_prime = (A_prime + 3 * NEWHOPE_Q - B_prime) % NEWHOPE_Q;
                C_prime = (C_prime + D_prime) % NEWHOPE_Q;
                D_prime = (C_prime + 3 * NEWHOPE_Q - D_prime) % NEWHOPE_Q;

                // A_prime = A_prime;
                // C_prime = C_prime;
                B_prime = s ? B_prime : B_save;
                D_prime = s ? D_prime : D_save;

                // SIPO
                // 3 state: SHIFT - WRITE - CHECK
                // SHIFT
                SIPO(sipo_idx, 4);
                SIPO(sipo_a, 4);
                SIPO(sipo_b, 5);
                SIPO(sipo_c, 6);
                SIPO(sipo_d, 7);

                // WRITE
                sipo_idx[0] = b;
                if (last_layer)
                {
                    sipo_a[0] = A_mid;
                    sipo_b[0] = B_mid;
                    sipo_c[0] = C_mid;
                    sipo_d[0] = D_mid;
                }
                else
                {
                    sipo_a[0] = A_prime;
                    sipo_b[0] = B_prime;
                    sipo_c[0] = C_prime;
                    sipo_d[0] = D_prime;
                }

                if (debug)
                {
                    printSIPO(sipo_idx, 4, "IDX");
                    printSIPO(sipo_a, 4, "A");
                    printSIPO(sipo_b, 5, "B");
                    printSIPO(sipo_c, 6, "C");
                    printSIPO(sipo_d, 7, "D");
                }

                // CHECK
                counter++;
                if (counter > 3)
                {
                    enable_write = true;
                }

                if (enable_write)
                {
                    addr_writeback = sipo_idx[3];
                    if (s)
                    {
                        switch (sipo_counter & 0x3)
                        {
                        case 0:
                            // SIPO A writeback
                            out_1 = sipo_a[3];
                            out_2 = sipo_a[2];
                            out_3 = sipo_a[1];
                            out_4 = sipo_a[0];
                            break;
                        case 1:
                            // SIPO B writeback
                            out_1 = sipo_b[4];
                            out_2 = sipo_b[3];
                            out_3 = sipo_b[2];
                            out_4 = sipo_b[1];
                            break;
                        case 2:
                            // SIPO C writeback
                            out_1 = sipo_c[5];
                            out_2 = sipo_c[4];
                            out_3 = sipo_c[3];
                            out_4 = sipo_c[2];
                            break;
                        case 3:
                            // SIPO D writeback
                            out_1 = sipo_d[6];
                            out_2 = sipo_d[5];
                            out_3 = sipo_d[4];
                            out_4 = sipo_d[3];
                            break;
                        default:
                            printf("[Error] SIPO counter = %u\n", sipo_counter);
                            return;
                        }
                        sipo_counter++;
                    }
                    else
                    {
                        out_1 = sipo_a[3];
                        out_2 = sipo_b[3];
                        out_3 = sipo_c[3];
                        out_4 = sipo_d[3];
                    }

                    pack(out_1, out_2, out_3, out_4, ram, addr_writeback, debug);
                }

                if (debug)
                {
                    printf("\n");
                }
            }
            NTT_idx += omega_idx1;
            NTT_idx3 += omega_idx2;
            NTT_idx4 += omega_idx2;
        }
    }
    // Write back remaning of SIPO
    // 2 states: SHIFT - CHECK
    for (uint16_t i = 0; i < 3; i++)
    {
        // SHIFT
        SIPO(sipo_idx, 4);
        SIPO(sipo_a, 4);
        SIPO(sipo_b, 5);
        SIPO(sipo_c, 6);
        SIPO(sipo_d, 7);
        // CHECK
        addr_writeback = sipo_idx[3];
        if (s)
        {
            switch (sipo_counter & 0x3)
            {
            case 0:
                // SIPO A writeback
                out_1 = sipo_a[3];
                out_2 = sipo_a[2];
                out_3 = sipo_a[1];
                out_4 = sipo_a[0];
                break;
            case 1:
                // SIPO B writeback
                out_1 = sipo_b[4];
                out_2 = sipo_b[3];
                out_3 = sipo_b[2];
                out_4 = sipo_b[1];
                break;
            case 2:
                // SIPO C writeback
                out_1 = sipo_c[5];
                out_2 = sipo_c[4];
                out_3 = sipo_c[3];
                out_4 = sipo_c[2];
                break;
            case 3:
                // SIPO D writeback
                out_1 = sipo_d[6];
                out_2 = sipo_d[5];
                out_3 = sipo_d[4];
                out_4 = sipo_d[3];
                break;
            default:
                printf("[Error] SIPO counter = %u\n", sipo_counter);
                return;
            }
            sipo_counter++;
        }
        else
        {
            out_1 = sipo_a[3];
            out_2 = sipo_b[3];
            out_3 = sipo_c[3];
            out_4 = sipo_d[3];
        }
        pack(out_1, out_2, out_3, out_4, ram, addr_writeback, debug);
    }
}

void hls_poly_ntt_mul(uint64_t *ram, uint16_t *ram1, uint16_t *ram2, enum STATE state, bool debug)
{
    switch (state)
    {
    case PSIS:
    case IPSIS:
    case MUL:
        hls_ntt(ram, ram1, ram2, MUL, debug);
        return;

    case INTT:
    case NTT:
        hls_ntt(ram, ram1, ram2, NTT, debug);
        printf("[Error] Create new memory\n");
        break;

    default:
        printf("[Error] Invalid STATE\n");
        return;
    }
}

// TODO: Write transpose
void transpose(uint64_t *ram)
{
    (void)ram;
}