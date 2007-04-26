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

union W64_T {
    uint64_t u;
    double d;
};
typedef union W64_T w64_t;

union W128_T {
    __m128i si;
    __m128d sd;
    double d[2];
    uint64_t u[2];
};
typedef union W128_T w128_t;

INLINE static void gen_rand_array(__m128i array[], int size);
INLINE static void gen_rand_all(void);

static __m128i sfmt[N + 1];
static int idx;
static uint64_t *sfmtp = (uint64_t *)sfmt;
static uint32_t *sfmtp32 = (uint32_t *)sfmt;
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
#define LOW_MASK  0x000FFFFFFFFFFFFFULL
#define HIGH_CONST 0x3FF0000000000000ULL
#define LOW_MASK32_1 0x000fffffU
#define LOW_MASK32_2 0xffffffffU
#define HIGH_CONST32 0x3ff00000U

void fill_array_open_close(double array[], int size);
void fill_array_close_open(double array[], int size);
void fill_array_open_open(double array[], int size);
void fill_array_close1_open2(double array[], int size);
INLINE double genrand_close1_open2(void);

#if defined(__GNUC__) && (!defined(DEBUG))
#define ALWAYSINLINE __attribute__((always_inline))
#else
#define ALWAYSINLINE
#endif
INLINE static void convert_12(w128_t array[], int size) ALWAYSINLINE;
INLINE static void convert_co(w128_t array[], int size) ALWAYSINLINE;
INLINE static void convert_oc(w128_t array[], int size) ALWAYSINLINE;
INLINE static void convert_oo(w128_t array[], int size) ALWAYSINLINE;
INLINE uint32_t genrand_int32(void) ALWAYSINLINE;
INLINE void fill_array_int32(uint32_t array[], int size) ALWAYSINLINE;

static __m128i sse2_low_mask;
static __m128i sse2_high_const;
static __m128i sse2_high_const_one;
static __m128d sse2_double_two;
static __m128d sse2_double_m_one;

static void setup_const(void) {
    static int first = true;
    if (!first) {
	return;
    }
    sse2_low_mask = _mm_set_epi32(LOW_MASK32_1, LOW_MASK32_2,
				  LOW_MASK32_1, LOW_MASK32_2);
    sse2_high_const = _mm_set_epi32(HIGH_CONST32, 0, HIGH_CONST32, 0);
    sse2_high_const_one = _mm_set_epi32(HIGH_CONST32, 1, HIGH_CONST32, 1);
    sse2_double_two = _mm_set_pd(2.0, 2.0);
    sse2_double_m_one = _mm_set_pd(-1.0, -1.0);
    first = false;
}

INLINE static void convert_12(w128_t array[], int size) {
    int i;
    w128_t r;

    for (i = 0; i < size; i++) {
	r.si = _mm_and_si128(array[i].si, sse2_low_mask);
	array[i].si = _mm_or_si128(r.si, sse2_high_const);
    }
}

INLINE static void convert_co(w128_t array[], int size) {
    int i;
    w128_t r;

    for (i = 0; i < size; i++) {
	r.si = _mm_and_si128(array[i].si, sse2_low_mask);
	r.si = _mm_or_si128(r.si, sse2_high_const);
	array[i].sd = _mm_add_pd(r.sd, sse2_double_m_one);
    }
}

INLINE static void convert_oc(w128_t array[], int size) {
    int i;
    w128_t r;

    for (i = 0; i < size; i++) {
	r.si = _mm_and_si128(array[i].si, sse2_low_mask);
	r.si = _mm_or_si128(r.si, sse2_high_const);
	array[i].sd = _mm_sub_pd(sse2_double_two, r.sd);
    }
}

INLINE static void convert_oo(w128_t array[], int size) {
    int i;
    w128_t r;

    for (i = 0; i < size; i++) {
	r.si = _mm_and_si128(array[i].si, sse2_low_mask);
	r.si = _mm_or_si128(r.si, sse2_high_const_one);
	array[i].sd = _mm_add_pd(r.sd, sse2_double_m_one);
    }
}

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

INLINE uint32_t genrand_int32(void)
{
    if (idx >= N * 4) {
	gen_rand_all();
	idx = 0;
    }
    return sfmtp32[idx++];
}

INLINE double genrand_close1_open2(void)
{
    w64_t r;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r.u = sfmtp[idx++];
    r.u &= LOW_MASK;
    r.u |= HIGH_CONST;
    return r.d;
}

INLINE double genrand_open_open(void)
{
    w64_t r;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r.u = sfmtp[idx++];
    r.u &= LOW_MASK;
    r.u |= HIGH_CONST | 1;
    return r.d - 1.0;
}

INLINE void fill_array_int32(uint32_t array[], int size)
{
    assert(size >= N * 4);
    assert(size % 4 == 0);
    assert((int)array % 16 == 0);

    gen_rand_array((__m128i *)array, size / 4);
}

INLINE void fill_array_close1_open2(double array[], int size)
{
    assert(size >= N * 2);
    assert(size % 2 == 0);
    assert((int)array % 16 == 0);

    gen_rand_array((__m128i *)array, size / 2);
    convert_12((w128_t *)array, size / 2);
}

INLINE void fill_array_close_open(double array[], int size)
{
    assert(size >= N * 2);
    assert(size % 2 == 0);
    assert((int)array % 16 == 0);

    gen_rand_array((__m128i *)array, size / 2);
    convert_co((w128_t *)array, size / 2);
}

INLINE void fill_array_open_close(double array[], int size)
{
    assert(size >= N * 2);
    assert(size % 2 == 0);
    assert((int)array % 16 == 0);

    gen_rand_array((__m128i *)array, size / 2);
    convert_oc((w128_t *)array, size / 2);
}

INLINE void fill_array_open_open(double array[], int size)
{
    assert(size >= N * 2);
    assert(size % 2 == 0);
    assert((int)array % 16 == 0);

    gen_rand_array((__m128i *)array, size / 2);
    convert_oo((w128_t *)array, size / 2);
}

INLINE void init_gen_rand(uint64_t seed)
{
    int i;

    sfmtp[0] = (uint32_t)seed;
    for (i = 1; i < (N + 1) * 2; i++) {
	sfmtp[i] = 1812433253UL * (sfmtp[i - 1] ^ (sfmtp[i - 1] >> 30)) + i;
    }
    idx = N * 4;
    setup_const();
}

#if defined(ORIGINAL)
#include "test_time4.c"
#else
#include "test_time3.c"
#endif
