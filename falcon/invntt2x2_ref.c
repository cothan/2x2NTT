#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "params.h"
#include "consts.h"

#define gsbf(a, b, z, t)             \
  t = (a + FALCON_Q - b) % FALCON_Q; \
  a = (a + FALCON_Q + b) % FALCON_Q; \
  b = ((uint32_t)t * z) % FALCON_Q;

#define DEBUG 0

void invntt2x2_ref(uint16_t a[FALCON_N])
{
  uint16_t len, last;
  uint16_t a1, b1, a2, b2;
  uint16_t t1, t2;
  uint16_t k1[2], k2;
  uint16_t zeta1[2], zeta2;
  for (unsigned l = 0; l < FALCON_LOGN; l += 2)
  {
    len = 1 << l;
    last = (l == (FALCON_LOGN - 2));
    for (unsigned i = 0; i < FALCON_N; i += 1 << (l + 2))
    {
      k1[0] = (FALCON_N >> l) - 1 - (i >> (l + 1));
      k1[1] = k1[0] - 1;
      k2 = (FALCON_N >> (l + 1)) - 1 - (i >> (l + 2));
      zeta1[0] = FALCON_Q - zetas_barrett[k1[0]];
      zeta1[1] = FALCON_Q - zetas_barrett[k1[1]];
      zeta2 = FALCON_Q - zetas_barrett[k2];
      // printf("[======]%d\n", i);
      for (unsigned j = i; j < i + len; j++)
      {
        a1 = a[j];
        b1 = a[j + len];
        a2 = a[j + 2 * len];
        b2 = a[j + 3 * len];

        // Left
        // a1 - b1, a2 - b2
        gsbf(a1, b1, zeta1[0], t1);
        gsbf(a2, b2, zeta1[1], t2);
#if DEBUG == 1
        printf("[%d]: %u, %u = %u, %u | %u\n", len, j, j + len,
               a1, b1, k1[0]);
        printf("[%d]: %u, %u = %u, %u | %u\n", len, j + 2 * len, j + 3 * len,
               a2, b2, k1[1]);
#endif
        // Right
        // a1 - a2, b1 - b2
        gsbf(a1, a2, zeta2, t1);
        gsbf(b1, b2, zeta2, t2);
#if DEBUG == 1
        printf("[%d]: %u, %u = %u, %u | %u\n", 2 * len, j, j + 2 * len,
               a1, a2, k2);
        printf("[%d]: %u, %u = %u, %u | %u\n", 2 * len, j + len, j + 3 * len,
               b1, b2, k2);
#endif
        a[j] = a1;
        a[j + len] = b1;
        a[j + 2 * len] = a2;
        a[j + 3 * len] = b2;
      }
    }
  }
}

void invntt(uint16_t a[FALCON_N])
{
  unsigned int start, len, j, k;
  uint16_t t, zeta, w;
  uint16_t m, n;

#if FALCON_N == 256
  const uint16_t f = 256;
#elif FALCON_N == 512
  const uint16_t f = 128;
#elif FALCON_N == 1024
  const uint16_t f = 64;
#else
#error "See config.h, FALCON_N is not supported"
#endif

  k = FALCON_N;
  for (len = 1; len < FALCON_N; len <<= 1)
  {
    for (start = 0; start < FALCON_N; start = j + len)
    {
      // Plus Q so it is alway positive
      zeta = FALCON_Q - zetas_barrett[--k];
      for (j = start; j < start + len; ++j)
      {
        t = a[j];
        a[j] = (t + a[j + len]) % FALCON_Q;
        w = (t + FALCON_Q - a[j + len]) % FALCON_Q;
        a[j + len] = ((uint32_t)zeta * w) % FALCON_Q;

        m = a[j];
        n = a[j + len];
        // printf("%d: %u, %u = %u, %u | %u\n", len, j, j + len, m, n, k);
      }
    }
  }

  // f is multiple of 2, so shift and reduction
  // for (j = 0; j < FALCON_N; ++j)
  // {
  //     a[j] = barret_mul(f, a[j]);
  // }
}

#define TESTS 1000000

int main()
{
  uint16_t a[FALCON_N] = {0}, a_gold[FALCON_N] = {0};
  uint16_t tmp;
  srand(0);
  for (int j = 0; j < TESTS; j++)
  {
    // Test million times
    for (int i = 0; i < FALCON_N; i++)
    {
      tmp = rand() % 0xffff;
      a[i] = tmp;
      a_gold[i] = tmp;
    }

    invntt(a_gold);
    // printf("=======\n");
    invntt2x2_ref(a);

    for (int i = 0; i < FALCON_N; i++)
    {
      if (a_gold[i] != a[i])
      {
        printf("%d: %u != %u", i, a_gold[i], a[i]);
        return 1;
      }
    }
  }
  return 0;
}