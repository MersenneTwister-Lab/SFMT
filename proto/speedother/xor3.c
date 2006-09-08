#include <string.h>
#if defined(__GNUC__)
  #include <stdint.h>
#endif
#include <stdio.h>
#include <assert.h>
#include "random-inline.h"

#define N 25
#define M1 15
#define M2 21
#define MultA(x) (x ^ (x >> 11))
#define MultB(x) (x ^ (x << 21))
#define MultC(x) (x ^ (x << 7))

static int num_used = 0;
static uint32_t x[N];
static uint32_t init_x[N]={ /* initial 25 seeds, change as you wish */
    0x95f24dab, 0x0b685215, 0xe76ccae7, 0xaf3ec239, 0x715fad23,
    0x24a590ad, 0x69e4b5ef, 0xbf456141, 0x96bc1b7b, 0xa7bdf825,
    0xc1de75b7, 0x8858a9c9, 0x2da87693, 0xb657f9dd, 0xffdc8a9f,
    0x8121da71, 0x8b823ecb, 0x885d05f5, 0x4e20cd47, 0x5a9ad5d9,
    0x512c0c03, 0xea857ccd, 0x4cc1d30f, 0x8891a8a1, 0xa6b7aadb
};

INLINE uint32_t gen_rand(void)
{
  uint32_t y;
  if (num_used==N) { /* generate N words at one time */
    int kk;
    for (kk=0;kk<N-M2;kk++)
      x[kk] = MultA(x[kk+M1]) ^ MultB(x[kk+M2]) ^ MultC(x[kk]);
    for (;kk<N-M1;kk++)
      x[kk] = MultA(x[kk+M1]) ^ MultB(x[kk+M2-N]) ^ MultC(x[kk]);
    for (;kk<N;kk++)
      x[kk] = MultA(x[kk+M1-N]) ^ MultB(x[kk+M2-N]) ^ MultC(x[kk]);
    num_used=0;
  }
  y = x[num_used];
  num_used++;
  return(y);
}

INLINE void init_gen_rand(uint32_t seed)
{
    int i;
    for (i = 0; i < N; i++) {
	x[i] = init_x[i];
    }
    x[0] = seed;
    num_used = N;
}

INLINE void fill_array(uint32_t array[], int size)
{
    int i, j;

    assert(size >= N);
    for (i = 0; i < N - M2; i++) {
	array[i] = MultA(x[i + M1]) ^ MultB(x[i + M2]) ^ MultC(x[i]);
    }
    for (;i < N - M1;i++) {
	array[i] = MultA(x[i + M1]) ^ MultB(array[i + M2 - N]) ^ MultC(x[i]);
    }
    for (;i < N; i++) {
	array[i] = MultA(array[i + M1 - N]) ^ MultB(array[i + M2 - N]) 
	    ^ MultC(x[i]);
    }
    for (; i < size - N; i++) {
	array[i] = MultA(array[i + M1 - N]) ^ MultB(array[i + M2 - N]) 
	    ^ MultC(array[i - N]);
    }
    for (j = 0; j < 2 * N - size; j++) {
	x[j] = array[j + size - N];
    }
    for (; i < size; i++, j++) {
	array[i] = MultA(array[i + M1 - N]) ^ MultB(array[i + M2 - N]) 
	    ^ MultC(array[i - N]);
	x[j] = array[i];
    }
}

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif
