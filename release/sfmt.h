/** 
 * @file sfmt.h 
 *
 * @brief SIMD oriented Fast Mersenne Twister(SFMT) pseudorandom
 * number generator
 *
 * @author Mutsuo Saito (saito@math.sci.hiroshima-u.ac.jp) Hiroshima
 * University 
 * @author Makoto Matsumoto
 * (m-mat@math.sci.hiroshima-u.ac.jp) Hiroshima University
 *
 * @date 2006-08-29
 *
 * Copyright (C) 2006 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * @note We assume that your system has inttypes.h.
 * If your system doesn't have inttypes.h, you have to typedef
 * uint32_t and uint64_t like this:
 * @verbatim
 typedef unsigned int uint32_t
 typedef unsigned long long uint64_t  @endverbatim
 * uint32_t must be just 32 bit unsigned integer type and
 * uint64_t must be just 64 bit unsigned integer type.
 */

#ifndef __SFMT_H__
#define __SFMT_H__

#include <stdio.h>

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
  #include <inttypes.h>
  #define INLINE inline
#else
  #include <inttypes.h>
  #if defined(__GNUC__)
    #define INLINE __inline__
  #endif
#endif

INLINE void init_gen_rand(uint32_t seed);
INLINE uint32_t gen_rand(void);
INLINE void fill_array(uint32_t array[], int size);
INLINE void init_by_array(uint32_t init_key[], int key_length);

#endif
