/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdio.h>
#if defined(__GNUC__)
  #include <inttypes.h>
#endif
#include "random.h"
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
static int idx;
static uint64_t *sfmtp = (uint64_t *)sfmt;
#define SHUFF 0x39

#define POS1 4
#define SL1 20
#define SL2 1
#define SR1 1
#define SR2 1
#define MSK1 0xfffef7f9U
#define MSK2 0xffdff7fbU
#define MSK3 0xcbfff7feU
#define MSK4 0xedfefffdU

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
    y = _mm_srli_epi32(*b, SR1);
    z = _mm_srli_si128(c, SR2);
    v = _mm_slli_epi32(d, SL1);
    z = _mm_xor_si128(z, x);
    z = _mm_xor_si128(z, v);
    x = _mm_slli_si128(x, SL2);
    y = _mm_and_si128(y, mask);
    z = _mm_xor_si128(z, x);
    z = _mm_xor_si128(z, y);
    return z;
}

INLINE void gen_rand_all(void) {
    int i;
    __m128i r, r1, r2, mask;
    mask = _mm_set_epi32(MSK4, MSK3, MSK2, MSK1);

    r1 = _mm_load_si128(&sfmt[N - 2]);
    r2 = _mm_load_si128(&sfmt[N - 1]);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1], r1, r2, mask);
	_mm_store_si128(&sfmt[i], r);
	r1 = r2;
	r2 = r;
    }
    for (; i < N; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1 - N], r1, r2, mask);
	_mm_store_si128(&sfmt[i], r);
	r1 = r2;
	r2 = r;
    }
}

INLINE static void gen_rand_array(__m128i array[], int size) {
    int i, j;
    __m128i r, r1, r2, mask;
    mask = _mm_set_epi32(MSK4, MSK3, MSK2, MSK1);

    r1 = _mm_load_si128(&sfmt[N - 2]);
    r2 = _mm_load_si128(&sfmt[N - 1]);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1], r1, r2, mask);
	_mm_store_si128(&array[i], r);
	r1 = r2;
	r2 = r;
    }
    for (; i < N; i++) {
	r = mm_recursion(&sfmt[i], &array[i + POS1 - N], r1, r2, mask);
	_mm_store_si128(&array[i], r);
	r1 = r2;
	r2 = r;
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = mm_recursion(&array[i - N], &array[i + POS1 - N], r1, r2, mask);
	_mm_store_si128(&array[i], r);
	r1 = r2;
	r2 = r;
    }
    for (j = 0; j < 2 * N - size; j++) {
	r = _mm_load_si128(&array[j + size - N]);
	_mm_store_si128(&sfmt[j], r);
    }    
    for (; i < size; i++, j++) {
	r = mm_recursion(&array[i - N], &array[i + POS1 - N], r1, r2, mask);
	_mm_store_si128(&array[i], r);
	_mm_store_si128(&sfmt[j], r);
	r1 = r2;
	r2 = r;
    }
}

#define LOW_MASK  0x000FFFFFFFFFFFFFULL
#define HIGH_CONST 0xBFF0000000000000ULL
#define LOW_MASK32_1 0x000fffffU
#define LOW_MASK32_2 0xffffffffU
#define HIGH_CONST32 0xbff00000U

INLINE double gen_rand(void)
{
    uint64_t r;
    double *dp;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r = sfmtp[idx++];
    r &= LOW_MASK;
    r |= HIGH_CONST;
    dp = (double *)&r;
    *dp += 2.0L;
    return *dp;
}

INLINE void fill_array(double array[], int size)
{
    int i;
    __m128i low_mask, high_const;
    __m128d double_two, *dp;
    __m128i r, *ap;

    low_mask = _mm_set_epi32(LOW_MASK32_1, LOW_MASK32_2,
				  LOW_MASK32_1, LOW_MASK32_2);
    high_const = _mm_set_epi32(HIGH_CONST32, 0, HIGH_CONST32, 0);
    double_two = _mm_set_pd(2.0L, 2.0L);
    //assert(size >= N * 4);
    //assert(size % 4 == 0);
    //assert((int)array % 16 == 0);

    gen_rand_array((__m128i *)array, size / 2);
    ap = (__m128i *)array;
    for (i = 0; i < size / 2; i++) {
	r = _mm_and_si128(ap[i], low_mask);
	ap[i] = _mm_or_si128(r, high_const);
    }
    dp = (__m128d *)array;
    for (i = 0; i < size / 2; i++) {
	dp[i] = _mm_add_pd(dp[i], double_two);
    }
}

INLINE void init_gen_rand(uint64_t seed)
{
    int i;

    sfmtp[0] = (uint32_t)seed;
    for (i = 1; i < (N + 1) * 2; i++) {
	sfmtp[i] = 1812433253UL * (sfmtp[i - 1] ^ (sfmtp[i - 1] >> 30)) + i;
    }
    idx = N * 4;
}

#include "test_time.c"

