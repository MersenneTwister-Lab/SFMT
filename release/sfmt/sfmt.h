/** 
 * @file sfmt.h 
 *
 * @brief SIMD oriented Fast Mersenne Twister(SFMT) pseudorandom
 * number generator
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * @date 2006-08-29
 *
 * Copyright (C) 2006 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software.
 * see LICENSE.txt
 *
 * @note We assume that your system has inttypes.h.  If your system
 * doesn't have inttypes.h, you have to typedef uint32_t and uint64_t,
 * and you have to define PRIu64 and PRIx64 in this file as follows:
 * @verbatim
 typedef unsigned int uint32_t
 typedef unsigned long long uint64_t  
 #define PRIu64 "llu"
 #define PRIx64 "llx"
@endverbatim
 * uint32_t must be exactly 32-bit unsigned integer type (no more, no
 * less), and uint64_t must be exactly 64-bit unsigned integer type.
 * PRIu64 and PRIx64 are used for printf function to print 64-bit
 * unsigned int and 64-bit unsigned int in hexadecimal format.
 */

#ifndef SFMT19937_H
#define SFMT19937_H

#include <stdio.h>

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
  #include <inttypes.h>
  #define INLINE inline
#elif defined(_MSC_VER)
  typedef unsigned int uint32_t;
  typedef unsigned long long uint64_t;
  #define INLINE
#else
  #include <inttypes.h>
  #if defined(__GNUC__)
    #define INLINE __inline__
  #endif
#endif

#ifndef PRIu64
  #if defined(_MSC_VER)
    #define PRIu64 "I64u"
    #define PRIx64 "I64x"
  #else
    #define PRIu64 "qu"
    #define PRIx64 "qx"
  #endif
#endif

INLINE uint32_t gen_rand32(void);
INLINE uint64_t gen_rand64(void);
INLINE void fill_array32(uint32_t array[], int size);
INLINE void fill_array64(uint64_t array[], int size);
void init_gen_rand(uint32_t seed);
void init_by_array(uint32_t init_key[], int key_length);

#endif
