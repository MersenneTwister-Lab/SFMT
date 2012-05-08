/**
 * @file  SFMTst-sse2.h
 * @brief SIMD oriented Fast Mersenne Twister(SFMT) for Intel SSE2
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * @note We assume LITTLE ENDIAN in this file
 *
 * Copyright (C) 2012 Mutsuo Saito, Makoto Matsumoto, Hiroshima
 * University and The University of Tokyo.
 * All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */

#ifndef SFMTST_SSE2_H
#define SFMTST_SSE2_H

PRE_ALWAYS static __m128i mm_recursion(__m128i *a, __m128i *b, __m128i c,
				   __m128i d, __m128i mask) ALWAYSINLINE;

/**
 * This function represents the recursion formula.
 * @param a a 128-bit part of the interal state array
 * @param b a 128-bit part of the interal state array
 * @param c a 128-bit part of the interal state array
 * @param d a 128-bit part of the interal state array
 * @param mask 128-bit mask
 * @return output
 */
PRE_ALWAYS static __m128i mm_recursion(__m128i *a, __m128i *b,
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

/**
 * This function fills the internal state array with pseudorandom
 * integers.
 */
inline static void gen_rand_all(sfmt_t * sfmt) {
    int i;
    __m128i r, r1, r2, mask;
    mask = _mm_set_epi32(SFMT_MSK4, SFMT_MSK3, SFMT_MSK2, SFMT_MSK1);

    r1 = _mm_load_si128(&sfmt[SFMT_N - 2].si);
    r2 = _mm_load_si128(&sfmt[SFMT_N - 1].si);
    for (i = 0; i < SFMT_N - SFMT_POS1; i++) {
	r = mm_recursion(&sfmt[i].si, &sfmt[i + SFMT_POS1].si, r1, r2, mask);
	_mm_store_si128(&sfmt[i].si, r);
	r1 = r2;
	r2 = r;
    }
    for (; i < SFMT_N; i++) {
	r = mm_recursion(&sfmt[i].si, &sfmt[i + SFMT_POS1 - SFMT_N].si, r1, r2, mask);
	_mm_store_si128(&sfmt[i].si, r);
	r1 = r2;
	r2 = r;
    }
}

/**
 * This function fills the user-specified array with pseudorandom
 * integers.
 *
 * @param array an 128-bit array to be filled by pseudorandom numbers.
 * @param size number of 128-bit pesudorandom numbers to be generated.
 */
inline static void gen_rand_array(w128_t *array, int size, sfmt_t * sfmt) {
    int i, j;
    __m128i r, r1, r2, mask;
    mask = _mm_set_epi32(SFMT_MSK4, SFMT_MSK3, SFMT_MSK2, SFMT_MSK1);

    r1 = _mm_load_si128(&sfmt[SFMT_N - 2].si);
    r2 = _mm_load_si128(&sfmt[SFMT_N - 1].si);
    for (i = 0; i < SFMT_N - SFMT_POS1; i++) {
	r = mm_recursion(&sfmt[i].si, &sfmt[i + SFMT_POS1].si, r1, r2, mask);
	_mm_store_si128(&array[i].si, r);
	r1 = r2;
	r2 = r;
    }
    for (; i < SFMT_N; i++) {
	r = mm_recursion(&sfmt[i].si,
			 &array[i + SFMT_POS1 - SFMT_N].si,
			 r1, r2, mask);
	_mm_store_si128(&array[i].si, r);
	r1 = r2;
	r2 = r;
    }
    /* main loop */
    for (; i < size - SFMT_N; i++) {
	r = mm_recursion(&array[i - SFMT_N].si,
			 &array[i + SFMT_POS1 - SFMT_N].si,
			 r1, r2, mask);
	_mm_store_si128(&array[i].si, r);
	r1 = r2;
	r2 = r;
    }
    for (j = 0; j < 2 * SFMT_N - size; j++) {
	r = _mm_load_si128(&array[j + size - SFMT_N].si);
	_mm_store_si128(&sfmt[j].si, r);
    }
    for (; i < size; i++) {
	r = mm_recursion(&array[i - SFMT_N].si,
			 &array[i + SFMT_POS1 - SFMT_N].si,
			 r1, r2, mask);
	_mm_store_si128(&array[i].si, r);
	_mm_store_si128(&sfmt[j++].si, r);
	r1 = r2;
	r2 = r;
    }
}

#endif
