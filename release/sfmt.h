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
 * uint32_t and uint64_t as follows:
 * @verbatim
 typedef unsigned int uint32_t
 typedef unsigned long long uint64_t  
@endverbatim
 *
 * uint32_t must be exactly 32-bit unsigned integer type (no more, no
 * less), and uint64_t must be exactly 64-bit unsigned integer type.
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

INLINE uint32_t gen_rand32(void);
INLINE uint64_t gen_rand64(void);
INLINE void fill_array32(uint32_t array[], int size);
INLINE void fill_array64(uint64_t array[], int size);
INLINE void init_gen_rand(uint32_t seed);
INLINE void init_by_array(uint32_t init_key[], int key_length);

#endif
