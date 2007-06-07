/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <emmintrin.h>
#include "random.h"
#include "paramsDY2.h"

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

static w128_t sfmt[N + 1];
static double *psfmt = &sfmt[0].d[0];
static int idx;

static __m128i sse2_param_mask;
static __m128i sse2_low_mask;
static __m128i sse2_int_one;
static __m128d sse2_double_two;
static __m128d sse2_double_m_one;

static void setup_const(void) {
    static int first = true;
    if (!first) {
	return;
    }
    sse2_param_mask = _mm_set_epi32(MSK32_3, MSK32_4, MSK32_1, MSK32_2);
    sse2_low_mask = _mm_set_epi32(LOW_MASK32_1, LOW_MASK32_2,
				  LOW_MASK32_1, LOW_MASK32_2);
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
INLINE static __m128i mm_recursion(__m128i *a, __m128i c, __m128i d) 
    ALWAYSINLINE;
INLINE static void convert_oc(w128_t array[], int size) ALWAYSINLINE;
INLINE static void convert_co(w128_t array[], int size) ALWAYSINLINE;
INLINE static void convert_oo(w128_t array[], int size) ALWAYSINLINE;

INLINE static __m128i recur_body(__m128i *a, __m128i r, __m128i u) {
    __m128i x, y;
    
    x = _mm_load_si128(a);
    y = _mm_shuffle_epi32(x, SSE2_SHUFF);
    x = _mm_slli_epi64(x, SL1);
    x = _mm_xor_si128(x, u);

    r = _mm_srli_epi64(r, SR1);
    r = _mm_and_si128(r, sse2_param_mask);
    r = _mm_xor_si128(r, x);

    r = _mm_and_si128(r, sse2_low_mask);
    r = _mm_xor_si128(r, y);
    return r;
}

INLINE static __m128i recur_lung(__m128i r, __m128i u) {
    r = _mm_xor_si128(r, u);
    r = _mm_slli_epi64(r, SL2);
    return u;
}

INLINE static void convert_oc(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].sd = _mm_sub_pd(sse2_double_two, array[i].sd);
    }
}

INLINE static void convert_co(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].sd = _mm_add_pd(array[i].sd, sse2_double_m_one);
    }
}

INLINE static void convert_oo(w128_t array[], int size) {
    int i;
    for (i = 0; i < size; i++) {
	array[i].si = _mm_or_si128(array[i].si, sse2_int_one);
	array[i].sd = _mm_add_pd(array[i].sd, sse2_double_m_one);
    }
}

INLINE static void gen_rand_all(void) {
    int i;
    __m128i r, u, t;

    u = _mm_load_si128(&sfmt[N].si);
    r = _mm_load_si128(&sfmt[N - 1].si);
    for (i = 0; i < N; i++) {
	t = recur_lung(r, u);
	r = recur_body(&sfmt[i].si, r, u);
	u = _mm_xor_si128(u, t);
	_mm_store_si128(&sfmt[i].si, r);
    }
    _mm_store_si128(&sfmt[N].si, u);
}

INLINE static void gen_rand_array(w128_t array[], int size) {
    int i, j;
    __m128i r, u, t;

    u = _mm_load_si128(&sfmt[N].si);
    r = _mm_load_si128(&sfmt[N - 1].si);
    for (i = 0; i < N; i++) {
	t = recur_lung(r, u);
	r = recur_body(&sfmt[i].si, r, u);
	u = _mm_xor_si128(u, t);
	_mm_store_si128(&array[i].si, r);
    }
    /* main loop */
    for (; i < size - N; i++) {
	t = recur_lung(r, u);
	r = recur_body(&array[i - N].si, r, u);
	u = _mm_xor_si128(u, t);
	_mm_store_si128(&array[i].si, r);
    }
    for (j = 0; j < 2 * N - size; j++) {
	r = _mm_load_si128(&array[j + size - N].si);
	_mm_store_si128(&sfmt[j].si, r);
    }    
    for (; i < size; i++, j++) {
	t = recur_lung(r, u);
	r = recur_body(&array[i - N].si, r, u);
	u = _mm_xor_si128(u, r);
	_mm_store_si128(&array[i].si, r);
	_mm_store_si128(&sfmt[j].si, r);
    }
    _mm_store_si128(&sfmt[N].si, u);
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
    gen_rand_array((w128_t *)array, size / 2);
    convert_oc((w128_t *)array, size / 2);
}

void fill_array_close_open(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array((w128_t *)array, size / 2);
    convert_co((w128_t *)array, size / 2);
}

void fill_array_open_open(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array((w128_t *)array, size / 2);
    convert_oo((w128_t *)array, size / 2);
}

void fill_array_close1_open2(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array((w128_t *)array, size / 2);
}

void init_gen_rand(uint64_t seed)
{
    int i;
    uint64_t *psfmt;

    psfmt = (uint64_t *)&sfmt[0];
    psfmt[0] = (seed & LOW_MASK) | HIGH_CONST;
    for (i = 1; i < (N + 1) * 2; i++) {
        psfmt[i] = 1812433253UL * (psfmt[i - 1] ^ (psfmt[i - 1] >> 30)) + i;
        psfmt[i] = (psfmt[i] & LOW_MASK) | HIGH_CONST;
    }
    idx = N * 2;
    setup_const();
}

#include "test_time3.c"
