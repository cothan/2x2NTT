#include "hls_ntt_mul.h"
#include "hls_const.c"

void unpack(uint64_t *ram, uint16_t index, uint16_t *A, uint16_t *B, uint16_t *C, uint16_t *D)
{
    // Memory style: D|C|B|A
    uint64_t tmp = ram[index];
    *A = tmp & 0xffff;
    *B = (tmp >> 16) & 0xffff;
    *C = (tmp >> 32) & 0xffff;
    *D = (tmp >> 48) & 0xffff;
}

void pack(uint16_t A, uint16_t B, uint16_t C, uint16_t D, uint64_t *ram, uint64_t index)
{
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
    for (int i = length-1; i > 0; i--)
    {
        // printf("SIPO: sipo[%d] <= sipo[%d] (%d <= %d)\n", i, i-1, sipo[i], sipo[i-1]);
        sipo[i] = sipo[i - 1];
    }
}

#if (NEWHOPE_N == 1024)

#define NEWHOPE_LOGN 10

void hls_ntt(uint64_t *ram, uint16_t *ram1, uint16_t *ram2, enum STATE state)
{
    // Define Signal
    bool s = false,
         last_layer = false,
         enable_write = false;
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
        break;
    case MUL:
        s = false;
        layers = 2;
        break;
    case IPSIS:
        s = false;
        layers = 2;
        break;

    case NTT:
        s = true;
        break;
    case INTT:
        s = true;
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
             counter = 0;
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
                printf("|%3d| %4d %4d %4d %4d\n", b,  A, B, C, D);

                // 1st layer

                if (s)
                {
                    idx1 = NTT_idx;
                    idx2 = idx1;
                }
                else
                {
                    idx1 = (b << 2) + 1;
                    idx2 = (b << 2) + 3;
                }

                printf("|%3d| %4d * ram1[%4d] (%4d)\n", b, B, idx1, ram1[idx1]);
                printf("|%3d| %4d * ram1[%4d] (%4d)\n", b, D, idx2, ram1[idx2]);

                B = montgomery_reduce((uint32_t) B * ram1[idx1]);
                D = montgomery_reduce((uint32_t) D * ram1[idx2]);
                B_save = B;
                D_save = D;

                A_hat = A + B;
                B_hat = A - B;
                C_hat = C + D;
                D_hat = C - D;

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
                    idx3 = (b << 2);
                    idx4 = (b << 2) + 2;
                }

                printf("|%3d| %4d * ram2[%4d] (%4d)\n", b ,B_prime, idx3, ram2[idx3]);
                printf("|%3d| %4d * ram2[%4d] (%4d)\n", b ,D_prime, idx4, ram2[idx4]);

                A_prime = s ? A_prime: 0;
                B_prime = montgomery_reduce(((uint32_t) B_prime * ram2[idx3]));
                C_prime = s ? C_prime: 0;
                D_prime = montgomery_reduce(((uint32_t) D_prime * ram2[idx4]));
                
                A_prime = A_prime + B_prime;
                B_prime = A_prime - B_prime;
                C_prime = C_prime + D_prime;
                D_prime = C_prime - D_prime;

                // A_prime = A_prime;
                // C_prime = C_prime;
                B_prime = s ? B_prime : B_save;
                D_prime = s ? D_prime : D_save;

                // SIPO
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
                SIPO(sipo_idx, 4);
                SIPO(sipo_a, 4);
                SIPO(sipo_b, 5);
                SIPO(sipo_c, 6);
                SIPO(sipo_d, 7);

                counter++;
                if (counter >= 4)
                {
                    enable_write = true;
                    counter = 0;
                }

                if (enable_write)
                {
                    addr_writeback = sipo_idx[3];
                    if (s)
                    {
                        switch (counter & 0x3)
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
                            printf("[Error] SIPO counter = %u\n", counter );
                            return;
                        }
                        counter++;
                    }
                    else
                    {
                        out_1 = sipo_a[3];
                        out_2 = sipo_b[4];
                        out_3 = sipo_c[5];
                        out_4 = sipo_d[6];
                    }
                    
                    pack(out_1, out_2, out_3, out_4, ram, addr_writeback);
                }
                printf("\n");
            }
            NTT_idx += omega_idx1;
            NTT_idx3 += omega_idx2;
            NTT_idx4 += omega_idx2;
        }
    }
    // Write back remaning of SIPO
    // TODO: DEBUG 
    /*
    for (uint16_t i = 0; i < 3; i++)
    {
        SIPO(sipo_idx, 4);
        SIPO(sipo_a, 4);
        SIPO(sipo_b, 5);
        SIPO(sipo_c, 6);
        SIPO(sipo_d, 7);

        addr_writeback = sipo_idx[3];
        counter++;
        switch (counter & 0b11)
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
            printf("[Error] SIPO\n");
            return;
        }
        pack(out_1, out_2, out_3, out_4, ram, addr_writeback);
    }
    */

}

void hls_poly_ntt_mul(uint64_t *ram, enum STATE state)
{
    uint16_t *ram1 = NULL, 
             *ram2 = NULL;
    switch (state)
    {
    case PSIS:    
        ram1 = gammas_bitrev_montgomery;
        ram2 = gammas_bitrev_montgomery;
        hls_ntt(ram, ram1, ram2, PSIS);
        break;
    case IPSIS:
        ram1 = gammas_inv_montgomery;
        ram2 = gammas_inv_montgomery;
        hls_ntt(ram, ram1, ram2, IPSIS);
        break;
    case MUL:
        // ram1 = gammas_bitrev_montgomery;
        // ram2 = gammas_bitrev_montgomery;
        printf("[Error] Not Finished\n");
        return;
    
    case INTT:
        ram1 = omegas_inv_bitrev_montgomery;
        ram2 = omegas_inv_bitrev_montgomery;
        hls_ntt(ram, ram1, ram2, INTT);
        break;
    case NTT:
        // ram1 = omegas_inv_bitrev_montgomery;
        // ram2 = omegas_inv_bitrev_montgomery;
        // hls_ntt(ram, ram1, ram2, INTT);
        printf("[Error] Not Finished\n");
        return;

    default:
        printf("[Error] Invalid STATE\n");
        return;
    }
}

#endif