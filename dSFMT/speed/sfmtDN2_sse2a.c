/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <emmintrin.h>
#include "random.h"
#include "paramsDN2.h"

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 104
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

union W128_T {
    __m128i si;
    __m128d sd;
    uint64_t a[2];
    double d[2];
};
typedef union W128_T w128_t;

union W64_T {
    double d;
    uint64_t u;
};
typedef union W64_T w64_t;

INLINE static void gen_rand_array(w128_t array[], int size);
INLINE static void gen_rand_all(void);

void fill_array_open_close(double array[], int size);
void fill_array_close_open(double array[], int size);
void fill_array_open_open(double array[], int size);
void fill_array_close1_open2(double array[], int size);
INLINE double genrand_open_close(void);
INLINE double genrand_close_open(void);
INLINE double genrand_open_open(void);
INLINE double genrand_close1_open2(void);

static w128_t sfmt[N];
static double *psfmt = &sfmt[0].d[0];
static int idx;

static __m128i sse2_param_mask1;
static __m128i sse2_param_mask2;
static __m128i sse2_int_one;
static __m128d sse2_double_two;
static __m128d sse2_double_m_one;

static void setup_const(void) {
    static int first = true;
    if (!first) {
	return;
    }
    sse2_param_mask1 = _mm_set_epi32(SFMT_MSK32_3, SFMT_MSK32_4,
				     SFMT_MSK32_1, SFMT_MSK32_2);
    sse2_param_mask2 = _mm_set_epi32(SFMT_MSK32_7, SFMT_MSK32_8,
				     SFMT_MSK32_5, SFMT_MSK32_6);
    sse2_int_one = _mm_set_epi32(0, 1, 0, 1);
    sse2_double_two = _mm_set_pd(2.0, 2.0);
    sse2_double_m_one = _mm_set_pd(-1.0, -1.0);
    first = false;
}

#if defined(__GNUC__) && (!defined(DEBUG))
#define ALWAYSINLINE __attribute__((always_inline)) 
#else
#define ALWAYSINLINE
#endif
INLINE static __m128i mm_recursion(__m128i a, __m128i b, __m128i c) 
    ALWAYSINLINE;
INLINE static void convert_oc(w128_t array[], int size) ALWAYSINLINE;
#if 0
INLINE static void convert_co(w128_t array[], int size) ALWAYSINLINE;
#endif
INLINE static void convert_oo(w128_t array[], int size) ALWAYSINLINE;

INLINE static __m128i mm_recursion(__m128i a, __m128i b, __m128i c) {
  __m128i x1, x2, x3, y1, y2, y3, z1, z2, z3, z4, r1, r2;
    
    x1 = _mm_shuffle_epi32(a, SSE2_SHUFF);
    x2 = _mm_srli_epi64(a, 12);
    x3 = _mm_xor_si128(x1, x2);

    y1 = _mm_slli_epi64(b, SFMT_SL1);
    y2 = _mm_xor_si128(y1, b);
    y3 = _mm_and_si128(y2, sse2_param_mask1);

    z1 = _mm_slli_epi64(c, SFMT_SL2);
    z2 = _mm_and_si128(z1, sse2_param_mask2);
    z3 = _mm_srli_epi64(c, SFMT_SR2);
    z4 = _mm_xor_si128(z2, z3);

    r1 = _mm_xor_si128(x3, y3);
    r2 = _mm_xor_si128(r1, z4);
    return r2;
}

INLINE static void convert_oc(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].sd = _mm_sub_pd(sse2_double_two, array[i].sd);
    }
}

#if 0
INLINE static void convert_co(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].sd = _mm_add_pd(array[i].sd, sse2_double_m_one);
    }
}
#endif

INLINE static void convert_oo(w128_t array[], int size) {
    int i;
    for (i = 0; i < size; i++) {
	array[i].si = _mm_or_si128(array[i].si, sse2_int_one);
	array[i].sd = _mm_add_pd(array[i].sd, sse2_double_m_one);
    }
}

INLINE static void gen_rand_all(void) {
    int i;
    __m128i r;

    r = _mm_load_si128(&sfmt[N - 1].si);
    for (i = 0; i < N - SFMT_POS1; i++) {
	r = mm_recursion(sfmt[i].si, sfmt[i + SFMT_POS1].si, r);
	sfmt[i].si = r;
    }
    for (; i < N; i++) {
	r = mm_recursion(sfmt[i].si, sfmt[i + SFMT_POS1 - N].si, r);
	sfmt[i].si = r;
    }
}

INLINE static void gen_rand_array12(w128_t array[], int size) {
    int i, j;
    __m128i r;

    r = _mm_load_si128(&sfmt[N - 1].si);
    for (i = 0; i < N - SFMT_POS1; i++) {
	r = mm_recursion(sfmt[i].si, sfmt[i + SFMT_POS1].si, r);
	array[i].si = r;
    }
    for (; i < N; i++) {
	r = mm_recursion(sfmt[i].si, array[i + SFMT_POS1 - N].si, r);
	array[i].si = r;
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = mm_recursion(array[i - N].si, array[i + SFMT_POS1 - N].si, r);
	array[i].si = r;
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j].si = array[j + size - N].si;
    }    
    for (; i < size; i++, j++) {
	r = mm_recursion(array[i - N].si, array[i + SFMT_POS1 - N].si, r);
	array[i].si = r;
	sfmt[j].si = r;
    }
}

INLINE static void gen_rand_arrayco(w128_t array[], int size) {
    int i, j;
    __m128i r;

    r = _mm_load_si128(&sfmt[N - 1].si);
    for (i = 0; i < N - SFMT_POS1; i++) {
	r = mm_recursion(sfmt[i].si, sfmt[i + SFMT_POS1].si, r);
	array[i].si = r;
    }
    for (; i < N; i++) {
	r = mm_recursion(sfmt[i].si, array[i + SFMT_POS1 - N].si, r);
	array[i].si = r;
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = mm_recursion(array[i - N].si, array[i + SFMT_POS1 - N].si, r);
	array[i].si = r;
	array[i - N].sd = _mm_add_pd(array[i - N].sd, sse2_double_m_one);
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j].si = array[j + size - N].si;
    }    
    for (; i < size; i++, j++) {
	r = mm_recursion(array[i - N].si, array[i + SFMT_POS1 - N].si, r);
	array[i].si = r;
	sfmt[j].si = r;
	array[i - N].sd = _mm_add_pd(array[i - N].sd, sse2_double_m_one);
    }
    for (i = size - N; i < size; i++) {
	array[i].sd = _mm_add_pd(array[i].sd, sse2_double_m_one);
    }
}

INLINE double genrand_close1_open2(void) {
    double r;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r = psfmt[idx++];
    return r;
}

INLINE double genrand_open_open(void) {
    w64_t r;

    r.d = genrand_close1_open2();
    r.u |= 1;
    return r.d - 1.0;
}

void fill_array_open_close(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array12((w128_t *)array, size / 2);
    convert_oc((w128_t *)array, size / 2);
}

void fill_array_close_open(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_arrayco((w128_t *)array, size / 2);
    //convert_co((w128_t *)array, size / 2);
}

void fill_array_open_open(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array12((w128_t *)array, size / 2);
    convert_oo((w128_t *)array, size / 2);
}

void fill_array_close1_open2(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array12((w128_t *)array, size / 2);
}

void init_gen_rand(uint64_t seed)
{
    int i;
    uint64_t *psfmt;

    psfmt = (uint64_t *)&sfmt[0];
    psfmt[0] = (seed & SFMT_LOW_MASK) | SFMT_HIGH_CONST;
    for (i = 1; i < N * 2; i++) {
	psfmt[i] = 6364136223846793005ULL 
	    * (psfmt[i - 1] ^ (psfmt[i - 1] >> 62)) + i;
	psfmt[i] = (psfmt[i] & SFMT_LOW_MASK) | SFMT_HIGH_CONST;
    }
    idx = N * 2;
    setup_const();
}

#include "test_time3.c"
