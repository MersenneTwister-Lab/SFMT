/** 
 * @file  sfmt19937-sse2.c
 * @brief SIMD oriented Fast Mersenne Twister(SFMT) for intel SSE2
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * @date 2006-09-07
 *
 * @note We assume LITTLE ENDIAN in this file
 *
 * Copyright (C) 2006 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */

#include <string.h>
#include <assert.h>
#include <emmintrin.h>
#include "sfmt19937.h"

/*-----------------
  BASIC DEFINITIONS
  -----------------*/
/** Mersenne Exponent. The period of the sequence 
 *  is a multiple of 2^MEXP-1. */
#define MEXP 19937
/** the word size of the recursion of SFMT is 128-bit. */
#define WORDSIZE 128
/** SFMT generator has an internal state array of 128-bit integers,
 * and N is its size. */
#define N (MEXP / WORDSIZE + 1)
/** N32 is the size of internal state array when regarded as an array
 * of 32-bit integers.*/
#define N32 (N * 4)
/** N64 is the size of internal state array when regarded as an array
 * of 64-bit integers.*/
#define N64 (N * 2)

/*----------------------
  the parameters of SFMT
  ----------------------*/
/** the pick up position of the array. */
#define POS1 122
/** the parameter of shift left as four 32-bit registers. */
#define SL1 18
/** the parameter of shift left as one 128-bit register. 
 * The 128-bit integer is shifted by (SL2 * 8) bits. 
 */
#define SL2 1
/** the parameter of shift right as four 32-bit registers. */
#define SR1 11
/** the parameter of shift right as one 128-bit register. 
 * The 128-bit integer is shifted by (SL2 * 8) bits. 
 */
#define SR2 1
/** A bitmask, used in the recursion.  These parameters are introduced
 * to break symmetry of SIMD.*/
#define MSK1 0xdfffffefU
/** A bitmask, used in the recursion.  These parameters are introduced
 * to break symmetry of SIMD.*/
#define MSK2 0xddfecb7fU
/** A bitmask, used in the recursion.  These parameters are introduced
 * to break symmetry of SIMD.*/
#define MSK3 0xbffaffffU
/** A bitmask, used in the recursion.  These parameters are introduced
 * to break symmetry of SIMD.*/
#define MSK4 0xbffffff6U
/** The 32 MSBs of the internal state array is seto to this
 * value. This peculiar value assures that the period length of the
 * output sequence is a multiple of 2^19937-1.
 */
#define INIT_LUNG 0x6d736d6dU

/*--------------------------------------
  FILE GLOBAL VARIABLES
  internal state, index counter and flag 
  --------------------------------------*/
/** the 128-bit internal state array */
static __m128i sfmt[N];
/** the 32bit interger pointer to the 128-bit internal state array */
static uint32_t *psfmt32 = (uint32_t *)&sfmt[0];
/** the 64bit interger pointer to the 128-bit internal state array */
static uint64_t *psfmt64 = (uint64_t *)&sfmt[0];
/** index counter to the 32-bit internal state array */
static int idx;
/** a flag: it is 0 if and only if the internal state is not yet
 * initialized. */
static int initialized = 0;

/*----------------
  STATIC FUNCTIONS
  ----------------*/
INLINE static __m128i mm_recursion(__m128i *a, __m128i *b, 
				   __m128i c, __m128i d, __m128i mask);
INLINE static void gen_rand_all(void);
INLINE static void gen_rand_array(__m128i array[], int size);
INLINE static uint32_t func1(uint32_t x);
INLINE static uint32_t func2(uint32_t x);

/**
 * This function represents the recursion formula.
 * @param a a 128-bit part of the interal state array
 * @param b a 128-bit part of the interal state array
 * @param c a 128-bit part of the interal state array
 * @param d a 128-bit part of the interal state array
 * @param mask 128-bit mask
 * @return output
 */
INLINE static 
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    __m128i mm_recursion(__m128i *a, __m128i *b, 
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
 * This function fills the internal state array with psedorandom
 * integers.
 */
INLINE void gen_rand_all(void) {
    int i;
    __m128i r, r1, r2, mask;
    mask = _mm_set_epi32(MSK4, MSK3, MSK2, MSK1);

    r1 = _mm_load_si128(&sfmt[N - 2]);
    r2 = _mm_load_si128(&sfmt[N - 1]);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1], r1, r2, mask);
	_mm_store_si128(&sfmt[i], r);
	r1 = r2;
	r2 = r;
    }
    for (; i < N; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1 - N], r1, r2, mask);
	_mm_store_si128(&sfmt[i], r);
	r1 = r2;
	r2 = r;
    }
}

/**
 * This function fills the user-specified array with psedorandom
 * integers.
 *
 * @param array an 128-bit array to be filled by pseudorandom numbers.  
 * @param size number of 128-bit pesudorandom numbers to be generated.
 */
INLINE static void gen_rand_array(__m128i array[], int size) {
    int i, j;
    __m128i r, r1, r2, mask;
    mask = _mm_set_epi32(MSK4, MSK3, MSK2, MSK1);

    r1 = _mm_load_si128(&sfmt[N - 2]);
    r2 = _mm_load_si128(&sfmt[N - 1]);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1], r1, r2, mask);
	_mm_store_si128(&array[i], r);
	r1 = r2;
	r2 = r;
    }
    for (; i < N; i++) {
	r = mm_recursion(&sfmt[i], &array[i + POS1 - N], r1, r2, mask);
	_mm_store_si128(&array[i], r);
	r1 = r2;
	r2 = r;
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = mm_recursion(&array[i - N], &array[i + POS1 - N], r1, r2, mask);
	_mm_store_si128(&array[i], r);
	r1 = r2;
	r2 = r;
    }
    for (j = 0; j < 2 * N - size; j++) {
	r = _mm_load_si128(&array[j + size - N]);
	_mm_store_si128(&sfmt[j], r);
    }
    for (; i < size; i++) {
	r = mm_recursion(&array[i - N], &array[i + POS1 - N], r1, r2, mask);
	_mm_store_si128(&array[i], r);
	_mm_store_si128(&sfmt[j++], r);
	r1 = r2;
	r2 = r;
    }
}

/**
 * This function represents a function used in the initialization
 * by \b init_by_array
 * @param x 32-bit integer
 * @return 32-bit integer
 */
INLINE static uint32_t func1(uint32_t x) {
    return (x ^ (x >> 27)) * (uint32_t)1664525UL;
}

/**
 * This function represents a function used in the initialization
 * by \b init_by_array
 * @param x 32-bit integer
 * @return 32-bit integer
 */
INLINE static uint32_t func2(uint32_t x) {
    return (x ^ (x >> 27)) * (uint32_t)1566083941UL;
}

/*----------------
  PUBLIC FUNCTIONS
  ----------------*/
/**
 * This function generates and returns 32-bit pseudorandom number.  \b
 * init_gen_rand or \b init_by_array must be called before this
 * function.
 *
 * @return 32-bit pseudorandom number
 */
INLINE uint32_t gen_rand32(void)
{
    uint32_t r;

    if (idx >= N32) {
	gen_rand_all();
	idx = 0;
    }
    r = psfmt32[idx++];
    return r;
}

/**
 * This function generates and returns 64-bit pseudorandom number.  \b
 * init_gen_rand or \b init_by_array must be called before this
 * function.  The function gen_rand64 should not be called after
 * gen_rand32, unless an initialization is again executed.
 *
 * @return 64-bit pseudorandom number
 */
INLINE uint64_t gen_rand64(void)
{
    uint64_t r;

    assert(initialized);
    assert(idx % 2 == 0);

    if (idx >= N32) {
	gen_rand_all();
	idx = 0;
    }
    r = psfmt64[idx / 2];
    idx += 2;
    return r;
}

/**
 * This function generates pseudorandom 32-bit integers in the
 * specified array[] by one call. The number of pseudorandom integers
 * is specified by the argument size, which must be at least 624 and a
 * multiple of four.  The generation by this function is much faster
 * than the following gen_rand function.
 *
 * For initialization, \b init_gen_rand or \b init_by_array must be
 * called before the first call of this function. This function can
 * not be used after calling gen_rand function, without
 * initialization.
 *
 * @param array an array where pseudorandom 32-bit integers are filled
 * by this function.  The pointer to the array must be "aligned"
 * (namely, must be a multiple of 16) in the SIMD version, since it
 * refers to the address of a 128-bit integer.  In the standard C
 * version, the pointer is arbitrary.
 *
 * @param size the number of 32-bit pseudorandom integers to be
 * generated.  size must be a multiple of 4, and greater than or equal
 * to 624.
 */
INLINE void fill_array32(uint32_t array[], int size)
{
    assert(initialized);
    assert((uint64_t)array % 16 == 0);
    assert(idx == N32);
    assert(size % 4 == 0);
    assert(size >= N32);

    gen_rand_array((__m128i *)array, size / 4);
    idx = N32;
}

/**
 * This function generates pseudorandom 64-bit integers in the
 * specified array[] by one call. The number of pseudorandom integers
 * is specified by the argument size, which must be at least 312 and a
 * multiple of two.  The generation by this function is much faster
 * than the following gen_rand function.
 *
 * For initialization, \b init_gen_rand or \b init_by_array must be
 * called before the first call of this function. This function can
 * not be used after calling gen_rand function, without
 * initialization.
 *
 * @param array an array where pseudorandom 64-bit integers are filled
 * by this function.  The pointer to the array must be "aligned"
 * (namely, must be a multiple of 16) in the SIMD version, since it
 * refers to the address of a 128-bit integer.  In the standard C
 * version, the pointer is arbitrary.
 *
 * @param size the number of 64-bit pseudorandom integers to be
 * generated.  size must be a multiple of 2, and greater than or equal
 * to 312.
 */
INLINE void fill_array64(uint64_t array[], int size)
{
    assert(initialized);
    assert((uint64_t)array % 16 == 0);
    assert(idx == N32);
    assert(size % 2 == 0);
    assert(size >= N64);

    gen_rand_array((__m128i *)array, size / 2);
    memcpy(psfmt64, array + size - N64, sizeof(uint64_t) * N64);
    idx = N32;
}

/**
 * This function initializes the internal state array with a 32-bit
 * integer seed.
 *
 * @param seed a 32-bit integer used as the seed.
 */
void init_gen_rand(uint32_t seed)
{
    int i;

    psfmt32[0] = seed;
    for (i = 1; i < N32; i++) {
	psfmt32[i] = 1812433253UL * (psfmt32[i - 1] ^ (psfmt32[i - 1] >> 30))
	    + i;
    }
    psfmt32[3] = INIT_LUNG;
    idx = N32;
    initialized = 1;
}

/**
 * This function initializes the internal state array,
 * with an array of 32-bit integers used as the seeds
 * @param init_key the array of 32-bit integers, used as a seed.
 * @param key_length the length of init_key.
 */
void init_by_array(uint32_t init_key[], int key_length) {
    int i, j, count;
    uint32_t r;
    const int MID = 306;
    const int LAG = 11;

    memset(sfmt, 0x8b, sizeof(sfmt));
    if (key_length + 1 > N32) {
	count = key_length + 1;
    } else {
	count = N32;
    }
    r = func1(psfmt32[0] ^ psfmt32[MID] ^ psfmt32[N32 - 1]);
    psfmt32[MID] += r;
    r += key_length;
    psfmt32[MID + LAG] += r;
    psfmt32[0] = r;
    i = 1;
    count--;
    for (i = 1, j = 0; (j < count) && (j < key_length); j++) {
	r = func1(psfmt32[i] ^ psfmt32[(i + MID) % N32] 
		  ^ psfmt32[(i + N32 - 1) % N32]);
	psfmt32[(i + MID) % N32] += r;
	r += init_key[j] + i;
	psfmt32[(i + MID + LAG) % N32] += r;
	psfmt32[i] = r;
	i = (i + 1) % N32;
    }
    for (; j < count; j++) {
	r = func1(psfmt32[i] ^ psfmt32[(i + MID) % N32] 
		  ^ psfmt32[(i + N32 - 1) % N32]);
	psfmt32[(i + MID) % N32] += r;
	r += i;
	psfmt32[(i + MID + LAG) % N32] += r;
	psfmt32[i] = r;
	i = (i + 1) % N32;
    }
    for (j = 0; j < N32; j++) {
	r = func2(psfmt32[i] + psfmt32[(i + MID) % N32] 
		  + psfmt32[(i + N32 - 1) % N32]);
	psfmt32[(i + MID) % N32] ^= r;
	r -= i;
	psfmt32[(i + MID + LAG) % N32] ^= r;
	psfmt32[i] = r;
	i = (i + 1) % N32;
    }

    psfmt32[3] = INIT_LUNG;
    idx = N32;
    initialized = 1;
}
