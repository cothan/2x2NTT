#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "params.h"
#include "consts.h"

#define ctbf(a, b, z, t)             \
  t = ((uint32_t)b * z) % FALCON_Q;  \
  b = (a + FALCON_Q - t) % FALCON_Q; \
  a = (a + t) % FALCON_Q;

// Falcon_N = 1024, 256
// Falcon_N = 512
void ntt2x2_ref(uint16_t a[FALCON_N])
{
  uint16_t zeta1, zeta2[2];
  uint16_t a1, b1, a2, b2;
  uint16_t t1, t2;
  uint16_t k1, k2[2];
  for (int l = FALCON_LOGN; l > 0; l -= 2)
  {
    uint16_t len = 1 << (l - 2);
    for (unsigned i = 0; i < FALCON_N; i += 1 << l)
    {
      k1 = (FALCON_N + i) >> l;
      k2[0] = (FALCON_N + i) >> (l - 1);
      k2[1] = k2[0]+ 1;
      zeta1 = zetas_barrett[k1];
      zeta2[0] = zetas_barrett[k2[0]];
      zeta2[1] = zetas_barrett[k2[1]];

      for (unsigned j = i; j < i + len; j++)
      {
        a1 = a[j];
        a2 = a[j + len];
        b1 = a[j + 2 * len];
        b2 = a[j + 3 * len];

        // Left
        // a1 - b1, a2 - b2
        ctbf(a1, b1, zeta1, t1);
        ctbf(a2, b2, zeta1, t2);
#if DEBUG == 1
        printf("[%d]: %u, %u = %u, %u | %u\n", 2 * len, j, j + 2 * len,
               a1, b1, k1);
        printf("[%d]: %u, %u = %u, %u | %u\n", 2 * len, j + len, j + 3 * len,
               a2, b2, k1);
#endif
        // Right
        // a1 - a2, b1 - b2
        ctbf(a1, a2, zeta2[0], t1);
        ctbf(b1, b2, zeta2[1], t2);
#if DEBUG == 1
        printf("[%d]: %u, %u = %u, %u | %u\n", len, j, j + len,
               a1, a2, k2[0]);
        printf("[%d]: %u, %u = %u, %u | %u\n", len, j + 2 * len, j + 3 * len,
               b1, b2, k2[1]);
#endif
        a[j] = a1;
        a[j + len] = a2;
        a[j + 2 * len] = b1;
        a[j + 3 * len] = b2;
      }
    }
  }
}

void ntt(uint16_t a[FALCON_N])
{
  unsigned int len, start, j, k;
  uint16_t zeta, t;
  uint16_t m, n;

  k = 0;
  unsigned count = 1;
  for (len = FALCON_N / 2; len > 0; len >>= 1)
  {
    for (start = 0; start < FALCON_N; start = j + len)
    {
      zeta = zetas_barrett[++k];
      for (j = start; j < start + len; ++j)
      {

        t = ((uint32_t)zeta * a[j + len]) % FALCON_Q;
        a[j + len] = (a[j] + FALCON_Q - t) % FALCON_Q;
        a[j] = (a[j] + t) % FALCON_Q;

        m = a[j];
        n = a[j + len];
#if DEBUG == 1
        printf("%d: %u, %u = %u, %u | %u\n", len, j, j + len, m, n, k);
#endif
      }
    }
  }
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

    ntt(a_gold);
    // printf("=======\n");
    ntt2x2_ref(a);

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

// Compile flags
// gcc -o ntt2x2_ref consts.cpp ntt2x2_ref.c; ./ntt2x2_ref
