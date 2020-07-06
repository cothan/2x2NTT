#include "hls_ntt_test.h"
/*
 218   │ def iterative_ntt_ches(a):
 219   │     """
 220   │     BO -> NO 
 221   │     Iteratite NTT in CHES paper
 222   │     https:////practice-project.eu//downloads//publications//ches_final.pdf
 223   │     """
 224   │     # a = bit_rev(a)
 225   │ 
 226   │     # for s in range(logN):
 227   │     for s in range(logN):
 228   │         m = 2 << s
 229   │         omega_m = NEWHOPE_N//m
 230   │         omega_idx = 0
 231   │ 
 232   │         for j in range(m//2):
 233   │             for k in range(0, NEWHOPE_N, m):
 234   │                 b = k + j
 235   │ 
 236   │                 u = a[b]
 237   │                 t = a[b+m//2]
 238   │ 
 239   │                 t = (omega_table[omega_idx] * t)
 240   │ 
 241   │                 a[b] = (u+t) % NEWHOPE_Q
 242   │                 a[b+m//2] = (u-t) % NEWHOPE_Q
 243   │ 
 244   │                 if DEBUG2 and b < less_32:
 245   │                     print('{} {} | {} {} | {} {} | "{}"'.format(
 246   │                         b, b + m//2, u, t,  a[b], a[b+m//2], omega_idx), '--', s)
 247   │ 
 248   │             omega_idx += omega_m
 249   │         if DEBUG2:
 250   │             print('=================', s)
 251   │     return a

*/

#define NEWHOPE_LOGN 10

uint16_t omega_table[NEWHOPE_N/2] = {4075, 1962, 52, 12115, 9090, 11177, 885, 8855, 147, 3762, 3482, 1583, 8410, 3054, 8687, 6381, 10316, 5184, 2505, 11942, 5415, 3150, 12147, 6147, 11099, 1146, 7509, 1815, 8579, 8633, 7507, 2767, 10120, 168, 6055, 5735, 10587, 11367, 8757, 9928, 5537, 6523, 4169, 6374, 1359, 4433, 3600, 6860, 3514, 10929, 8332, 3315, 7341, 8049, 2844, 10335, 8429, 6299, 192, 6920, 11821, 1566, 4213, 10008, 4324, 6328, 10966, 3009, 5529, 10331, 10333, 9381, 7840, 4016, 5468, 2500, 2033, 3123, 421, 8517, 1278, 6122, 10710, 1975, 6153, 8243, 8812, 8326, 6171, 11964, 7232, 10777, 6950, 9830, 3029, 8298, 7210, 8960, 11612, 2738, 11635, 4079, 58, 9259, 4467, 11994, 5241, 12240, 11035, 7032, 7665, 1293, 11271, 5297, 10162, 4754, 10561, 11454, 4212, 10484, 11239, 8240, 10240, 4493, 11907, 9786, 11684, 5333, 5315, 1594, 3174, 723, 12233, 2078, 6281, 8760, 8500, 9370, 787, 6347, 1922, 6803, 6068, 11836, 6715, 11089, 5906, 2925, 8646, 1319, 11184, 9842, 9606, 11341, 8844, 5383, 6093, 12225, 5886, 156, 11767, 2692, 8953, 2655, 1987, 441, 11286, 10446, 4749, 652, 9162, 1483, 6854, 6370, 3263, 7515, 11248, 3956, 9450, 11863, 6152, 8719, 3438, 10238, 5445, 1159, 1321, 10232, 8301, 5782, 504, 5876, 4916, 7183, 9523, 1693, 5206, 4322, 7280, 218, 6833, 4077, 1010, 10800, 8291, 10542, 8209, 418, 9945, 9734, 11858, 8532, 6427, 709, 6608, 576, 8471, 10885, 4698, 350, 5446, 683, 6695, 8320, 9027, 4298, 6415, 6421, 3565, 11231, 12048, 4115, 7500, 6099, 9369, 1263, 973, 3834, 6077, 7552, 5925, 6170, 151, 1858, 400, 6224, 11314, 9407, 7753, 8561, 4912, 9087, 316, 9341, 2302, 10258, 8214, 10327, 12237, 174, 3199, 1112, 11404, 3434, 12142, 8527, 8807, 10706, 3879, 9235, 3602, 5908, 1973, 7105, 9784, 347, 6874, 9139, 142, 6142, 1190, 11143, 4780, 10474, 3710, 3656, 4782, 9522, 2169, 12121, 6234, 6554, 1702, 922, 3532, 2361, 6752, 5766, 8120, 5915, 10930, 7856, 8689, 5429, 8775, 1360, 3957, 8974, 4948, 4240, 9445, 1954, 3860, 5990, 12097, 5369, 468, 10723, 8076, 2281, 7965, 5961, 1323, 9280, 6760, 1958, 1956, 2908, 4449, 8273, 6821, 9789, 10256, 9166, 11868, 3772, 11011, 6167, 1579, 10314, 6136, 4046, 3477, 3963, 6118, 325, 5057, 1512, 5339, 2459, 9260, 3991, 5079, 3329, 677, 9551, 654, 8210, 12231, 3030, 7822, 295, 7048, 49, 1254, 5257, 4624, 10996, 1018, 6992, 2127, 7535, 1728, 835, 8077, 1805, 1050, 4049, 2049, 7796, 382, 2503, 605, 6956, 6974, 10695, 9115, 11566, 56, 10211, 6008, 3529, 3789, 2919, 11502, 5942, 10367, 5486, 6221, 453, 5574, 1200, 6383, 9364, 3643, 10970, 1105, 2447, 2683, 948, 3445, 6906, 6196, 64, 6403, 12133, 522, 9597, 3336, 9634, 10302, 11848, 1003, 1843, 7540, 11637, 3127, 10806, 5435, 5919, 9026, 4774, 1041, 8333, 2839, 426, 6137, 3570, 8851, 2051, 6844, 11130, 10968, 2057, 3988, 6507, 11785, 6413, 7373, 5106, 2766, 10596, 7083, 7967, 5009, 12071, 5456, 8212, 11279, 1489, 3998, 1747, 4080, 11871, 2344, 2555, 431, 3757, 5862, 11580, 5681, 11713, 3818, 1404, 7591, 11939, 6843, 11606, 5594, 3969, 3262, 7991, 5874, 5868, 8724, 1058, 241, 8174, 4789, 6190, 2920, 11026, 11316, 8455, 6212, 4737, 6364, 6119, 12138, 10431, 11889, 6065, 975, 2882, 4536, 3728, 7377, 3202, 11973, 2948, 9987, 2031};

void iterative_ntt(uint16_t *a)
{
    for (uint16_t s = 0; s < NEWHOPE_LOGN; s++)
    {
        uint16_t m = 2 << s;
        uint16_t omega_m = NEWHOPE_N / m;
        uint16_t omega_idx = 0;

        for (uint16_t j = 0; j < m / 2; j++)
        {
            for (uint16_t k = 0; k < NEWHOPE_N; k += m)
            {
                uint16_t b = j + k;
                uint16_t u = a[b];
                uint16_t t = a[b + m / 2];

                t = montgomery_reduce(omega_table[omega_idx] * t);
                printf("b[%d] + b[%d] * omega[%d] (%d) \n", b, b + m / 2, omega_idx, omega_table[omega_idx]);
                a[b] = (u + t);
                a[b + m / 2] = (u - t);
            }
            omega_idx += omega_m;
        }
    }
}