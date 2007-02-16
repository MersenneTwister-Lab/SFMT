/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <emmintrin.h>
#include "random.h"
#include "paramsD11.h"

INLINE static void gen_rand_array(__m128i array[], int size);
INLINE static void gen_rand_all(void);

static __m128i sfmt[N + 1];
static double *dsfmt = (double *)sfmt;
static int idx;

static __m128i sse2_param_mask;
static __m128i sse2_low_mask;
static __m128i sse2_high_const;
static __m128d sse2_double_two;

static void setup_const(void) {
    static int first = true;
    if (!first) {
	return;
    }
    sse2_param_mask = _mm_set_epi32(MSK32_3, MSK32_4, MSK32_1, MSK32_2);
    sse2_low_mask = _mm_set_epi32(LOW_MASK32_1, LOW_MASK32_2,
				  LOW_MASK32_1, LOW_MASK32_2);
    sse2_high_const = _mm_set_epi32(HIGH_CONST32, 0, HIGH_CONST32, 0);
    sse2_double_two = _mm_set_pd(2.0L, 2.0L);
    first = false;
}

INLINE static
#if defined(__GNUC__) && (!defined(DEBUG))
__attribute__((always_inline)) 
#endif
__m128i 
mm_recursion(__m128i *a, __m128i *b, __m128i c, __m128i d) {
    __m128i v, w, x, y, z;
    
    z = _mm_load_si128(a);
    y = _mm_srli_epi64(*b, SR1);
    y = _mm_and_si128(y, sse2_param_mask);
    w = _mm_slli_epi64(c, SL1);
    x = _mm_srli_epi64(c, SR2);
    v = _mm_shuffle_epi32(d, SSE2_SHUFF);
    w = _mm_xor_si128(w, x);
    v = _mm_xor_si128(v, z);
    z = _mm_slli_si128(z, SL2);
    w = _mm_xor_si128(w, y);
    v = _mm_xor_si128(v, z);
    v = _mm_xor_si128(v, w);
    v = _mm_and_si128(v, sse2_low_mask);
    v = _mm_or_si128(v, sse2_high_const);
    return v;
}

INLINE static
#if defined(__GNUC__) && (!defined(DEBUG))
__attribute__((always_inline)) 
#endif
    void gen_rand_all(void) {
    int i;
    __m128i r, lung;

    lung = _mm_load_si128(&sfmt[N]);
    r = _mm_load_si128(&sfmt[N - 1]);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1], r, lung);
	_mm_store_si128(&sfmt[i], r);
	lung = _mm_xor_si128(lung, r);
    }
    for (; i < N; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1 - N], r, lung);
	_mm_store_si128(&sfmt[i], r);
	lung = _mm_xor_si128(lung, r);
    }
    _mm_store_si128(&sfmt[N], lung);
}

INLINE static void gen_rand_array(__m128i array[], int size) {
    int i, j;
    __m128i r, lung;
    __m128d *darray;

    darray = (__m128d *)array;
    lung = _mm_load_si128(&sfmt[N]);
    r = _mm_load_si128(&sfmt[N - 1]);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1], r, lung);
	_mm_store_si128(&array[i], r);
	lung = _mm_xor_si128(lung, r);
    }
    for (; i < N; i++) {
	r = mm_recursion(&sfmt[i], &array[i + POS1 - N], r, lung);
	_mm_store_si128(&array[i], r);
	lung = _mm_xor_si128(lung, r);
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = mm_recursion(&array[i - N], &array[i + POS1 - N], r, lung);
	_mm_store_si128(&array[i], r);
	lung = _mm_xor_si128(lung, r);
	darray[i - N] = _mm_add_pd(darray[i - N], sse2_double_two);
    }
    for (j = 0; j < 2 * N - size; j++) {
	r = _mm_load_si128(&array[j + size - N]);
	_mm_store_si128(&sfmt[j], r);
    }    
    for (; i < size; i++, j++) {
	r = mm_recursion(&array[i - N], &array[i + POS1 - N], r, lung);
	_mm_store_si128(&array[i], r);
	_mm_store_si128(&sfmt[j], r);
	lung = _mm_xor_si128(lung, r);
	darray[i - N] = _mm_add_pd(darray[i - N], sse2_double_two);
    }
    for (j = size - N; j < size; j++) {
	darray[j] = _mm_add_pd(darray[j], sse2_double_two);
    }
    _mm_store_si128(&sfmt[N], lung);
}

INLINE
#if defined(__GNUC__) && (!defined(DEBUG))
__attribute__((always_inline)) 
#endif
    double gen_rand(void) {
    double r;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r = dsfmt[idx++];
    r += 2.0L;
    return r;
}

void fill_array(double array[], int size) {
    assert(size >= N * 2);
    assert(size % 2 == 0);
    assert((int)array % 16 == 0);

    gen_rand_array((__m128i *)array, size / 2);
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

#include "test_time.c"
