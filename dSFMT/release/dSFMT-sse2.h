/** 
 * @file dSFMT-sse2.h 
 *
 * @brief double precision SIMD oriented Fast Mersenne Twister(dSFMT)
 * pseudorandom number generator
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software.
 * see LICENSE.txt
 */

#ifndef DSFMT_SSE2_H
#define DSFMT_SSE2_H
#include <emmintrin.h>

union W128_T {
    __m128i si;
    __m128d sd;
    uint64_t u[2];
    double d[2];
};

static __m128i sse2_param_mask;
static __m128i sse2_low_mask;
static __m128i sse2_high_const;
static __m128i sse2_int_one;
static __m128d sse2_double_two;
static __m128d sse2_double_m_one;

static void setup_const(void);

inline static __m128i mm_recursion(__m128i *a, __m128i *b, __m128i c, __m128i d)
    __attribute__((always_inline));
#endif
