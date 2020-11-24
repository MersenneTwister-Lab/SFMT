#pragma once
/**
 * @file  SFMT-avx256.h
 * @brief SFMT(SIMD oriented Fast Mersenne Twister) AVX2/AVX512VL
 * @author Masaki Ota (GitHub: @magurosan
 * @note referenced from SFMT-sse2.h 
 *  asssumed: 
 *    SFMT_SL1 >= 16
 *    SFMT_N % 2 == 0 
 *    SFMT_POS1 % 2 == 0
 * 
 * Copyright (C) 2020. Masaki Ota. All rights reserved.
 * 
 * The new BSD License is applied to this software, see LICENSE.txt
 */
#ifndef SFMT_AVX256_H
#define SFMT_AVX256_H

#define LD2(addr) _mm256_loadu_si256((__m256i*)addr)
#define ST2(addr, x) _mm256_storeu_si256((__m256i*)addr, x)
#define ST1(addr, x) _mm_store_si128((__m128i*)addr, _mm256_castsi256_si128(x))

/**
 * This function represents the recursion formula.
 * @param a 256-bit part of the interal state array 
 * @param b 256-bit part of the interal state array
 * @param c 256-bit part of the interal state array
 * @return new value
 */
inline static __m256i mm256_recursion(__m256i a, __m256i b, __m256i c)
{
    __m256i x, y, z, w;
    __m256i mask = _mm256_broadcastsi128_si256(sse2_param_mask.si);

    x = _mm256_slli_si256(a, SFMT_SL2);
    y = _mm256_srli_epi32(b, SFMT_SR1);

#if defined(__AVX512VL__)
    x = _mm256_ternarylogic_epi32(x, y, mask, 0x78); // A ^ (B & C)
#else
    y = _mm256_and_si256(y, mask); 
    x = _mm256_xor_si256(x, y);
#endif

    z = _mm256_srli_si256(c, SFMT_SR2);

#if defined(__AVX512VL__)
    x = _mm256_ternarylogic_epi32(x, a, z, 0x96); // XOR3(A,B,C)
#else
    x = _mm256_xor_si256(x, a);
    x = _mm256_xor_si256(x, z);
#endif

    w = _mm256_permute2f128_si256(c, x, 0x21); 	
    w = _mm256_slli_epi32(w, SFMT_SL1); 
    x = _mm256_xor_si256(x, w);
    return x;
}

/**
 * This function fills the internal state array with pseudorandom
 * integers.
 * @param sfmt SFMT internal state
 */
void sfmt_gen_rand_all(sfmt_t * sfmt) {
    int i;
    __m256i r;
    w128_t * pstate = sfmt->state;

    r = LD2(&pstate[SFMT_N - 2]); 
    for (i = 0; i < SFMT_N - SFMT_POS1; i+=2) {
        r = mm256_recursion(LD2(&pstate[i]), LD2(&pstate[i + SFMT_POS1]), r);
        ST2(&pstate[i], r);
    }
    for (; i < SFMT_N; i+=2) {
        r = mm256_recursion(LD2(&pstate[i]), LD2(&pstate[i + SFMT_POS1 - SFMT_N]), r);
        ST2(&pstate[i], r);
    }
}

/**
 * This function fills the user-specified array with pseudorandom
 * integers.
 * @param sfmt SFMT internal state.
 * @param array an 128-bit array to be filled by pseudorandom numbers.
 * @param size number of 128-bit pseudorandom numbers to be generated.
 */
static void gen_rand_array(sfmt_t * sfmt, w128_t * array, int size)
{
    int i, j;
    __m256i r;
    w128_t * pstate = sfmt->state;

    r = LD2(&pstate[SFMT_N - 2]); 
    for (i = 0; i < SFMT_N - SFMT_POS1; i+=2) {
        r = mm256_recursion(LD2(&pstate[i]), LD2(&pstate[i + SFMT_POS1]), r);
        ST2(&array[i], r);
    }
    for (; i < SFMT_N; i+=2) {
        r = mm256_recursion(LD2(&pstate[i]), LD2(&array[i + SFMT_POS1 - SFMT_N]), r);
        ST2(&array[i], r);
    }
    for (; i < size - SFMT_N; i+=2) {
        r = mm256_recursion(LD2(&array[i - SFMT_N]), LD2(&array[i + SFMT_POS1 - SFMT_N]), r);
        ST2(&array[i], r);
    }
    for (j = 0; j < 2 * SFMT_N - size; j += 2) {
        r = LD2(&array[j + size - SFMT_N]);
        ST2(&pstate[j], r);
    }
    for (; i < size - 1; i+=2, j+=2) {
        r = mm256_recursion(LD2(&array[i - SFMT_N]), LD2(&array[i + SFMT_POS1 - SFMT_N]), r);
        ST2(&array[i], r);
        ST2(&pstate[j], r);
    }
    if (size & 1) {
        r = mm256_recursion(LD2(&array[i - SFMT_N]), LD2(&array[i + SFMT_POS1 - SFMT_N]), r);
        ST1(&array[i], r);
        ST1(&pstate[j], r);
    }
}
#endif 
