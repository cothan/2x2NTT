#include "../ref/newhope_ntt.h"
#include "../ref/newhope_poly.h"
#include "../ref/newhope_precomp.c"
#include "../ref/newhope_reduce.h"
#include <stdlib.h>
#include <stdbool.h>

#if (NEWHOPE_N == 512)

uint16_t dit_gammas[NEWHOPE_N] = {1, 10968, 3, 8326, 9, 400, 27, 1200, 81, 3600, 243, 10800, 729, 7822, 2187, 11177, 6561, 8953, 7394, 2281, 9893, 6843, 5101, 8240, 3014, 142, 9042, 426, 2548, 1278, 7644, 3834, 10643, 11502, 7351, 9928, 9764, 5206, 4714, 3329, 1853, 9987, 5559, 5383, 4388, 3860, 875, 11580, 2625, 10162, 7875, 5908, 11336, 5435, 9430, 4016, 3712, 12048, 11136, 11566, 8830, 10120, 1912, 5782, 5736, 5057, 4919, 2882, 2468, 8646, 7404, 1360, 9923, 4080, 5191, 12240, 3284, 12142, 9852, 11848, 4978, 10966, 2645, 8320, 7935, 382, 11516, 1146, 9970, 3438, 5332, 10314, 3707, 6364, 11121, 6803, 8785, 8120, 1777, 12071, 5331, 11635, 3704, 10327, 11112, 6403, 8758, 6920, 1696, 8471, 5088, 835, 2975, 2505, 8925, 7515, 2197, 10256, 6591, 6190, 7484, 6281, 10163, 6554, 5911, 7373, 5444, 9830, 4043, 4912, 12129, 2447, 11809, 7341, 10849, 9734, 7969, 4624, 11618, 1583, 10276, 4749, 6250, 1958, 6461, 5874, 7094, 5333, 8993, 3710, 2401, 11130, 7203, 8812, 9320, 1858, 3382, 5574, 10146, 4433, 5860, 1010, 5291, 3030, 3584, 9090, 10752, 2692, 7678, 8076, 10745, 11939, 7657, 11239, 10682, 9139, 7468, 2839, 10115, 8517, 5767, 973, 5012, 2919, 2747, 8757, 8241, 1693, 145, 5079, 435, 2948, 1305, 8844, 3915, 1954, 11745, 5862, 10657, 5297, 7393, 3602, 9890, 10806, 5092, 7840, 2987, 11231, 8961, 9115, 2305, 2767, 6915, 8301, 8456, 325, 790, 975, 2370, 2925, 7110, 8775, 9041, 1747, 2545, 5241, 7635, 3434, 10616, 10302, 7270, 6328, 9521, 6695, 3985, 7796, 11955, 11099, 11287, 8719, 9283, 1579, 3271, 4737, 9813, 1922, 4861, 5766, 2294, 5009, 6882, 2738, 8357, 8214, 493, 64, 1479, 192, 4437, 576, 1022, 1728, 3066, 5184, 9198, 3263, 3016, 9789, 9048, 4789, 2566, 2078, 7698, 6234, 10805, 6413, 7837, 6950, 11222, 8561, 9088, 1105, 2686, 3315, 8058, 9945, 11885, 5257, 11077, 3482, 8653, 10446, 1381, 6760, 4143, 7991, 140, 11684, 420, 10474, 1260, 6844, 3780, 8243, 11340, 151, 9442, 453, 3748, 1359, 11244, 4077, 9154, 12231, 2884, 12115, 8652, 11767, 1378, 10723, 4134, 7591, 113, 10484, 339, 6874, 1017, 8333, 3051, 421, 9153, 1263, 2881, 3789, 8643, 11367, 1351, 9523, 4053, 3991, 12159, 11973, 11899, 11341, 11119, 9445, 8779, 3757, 1759, 11271, 5277, 9235, 3542, 3127, 10626, 9381, 7300, 3565, 9611, 10695, 4255, 7507, 476, 10232, 1428, 6118, 4284, 6065, 563, 5906, 1689, 5429, 5067, 3998, 2912, 11994, 8736, 11404, 1630, 9634, 4890, 4324, 2381, 683, 7143, 2049, 9140, 6147, 2842, 6152, 8526, 6167, 1000, 6212, 3000, 6347, 9000, 6752, 2422, 7967, 7266, 11612, 9509, 10258, 3949, 6196, 11847, 6299, 10963, 6608, 8311, 7535, 355, 10316, 1065, 6370, 3195, 6821, 9585, 8174, 4177, 12233, 242, 12121, 726, 11785, 2178, 10777, 6534, 7753, 7313, 10970, 9650, 8332, 4372, 418, 827, 1254, 2481, 3762, 7443, 11286, 10040, 9280, 5542, 3262, 4337, 9786, 722, 4780, 2166, 2051, 6498, 6153, 7205, 6170, 9326, 6221, 3400, 6374, 10200, 6833, 6022, 8210, 5777, 52, 5042, 156, 2837, 468, 8511, 1404, 955, 4212, 2865, 347, 8595, 1041, 1207, 3123, 3621, 9369, 10863, 3529, 8011, 10587, 11744, 7183, 10654, 9260, 7384, 3202, 9863, 9606, 5011, 4240, 2744, 431, 8232, 1293, 118, 3879, 354, 11637, 1062, 10333, 3186, 6421, 9558, 6974, 4096, 8633};

uint16_t dif_gammas_bitrev[NEWHOPE_N] = {1, 1479, 4043, 7143, 5736, 4134, 1305, 722, 10643, 11077, 5860, 3195, 8785, 3542, 2545, 3621, 6561, 7698, 6461, 7266, 4978, 1351, 8961, 5777, 2625, 11340, 7468, 9650, 2975, 563, 9283, 2744, 81, 9198, 7969, 1000, 9923, 3051, 7393, 9326, 1853, 140, 7678, 726, 11112, 4255, 9521, 10654, 3014, 9088, 7203, 10963, 9970, 11119, 790, 955, 3712, 9154, 2747, 7443, 7484, 8736, 2294, 1062, 9, 1022, 11809, 2842, 2468, 339, 11745, 6498, 9764, 1381, 3584, 4177, 5331, 7300, 10616, 8011, 9893, 7837, 8993, 3949, 7935, 12159, 6915, 2837, 11336, 3748, 5767, 827, 2197, 5067, 9813, 118, 729, 9048, 10276, 9000, 3284, 2881, 5092, 10200, 4388, 1260, 7657, 6534, 1696, 1428, 11955, 9863, 2548, 8058, 3382, 355, 3707, 1759, 7110, 8595, 8830, 8652, 145, 5542, 5911, 4890, 8357, 9558, 3, 4437, 12129, 9140, 4919, 113, 3915, 2166, 7351, 8653, 5291, 9585, 1777, 10626, 7635, 10863, 7394, 10805, 7094, 9509, 2645, 4053, 2305, 5042, 7875, 9442, 10115, 4372, 8925, 1689, 3271, 8232, 243, 3016, 11618, 3000, 5191, 9153, 9890, 3400, 5559, 420, 10745, 2178, 8758, 476, 3985, 7384, 9042, 2686, 9320, 8311, 5332, 8779, 2370, 2865, 11136, 2884, 8241, 10040, 10163, 1630, 6882, 3186, 27, 3066, 10849, 8526, 7404, 1017, 10657, 7205, 4714, 4143, 10752, 242, 3704, 9611, 7270, 11744, 5101, 11222, 2401, 11847, 11516, 11899, 8456, 8511, 9430, 11244, 5012, 2481, 6591, 2912, 4861, 354, 2187, 2566, 6250, 2422, 9852, 8643, 2987, 6022, 875, 3780, 10682, 7313, 5088, 4284, 11287, 5011, 7644, 11885, 10146, 1065, 11121, 5277, 9041, 1207, 1912, 1378, 435, 4337, 5444, 2381, 493, 4096, 10968, 192, 4912, 2049, 5057, 7591, 8844, 4780, 11502, 3482, 1010, 6821, 8120, 3127, 5241, 9369, 8953, 6234, 5874, 11612, 10966, 9523, 9115, 52, 10162, 151, 2839, 8332, 2505, 5906, 1579, 431, 3600, 3263, 4624, 6212, 4080, 421, 3602, 6221, 9987, 11684, 8076, 11785, 6403, 7507, 6695, 9260, 142, 1105, 8812, 6608, 3438, 9445, 975, 4212, 12048, 12231, 8757, 11286, 6281, 11404, 5009, 10333, 400, 1728, 7341, 6152, 8646, 6874, 5862, 6153, 5206, 6760, 9090, 12233, 11635, 3565, 10302, 10587, 6843, 6950, 3710, 6196, 382, 11973, 8301, 468, 5435, 1359, 973, 1254, 10256, 3998, 1922, 3879, 7822, 4789, 4749, 6752, 12142, 3789, 7840, 6833, 3860, 6844, 11239, 7753, 8471, 6118, 11099, 9606, 1278, 9945, 5574, 10316, 6364, 11271, 8775, 1041, 10120, 11767, 5079, 3262, 7373, 4324, 8214, 6974, 8326, 576, 2447, 6147, 2882, 10484, 1954, 2051, 9928, 10446, 3030, 8174, 12071, 9381, 3434, 3529, 2281, 6413, 5333, 10258, 8320, 3991, 2767, 156, 5908, 453, 8517, 418, 7515, 5429, 4737, 1293, 10800, 9789, 1583, 6347, 12240, 1263, 10806, 6374, 5383, 10474, 11939, 10777, 6920, 10232, 7796, 3202, 426, 3315, 1858, 7535, 10314, 3757, 2925, 347, 11566, 12115, 1693, 9280, 6554, 9634, 2738, 6421, 1200, 5184, 9734, 6167, 1360, 8333, 5297, 6170, 3329, 7991, 2692, 12121, 10327, 10695, 6328, 7183, 8240, 8561, 11130, 6299, 1146, 11341, 325, 1404, 4016, 4077, 2919, 3762, 6190, 11994, 5766, 11637, 11177, 2078, 1958, 7967, 11848, 11367, 11231, 8210, 11580, 8243, 9139, 10970, 835, 6065, 8719, 4240, 3834, 5257, 4433, 6370, 6803, 9235, 1747, 3123, 5782, 10723, 2948, 9786, 9830, 683, 64, 8633};

uint16_t dif_omegas_bitrev[NEWHOPE_N/2] = {1, 1479, 4043, 7143, 5736, 4134, 1305, 722, 10643, 11077, 5860, 3195, 8785, 3542, 2545, 3621, 6561, 7698, 6461, 7266, 4978, 1351, 8961, 5777, 2625, 11340, 7468, 9650, 2975, 563, 9283, 2744, 81, 9198, 7969, 1000, 9923, 3051, 7393, 9326, 1853, 140, 7678, 726, 11112, 4255, 9521, 10654, 3014, 9088, 7203, 10963, 9970, 11119, 790, 955, 3712, 9154, 2747, 7443, 7484, 8736, 2294, 1062, 9, 1022, 11809, 2842, 2468, 339, 11745, 6498, 9764, 1381, 3584, 4177, 5331, 7300, 10616, 8011, 9893, 7837, 8993, 3949, 7935, 12159, 6915, 2837, 11336, 3748, 5767, 827, 2197, 5067, 9813, 118, 729, 9048, 10276, 9000, 3284, 2881, 5092, 10200, 4388, 1260, 7657, 6534, 1696, 1428, 11955, 9863, 2548, 8058, 3382, 355, 3707, 1759, 7110, 8595, 8830, 8652, 145, 5542, 5911, 4890, 8357, 9558, 3, 4437, 12129, 9140, 4919, 113, 3915, 2166, 7351, 8653, 5291, 9585, 1777, 10626, 7635, 10863, 7394, 10805, 7094, 9509, 2645, 4053, 2305, 5042, 7875, 9442, 10115, 4372, 8925, 1689, 3271, 8232, 243, 3016, 11618, 3000, 5191, 9153, 9890, 3400, 5559, 420, 10745, 2178, 8758, 476, 3985, 7384, 9042, 2686, 9320, 8311, 5332, 8779, 2370, 2865, 11136, 2884, 8241, 10040, 10163, 1630, 6882, 3186, 27, 3066, 10849, 8526, 7404, 1017, 10657, 7205, 4714, 4143, 10752, 242, 3704, 9611, 7270, 11744, 5101, 11222, 2401, 11847, 11516, 11899, 8456, 8511, 9430, 11244, 5012, 2481, 6591, 2912, 4861, 354, 2187, 2566, 6250, 2422, 9852, 8643, 2987, 6022, 875, 3780, 10682, 7313, 5088, 4284, 11287, 5011, 7644, 11885, 10146, 1065, 11121, 5277, 9041, 1207, 1912, 1378, 435, 4337, 5444, 2381, 493, 4096};

uint16_t dit_omegas[NEWHOPE_N/2] = {1, 9, 81, 729, 6561, 9893, 3014, 2548, 10643, 9764, 1853, 4388, 2625, 11336, 3712, 8830, 5736, 2468, 9923, 3284, 4978, 7935, 9970, 3707, 8785, 5331, 11112, 1696, 2975, 2197, 7484, 5911, 4043, 11809, 7969, 10276, 6461, 8993, 7203, 3382, 5860, 3584, 7678, 7657, 7468, 5767, 2747, 145, 1305, 11745, 7393, 5092, 8961, 6915, 790, 7110, 2545, 10616, 9521, 11955, 9283, 9813, 2294, 8357, 1479, 1022, 9198, 9048, 7698, 7837, 9088, 8058, 11077, 1381, 140, 1260, 11340, 3748, 9154, 8652, 4134, 339, 3051, 2881, 1351, 12159, 11119, 1759, 3542, 7300, 4255, 1428, 563, 5067, 8736, 4890, 7143, 2842, 1000, 9000, 7266, 3949, 10963, 355, 3195, 4177, 726, 6534, 9650, 827, 7443, 5542, 722, 6498, 9326, 10200, 5777, 2837, 955, 8595, 3621, 8011, 10654, 9863, 2744, 118, 1062, 9558, 12288, 12280, 12208, 11560, 5728, 2396, 9275, 9741, 1646, 2525, 10436, 7901, 9664, 953, 8577, 3459, 6553, 9821, 2366, 9005, 7311, 4354, 2319, 8582, 3504, 6958, 1177, 10593, 9314, 10092, 4805, 6378, 8246, 480, 4320, 2013, 5828, 3296, 5086, 8907, 6429, 8705, 4611, 4632, 4821, 6522, 9542, 12144, 10984, 544, 4896, 7197, 3328, 5374, 11499, 5179, 9744, 1673, 2768, 334, 3006, 2476, 9995, 3932, 10810, 11267, 3091, 3241, 4591, 4452, 3201, 4231, 1212, 10908, 12149, 11029, 949, 8541, 3135, 3637, 8155, 11950, 9238, 9408, 10938, 130, 1170, 10530, 8747, 4989, 8034, 10861, 11726, 7222, 3553, 7399, 5146, 9447, 11289, 3289, 5023, 8340, 1326, 11934, 9094, 8112, 11563, 5755, 2639, 11462, 4846, 6747, 11567, 5791, 2963, 2089, 6512, 9452, 11334, 3694, 8668, 4278, 1635, 2426, 9545, 12171, 11227, 2731};


#elif (NEWHOPE_N == 1024)

#else
#error "Either 512 or 1024"
#endif

// NTT DIT RN BO->NO
void ntt_dit(uint16_t *a, const uint16_t *omega)
{
    uint16_t PairsInGroup = NEWHOPE_N / 2;

    uint16_t Distance = 1;
    for (uint16_t NumOfGroups = 1; NumOfGroups < NEWHOPE_N; NumOfGroups = NumOfGroups * 2)
    {
        uint16_t GapToNextPair = 2 * NumOfGroups;
        uint16_t GapToLastPair = GapToNextPair * (PairsInGroup - 1);
        for (uint16_t k = 0; k < NumOfGroups; k++)
        {
            uint16_t Jlast = k + GapToLastPair;
            uint16_t jTwiddle = k * PairsInGroup;
            uint16_t W = omega[jTwiddle];
            for (uint16_t j = k; j <= Jlast; j += GapToNextPair)
            {
                uint16_t temp = montgomery_reduce((uint32_t) W * a[j + Distance]);
                a[j + Distance] = (a[j] - temp) % NEWHOPE_Q;
                a[j] = (a[j] + temp) % NEWHOPE_Q;
            }
        }
        PairsInGroup = PairsInGroup / 2;
        Distance = Distance * 2;
    }
}

// NTT DIF RN BO->NO
void ntt_dif(uint16_t *a, const uint16_t *omega)
{
    uint16_t NumberOfProblems = 1;

    uint16_t Distance = 1;
    for (uint16_t ProblemSize = NEWHOPE_N; ProblemSize > 1; ProblemSize = ProblemSize / 2)
    {
        for (uint16_t JFirst = 0; JFirst < NumberOfProblems; JFirst++)
        {
            uint16_t Jtwiddle = 0;
            for (uint16_t J = JFirst; J < NEWHOPE_N - 1; J += 2 * NumberOfProblems)
            {
                uint16_t W = omega[Jtwiddle++];
                uint16_t temp = a[J];
                a[J] = (temp + a[J + Distance]) % NEWHOPE_Q;
                a[J + Distance] = montgomery_reduce(((uint32_t)temp + 3 * NEWHOPE_Q - a[J + Distance]) * W);
            }
        }
        NumberOfProblems = NumberOfProblems * 2;
        Distance = Distance * 2;
    }
}

// Copy of NTT DIT RN BO->NO, with full reduction
void ntt_dit_copy_full_reduction(uint16_t *a, const uint16_t *omega)
{
    uint16_t PairsInGroup = NEWHOPE_N / 2;
    uint16_t count = 0;
    uint16_t Distance = 1;
    for (uint16_t NumOfGroups = 1; NumOfGroups < NEWHOPE_N; NumOfGroups = NumOfGroups * 2)
    {
        uint16_t GapToNextPair = 2 * NumOfGroups;
        uint16_t GapToLastPair = GapToNextPair * (PairsInGroup-1);
        for (uint16_t k = 0; k < NumOfGroups; k++)
        {
            uint16_t JLast = k + GapToLastPair;
            uint16_t jTwiddle = k * PairsInGroup;
            uint32_t W = omega[jTwiddle];
            for (uint16_t j = k; j <= JLast; j += GapToNextPair)
            {
                uint32_t temp = (W * a[j + Distance]) % NEWHOPE_Q;
                a[j + Distance] = (a[j] - temp) % NEWHOPE_Q;
                a[j] = (a[j] + temp) % NEWHOPE_Q;
                count++;
            }
        }
        PairsInGroup = PairsInGroup / 2;
        Distance = Distance * 2;
        // printf("-------------\n");
    }
    printf("DIT count: %d\n", count);
}


// Copy of NTT DIF RN BO->NO, with full reduction 
void ntt_dif_copy(uint16_t *a, const uint16_t *omega)
{
    uint16_t NumberOfProblems = 1;
    uint16_t count = 0;
    uint16_t Distance = 1;
    for (uint16_t ProblemSize = NEWHOPE_N; ProblemSize > 1; ProblemSize = ProblemSize / 2)
    {
        for (uint16_t JFirst = 0; JFirst < NumberOfProblems; JFirst++)
        {
            uint16_t Jtwiddle = 0;
            for (uint16_t J = JFirst; J < NEWHOPE_N - 1; J += 2 * NumberOfProblems)
            {
                uint32_t W = omega[Jtwiddle++];
                uint16_t temp = a[J];
                a[J] = (temp + a[J + Distance]) % NEWHOPE_Q;
                a[J + Distance] = montgomery_reduce( W*(temp + 3*NEWHOPE_Q - a[J + Distance]) );
                count++;
            }
        }
        NumberOfProblems = NumberOfProblems * 2;
        Distance = Distance * 2;
    }
    printf("DIF count: %d\n", count);
}

// Copy of NTT DIF RN BO->NO, with full reduction 
void ntt_dif_copy_full_reduction(uint16_t *a, const uint16_t *omega)
{
    uint16_t NumberOfProblems = 1;
    uint16_t count = 0;
    uint16_t Distance = 1;
    for (uint16_t ProblemSize = NEWHOPE_N; ProblemSize > 1; ProblemSize = ProblemSize / 2)
    {
        for (uint16_t JFirst = 0; JFirst < NumberOfProblems; JFirst++)
        {
            uint16_t Jtwiddle = 0;
            for (uint16_t J = JFirst; J < NEWHOPE_N - 1; J += 2 * NumberOfProblems)
            {
                uint32_t W = omega[Jtwiddle++];
                uint16_t temp = a[J];
                a[J] = (temp + a[J + Distance]) % NEWHOPE_Q;
                a[J + Distance] = ( W* ( (temp + NEWHOPE_Q - a[J + Distance]) % NEWHOPE_Q) ) % NEWHOPE_Q;
                count++;
            }
        }
        NumberOfProblems = NumberOfProblems * 2;
        Distance = Distance * 2;
    }
    printf("DIF count: %d\n", count);
}

int compare(poly *r, poly *r_test, const char *string)
{
    printf("%s:\n", string);
    uint16_t a[4];
    uint16_t b[4];
    bool error = false;
    for (uint16_t i = 0; i < NEWHOPE_N; i += 4)
    {
        a[0] = r->coeffs[i];
        a[1] = r->coeffs[i + 1];
        a[2] = r->coeffs[i + 2];
        a[3] = r->coeffs[i + 3];

        b[0] = r_test->coeffs[i];
        b[1] = r_test->coeffs[i + 1];
        b[2] = r_test->coeffs[i + 2];
        b[3] = r_test->coeffs[i + 3];

        for (uint16_t j = 0; j < 4; j++)
        {
            if (a[j] != b[j])
            {
                printf("[%u] %u != %u\n", i + j, a[j], b[j]);
                error = true;
            }
        }
        if (error)
        {
            printf("Result: Failed!\n");
            exit(1);
        }
    }
    printf("Result: Success!\n---------------------\n");
    return 0;
}

void printArray(uint16_t *sipo, int length, char const *string)
{
    printf("%s: [", string);
    for (int i = 0; i < length; i++)
    {
        printf("%5d,", sipo[i]);
    }
    printf("]\n");
}

void full_reduce(poly *a)
{
    for (uint16_t i = 0; i < NEWHOPE_N; i++)
    {
        a->coeffs[i] = a->coeffs[i] % NEWHOPE_Q;
    }
}

int main()
{
    poly r_gold,
         r_test_dif,
         r_test_dif_copy,
         r_test_dit_copy;
    uint16_t a, b, c, d;
    srand((unsigned int)&r_gold);

    for (uint16_t i = 0; i < NEWHOPE_N; i += 4)
    {
        // TODO: User rand() here
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

        r_test_dif_copy.coeffs[i] = a;
        r_test_dif_copy.coeffs[i + 1] = b;
        r_test_dif_copy.coeffs[i + 2] = c;
        r_test_dif_copy.coeffs[i + 3] = d;

        r_test_dit_copy.coeffs[i] = a;
        r_test_dit_copy.coeffs[i + 1] = b;
        r_test_dit_copy.coeffs[i + 2] = c;
        r_test_dit_copy.coeffs[i + 3] = d;
    }
    mul_coefficients(r_gold.coeffs, gammas_bitrev_montgomery);
    ntt_copy(r_gold.coeffs, gammas_bitrev_montgomery);
    // NTT DIF: BO-> NO
    mul_coefficients(r_test_dif.coeffs, gammas_bitrev_montgomery);
    ntt_dif(r_test_dif.coeffs, gammas_bitrev_montgomery);

    full_reduce(&r_gold);
    full_reduce(&r_test_dif);

    uint16_t res = compare(&r_gold, &r_test_dif, "NEWHOPE DIF vs MY DIF");
    
    // Make sure NTT_DIF and NTT_DIF_COPY are the same 
    mul_coefficients(r_test_dif_copy.coeffs, gammas_bitrev_montgomery);
    ntt_dif_copy(r_test_dif_copy.coeffs, gammas_bitrev_montgomery);
    full_reduce(&r_test_dif_copy);

    res = compare(&r_test_dif_copy, &r_test_dif, "DIF vs DIF_copy");
    
    // Revert back to original value
    for (uint16_t i = 0; i < NEWHOPE_N; i++){
        r_test_dif_copy.coeffs[i] = r_test_dit_copy.coeffs[i];
    }
    /************************************ Below are trash ************************/ 
    //TODO: Full reduction test
    
    // NTT DIF: BO-> NO
    mul_coefficients(r_test_dif_copy.coeffs, dif_gammas_bitrev);
    ntt_dif_copy_full_reduction(r_test_dif_copy.coeffs, dif_omegas_bitrev);
    // ntt_dif_copy(r_test_dif_copy.coeffs, omega_bitrev_order, 8);
    // ntt_dif_copy(r_test_dif_copy.coeffs, gammas_bitrev_full_reduction);
    
    // NTT DIT: BO-> NO
    // ntt_dit_copy(r_test_dit_copy.coeffs, omega_natural_order);
    // ntt_dit_copy(r_test_dit_copy.coeffs, bla, N);
    mul_coefficients(r_test_dit_copy.coeffs, dif_gammas_bitrev);
    ntt_dit_copy_full_reduction(r_test_dit_copy.coeffs, dif_omegas_bitrev);

    // full_reduce(&r_test_dit_copy);
    // full_reduce(&r_test_dif_copy);

    printArray(r_test_dif_copy.coeffs, NEWHOPE_N, "[GOLD]");
    printArray(r_test_dit_copy.coeffs, NEWHOPE_N, "[TEST]");

    res = compare(&r_test_dif_copy, &r_test_dit_copy, "my DIF vs my DIT");


    return res;
}