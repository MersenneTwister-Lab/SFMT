/** 
 * @file dSFMT-sse2.c 
 * @brief double precision SIMD-oriented Fast Mersenne Twister (dSFMT)
 * for Intel SSE2.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */

#include <emmintrin.h>
/*------------------------------------------
  128-bit SIMD like data type for SSE2
  ------------------------------------------*/

/** 128-bit data structure */
union W128_T {
    __m128i si;
    __m128d sd;
    uint64_t u[2];
    double d[2];
};

/** 128-bit data type */
typedef union W128_T w128_t;

/** the 128-bit internal state array */
static w128_t sfmt[N + 1];

static __m128i sse2_param_mask;
static __m128i sse2_low_mask;
static __m128i sse2_high_const;
static __m128i sse2_int_one;
static __m128d sse2_double_two;
static __m128d sse2_double_m_one;

static void setup_const(void);

inline static __m128i mm_recursion(__m128i *a, __m128i *b, __m128i c, __m128i d)
    __attribute__((always_inline));

/**
 * This function setup some constant variables for SSE2.
 */
static void setup_const(void) {
    static int first = 1;
    if (!first) {
	return;
    }
    sse2_param_mask = _mm_set_epi32(MSK32_3, MSK32_4, MSK32_1, MSK32_2);
    sse2_low_mask = _mm_set_epi32(LOW_MASK32_1, LOW_MASK32_2,
				  LOW_MASK32_1, LOW_MASK32_2);
    sse2_int_one = _mm_set_epi32(0, 1, 0, 1);
    sse2_high_const = _mm_set_epi32(HIGH_CONST32, 0, HIGH_CONST32, 0);
    sse2_double_two = _mm_set_pd(2.0L, 2.0L);
    sse2_double_m_one = _mm_set_pd(-1.0L, -1.0L);
    first = 0;
}

/**
 * This function represents the recursion formula.
 * @param a a 128-bit part of the internal state array
 * @param b a 128-bit part of the internal state array
 * @param c a 128-bit part of the internal state array
 * @param d a 128-bit part of the internal state array
 * @return next value of recursion
 */
inline static __m128i mm_recursion(__m128i *a, __m128i *b,
				   __m128i c, __m128i d) {
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
    /*v = _mm_or_si128(v, sse2_high_const);*/
    return v;
}

/**
 * This function converts the double precision floating point numbers which
 * distribute uniformly in the range [1, 2) to those which distribute uniformly
 * in the range [0, 1).
 * @param array array of double precision floating point numbers
 * @param size size of the array
 */
inline static void convert_co(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].sd = _mm_add_pd(array[i].sd, sse2_double_m_one);
    }
}

/**
 * This function converts the double precision floating point numbers which
 * distribute uniformly in the range [1, 2) to those which distribute uniformly
 * in the range (0, 1].
 * @param array array of double precision floating point numbers
 * @param size size of the array
 */
inline static void convert_oc(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].sd = _mm_sub_pd(sse2_double_two, array[i].sd);
    }
}

/**
 * This function converts the double precision floating point numbers which
 * distribute uniformly in the range [1, 2) to those which distribute uniformly
 * in the range (0, 1).
 * @param array array of double precision floating point numbers
 * @param size size of the array
 */
inline static void convert_oo(w128_t array[], int size) {
    int i;
    for (i = 0; i < size; i++) {
	array[i].si = _mm_or_si128(array[i].si, sse2_int_one);
	array[i].sd = _mm_add_pd(array[i].sd, sse2_double_m_one);
    }
}

/**
 * This function fills the internal state array with double precision
 * floating point pseudorandom numbers of the IEEE 754 format.
 */
inline static void gen_rand_all(void) {
    int i;
    __m128i r, lung;

    lung = _mm_load_si128(&sfmt[N].si);
    r = _mm_load_si128(&sfmt[N - 1].si);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&sfmt[i].si, &sfmt[i + POS1].si, r, lung);
	lung = _mm_xor_si128(lung, r);
	r = _mm_or_si128(r, sse2_high_const);
	_mm_store_si128(&sfmt[i].si, r);
    }
    for (; i < N; i++) {
	r = mm_recursion(&sfmt[i].si, &sfmt[i + POS1 - N].si, r, lung);
	lung = _mm_xor_si128(lung, r);
	r = _mm_or_si128(r, sse2_high_const);
	_mm_store_si128(&sfmt[i].si, r);
    }
    _mm_store_si128(&sfmt[N].si, lung);
}

/**
 * This function fills the user-specified array with double precision
 * floating point pseudorandom numbers of the IEEE 754 format.
 * @param array an 128-bit array to be filled by pseudorandom numbers.  
 * @param size number of 128-bit pseudorandom numbers to be generated.
 */
inline static void gen_rand_array(w128_t array[], int size) {
    int i, j;
    __m128i r, lung;

    lung = _mm_load_si128(&sfmt[N].si);
    r = _mm_load_si128(&sfmt[N - 1].si);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&sfmt[i].si, &sfmt[i + POS1].si, r, lung);
	lung = _mm_xor_si128(lung, r);
	r = _mm_or_si128(r, sse2_high_const);
	_mm_store_si128(&array[i].si, r);
    }
    for (; i < N; i++) {
	r = mm_recursion(&sfmt[i].si, &array[i + POS1 - N].si, r, lung);
	lung = _mm_xor_si128(lung, r);
	r = _mm_or_si128(r, sse2_high_const);
	_mm_store_si128(&array[i].si, r);
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = mm_recursion(&array[i - N].si, &array[i + POS1 - N].si, r, lung);
	lung = _mm_xor_si128(lung, r);
	r = _mm_or_si128(r, sse2_high_const);
	_mm_store_si128(&array[i].si, r);
    }
    for (j = 0; j < 2 * N - size; j++) {
	r = _mm_load_si128(&array[j + size - N].si);
	_mm_store_si128(&sfmt[j].si, r);
    }    
    for (; i < size; i++, j++) {
	r = mm_recursion(&array[i - N].si, &array[i + POS1 - N].si, r, lung);
	lung = _mm_xor_si128(lung, r);
	r = _mm_or_si128(r, sse2_high_const);
	_mm_store_si128(&array[i].si, r);
	_mm_store_si128(&sfmt[j].si, r);
    }
    _mm_store_si128(&sfmt[N].si, lung);
}
