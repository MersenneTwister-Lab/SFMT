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
    v = _mm_or_si128(v, sse2_high_const);
    return v;
}

inline static void convert_oc(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].sd = _mm_sub_pd(sse2_double_two, array[i].sd);
    }
}

inline static void convert_co(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].sd = _mm_add_pd(array[i].sd, sse2_double_m_one);
    }
}

inline static void convert_oo(w128_t array[], int size) {
    int i;
    for (i = 0; i < size; i++) {
	array[i].si = _mm_or_si128(array[i].si, sse2_int_one);
	array[i].sd = _mm_add_pd(array[i].sd, sse2_double_m_one);
    }
}

inline static void gen_rand_all(void) {
    int i;
    __m128i r, lung;

    lung = _mm_load_si128(&sfmt[N].si);
    r = _mm_load_si128(&sfmt[N - 1].si);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&sfmt[i].si, &sfmt[i + POS1].si, r, lung);
	_mm_store_si128(&sfmt[i].si, r);
	lung = _mm_xor_si128(lung, r);
    }
    for (; i < N; i++) {
	r = mm_recursion(&sfmt[i].si, &sfmt[i + POS1 - N].si, r, lung);
	_mm_store_si128(&sfmt[i].si, r);
	lung = _mm_xor_si128(lung, r);
    }
    _mm_store_si128(&sfmt[N].si, lung);
}

inline static void gen_rand_array(w128_t array[], int size) {
    int i, j;
    __m128i r, lung;

    lung = _mm_load_si128(&sfmt[N].si);
    r = _mm_load_si128(&sfmt[N - 1].si);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&sfmt[i].si, &sfmt[i + POS1].si, r, lung);
	_mm_store_si128(&array[i].si, r);
	lung = _mm_xor_si128(lung, r);
    }
    for (; i < N; i++) {
	r = mm_recursion(&sfmt[i].si, &array[i + POS1 - N].si, r, lung);
	_mm_store_si128(&array[i].si, r);
	lung = _mm_xor_si128(lung, r);
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = mm_recursion(&array[i - N].si, &array[i + POS1 - N].si, r, lung);
	_mm_store_si128(&array[i].si, r);
	lung = _mm_xor_si128(lung, r);
    }
    for (j = 0; j < 2 * N - size; j++) {
	r = _mm_load_si128(&array[j + size - N].si);
	_mm_store_si128(&sfmt[j].si, r);
    }    
    for (; i < size; i++, j++) {
	r = mm_recursion(&array[i - N].si, &array[i + POS1 - N].si, r, lung);
	_mm_store_si128(&array[i].si, r);
	_mm_store_si128(&sfmt[j].si, r);
	lung = _mm_xor_si128(lung, r);
    }
    _mm_store_si128(&sfmt[N].si, lung);
}
