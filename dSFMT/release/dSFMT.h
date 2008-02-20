/** 
 * @file dSFMT.h 
 *
 * @brief double precision SIMD oriented Fast Mersenne Twister(dSFMT)
 * pseudorandom number generator based on IEEE 754 format.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
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

#ifndef DSFMT_H
#define DSFMT_H

#include <stdio.h>

#if defined(__BYTE_ORDER) && defined(__BIG_ENDIAN)
  #if __BYTE_ORDER != __BIG_ENDIAN
    #define NOT_BIG_ENDIAN 1
  #endif
#elif defined(__BYTE_ORDER__) && defined(__BIG_ENDIAN__)
  #if __BYTE_ORDER__ != __BIG_ENDIAN__
    #define NOT_BIG_ENDIAN 1
  #endif
#elif defined(BYTE_ORDER) && defined(BIG_ENDIAN)
  #if BYTE_ORDER != BIG_ENDIAN
    #define NOT_BIG_ENDIAN 1
  #endif
#endif

#if !defined(NOT_BIG_ENDIAN) && defined(__amd64)
  #define NOT_BIG_ENDIAN 1
#endif

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
  #include <inttypes.h>
#elif defined(_MSC_VER) || defined(__BORLANDC__)
  typedef unsigned int uint32_t;
  typedef unsigned long long uint64_t;
  #define inline __inline
#else
  #include <inttypes.h>
  #if defined(__GNUC__)
    #define inline __inline__
  #else
    #define inline
  #endif
#endif

#ifndef PRIu64
  #if defined(_MSC_VER) || defined(__BORLANDC__)
    #define PRIu64 "I64u"
    #define PRIx64 "I64x"
  #else
    #define PRIu64 "llu"
    #define PRIx64 "llx"
  #endif
#endif

#ifndef UINT64_C
  #define UINT64_C(v) (v ## ULL) 
#endif

double genrand_close1_open2(void);
#if defined(__GNUC__)
inline static double genrand_close_open(void) __attribute__((always_inline));
inline static double genrand_open_close(void) __attribute__((always_inline));
inline static double genrand_open_open(void) __attribute__((always_inline));
#elif defined(_MSC_VER) && _MSC_VER >= 1200
__forceinline static double genrand_close_open(void);
__forceinline static double genrand_open_close(void);
__forceinline static double genrand_open_open(void);
#else
inline static double genrand_close_open(void);
inline static double genrand_open_close(void);
inline static double genrand_open_open(void);
#endif

void fill_array_open_close(double array[], int size);
void fill_array_close_open(double array[], int size);
void fill_array_open_open(double array[], int size);
void fill_array_close1_open2(double array[], int size);
const char *get_idstring(void);
int get_min_array_size(void);
void init_gen_rand(uint32_t seed);
void init_by_array(uint32_t init_key[], int key_length);

/**
 * This function generates and returns double precision pseudorandom
 * number which distributes uniformly in the range [0, 1).
 * init_gen_rand() or init_by_array() must be called before this
 * function.  
 * @return double precision floating point pseudorandom number
 */
inline static double genrand_close_open(void) {
    return genrand_close1_open2() - 1.0;
}

/**
 * This function generates and returns double precision pseudorandom
 * number which distributes uniformly in the range (0, 1].
 * init_gen_rand() or init_by_array() must be called before this
 * function.
 * @return double precision floating point pseudorandom number
 */
inline static double genrand_open_close(void) {
    return 2.0 - genrand_close1_open2();
}

/**
 * This function generates and returns double precision pseudorandom
 * number which distributes uniformly in the range (0, 1).
 * init_gen_rand() or init_by_array() must be called before this
 * function.
 * @return double precision floating point pseudorandom number
 */
inline static double genrand_open_open(void) {
    union {
	uint64_t u;
	double d;
    } conv;

    conv.d = genrand_close1_open2();
    conv.u |= 1;
    return conv.d - 1.0;
}

#endif /* DSFMT_H */
