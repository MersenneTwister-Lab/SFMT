/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdio.h>
#if defined(__GNUC__)
  #include <inttypes.h>
#endif
#include "random-inline.h"
#include <emmintrin.h>
#include <assert.h>

#define MEXP 19937

#define WORDSIZE 128
#define N ((MEXP -1) / WORDSIZE)
#define MAXDEGREE (WORDSIZE * (N + 1))

//#define MAX_BLOCKS 10

INLINE static void gen_rand_array(__m128i array[], int size);
INLINE static void gen_rand_all(void);

static __m128i sfmt[N + 1];
static unsigned int idx;
static uint32_t *sfmtp = (uint32_t *)sfmt;

#define POS1 89
#define SL1 18
#define SL2 1
#define SR1 3
#define PM1 1
#define PM2 0
#define PM3 2
#define PM4 3
#define MSK1 0x7f7fffffU
#define MSK2 0xfffbffffU
#define MSK3 0xeffffbffU
#define MSK4 0xfefe7befU
#define SHUFF (PM1 | (PM2 << 2) | (PM3 << 4) | (PM4 << 6))

INLINE unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

INLINE unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

INLINE unsigned int get_onetime_rnds(void) {
    return N * 4;
}

INLINE static __m128i mm_recursion(__m128i *a, __m128i *b, 
				   __m128i c, __m128i d, __m128i mask) {
    __m128i v, x, y, z;
    
    x = _mm_load_si128(a);
    y = _mm_load_si128(b);
    z = _mm_slli_epi32(c, SL1);
    v = _mm_shuffle_epi32(d, SHUFF);
    z = _mm_xor_si128(z, v);
    z = _mm_xor_si128(z, x);
    x = _mm_slli_si128(x, SL2);
    y = _mm_srli_epi32(y, SR1);
    y = _mm_and_si128(y, mask);
    z = _mm_xor_si128(z, x);
    z = _mm_xor_si128(z, y);
    return z;
}

INLINE void gen_rand_all(void) {
    int i;
    __m128i r, u, mask;
    mask = _mm_set_epi32(MSK4, MSK3, MSK2, MSK1);

    r = _mm_load_si128(&sfmt[N - 1]);
    u = _mm_load_si128(&sfmt[N]);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1], r, u, mask);
	_mm_store_si128(&sfmt[i], r);
	u =_mm_xor_si128(u, r);
    }
    for (; i < N; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1 - N], r, u, mask);
	_mm_store_si128(&sfmt[i], r);
	u =_mm_xor_si128(u, r);
    }
    _mm_store_si128(&sfmt[N], u);
}

INLINE static void gen_rand_array(__m128i array[], int size) {
    int i, j;
    __m128i r, u, mask;
    mask = _mm_set_epi32(MSK4, MSK3, MSK2, MSK1);

    u = _mm_load_si128(&sfmt[N]);
    r = _mm_load_si128(&sfmt[N - 1]);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1], r, u, mask);
	_mm_store_si128(&array[i], r);
	u =_mm_xor_si128(u, r);
    }
    for (; i < N; i++) {
	r = mm_recursion(&sfmt[i], &array[i + POS1 - N], r, u, mask);
	_mm_store_si128(&array[i], r);
	u = _mm_xor_si128(u, r);
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = mm_recursion(&array[i - N], &array[i + POS1 - N], r, u, mask);
	_mm_store_si128(&array[i], r);
	u = _mm_xor_si128(u, r);
    }
    for (j = 0; j < 2 * N - size; j++) {
	r = _mm_load_si128(&array[j + size - N]);
	_mm_store_si128(&sfmt[j], r);
    }
    for (; i < size; i++, j++) {
	r = mm_recursion(&array[i - N], &array[i + POS1 - N], r, u, mask);
	_mm_store_si128(&array[i], r);
	_mm_store_si128(&sfmt[j], r);
	u = _mm_xor_si128(u, r);
    }
    _mm_store_si128(&sfmt[N], u);
}

INLINE uint32_t gen_rand(void)
{
    uint32_t r;

    if (idx >= N * 4) {
	gen_rand_all();
	idx = 0;
    }
    r = sfmtp[idx++];
    return r;
}

INLINE void fill_array(uint32_t array[], int size)
{
    //assert(size >= N * 4);
    //assert(size % 4 == 0);
    //assert((int)array % 16 == 0);

    gen_rand_array((__m128i *)array, size / 4);
}

INLINE void init_gen_rand(uint32_t seed)
{
    int i;

    sfmtp[0] = seed;
    for (i = 1; i < (N + 1) * 4; i++) {
	sfmtp[i] = 1812433253UL * (sfmtp[i - 1] ^ (sfmtp[i - 1] >> 30)) + i;
    }
    idx = N * 4;
}

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif
