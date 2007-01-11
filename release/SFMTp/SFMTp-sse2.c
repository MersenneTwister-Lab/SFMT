/** 
 * @file sfmt19937-sse2.c 
 * @brief SIMD oriented Fast Mersenne Twister Pulmonary version
 * (SFMTp) for intel SSE2.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * @date 2007-01-11
 *
 * @note We assume LITTLE ENDIAN in this file
 *
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */

#include <string.h>
#include <assert.h>
#include <emmintrin.h>
#include "SFMTp.h"
#include "SFMTp-params.h"
/** permutation */
#define SHUFF 0x4B

/*--------------------------------------
  FILE GLOBAL VARIABLES
  internal state, index counter and flag 
  --------------------------------------*/
/** the 128-bit internal state array */
static __m128i sfmt[N + 1];
/** the 32bit interger pointer to the 128-bit internal state array */
static uint32_t *psfmt32 = (uint32_t *)&sfmt[0];
/** the 64bit interger pointer to the 128-bit internal state array */
static uint64_t *psfmt64 = (uint64_t *)&sfmt[0];
/** index counter to the 32-bit internal state array */
static int idx;
/** a flag: it is 0 if and only if the internal state is not yet
 * initialized. */
static int initialized = 0;
/** a parity check vector which certificate the period of 2^{MEXP} */
static uint32_t parity[4] = {PARITY1, PARITY2, PARITY3, PARITY4};

/*----------------
  STATIC FUNCTIONS
  ----------------*/
INLINE static __m128i mm_recursion(__m128i *a, __m128i *b, 
				   __m128i c, __m128i d, __m128i mask);
INLINE static void gen_rand_all(void);
INLINE static void gen_rand_array(__m128i array[], int size);
INLINE static uint32_t func1(uint32_t x);
INLINE static uint32_t func2(uint32_t x);
static void period_certification(void);

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
    
    y = _mm_srli_epi32(*b, SR1);
    y = _mm_and_si128(y, mask);
    z = _mm_srli_epi32(c, SR2);
    v = _mm_slli_epi32(c, SL1);
    z = _mm_xor_si128(z, v);
    v = _mm_shuffle_epi32(d, SHUFF);
    v = _mm_xor_si128(v, *a);
    x = _mm_slli_si128(*a, SL2);
    x = _mm_xor_si128(x, y);
    z = _mm_xor_si128(z, v);
    z = _mm_xor_si128(z, x);
    return z;
}

/**
 * This function fills the internal state array with psedorandom
 * integers.
 */
INLINE static void gen_rand_all(void) {
    int i;
    __m128i r, u, mask;
    mask = _mm_set_epi32(MSK4, MSK3, MSK2, MSK1);

    r = _mm_load_si128(&sfmt[N - 1]);
    u = _mm_load_si128(&sfmt[N]);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1], r, u, mask);
	_mm_store_si128(&sfmt[i], r);
	u =_mm_xor_si128(u, r);
    }
    for (; i < N; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1 - N], r, u, mask);
	_mm_store_si128(&sfmt[i], r);
	u =_mm_xor_si128(u, r);
    }
    _mm_store_si128(&sfmt[N], u);
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
    __m128i r, u, mask;
    mask = _mm_set_epi32(MSK4, MSK3, MSK2, MSK1);

    u = _mm_load_si128(&sfmt[N]);
    r = _mm_load_si128(&sfmt[N - 1]);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1], r, u, mask);
	_mm_store_si128(&array[i], r);
	u =_mm_xor_si128(u, r);
    }
    for (; i < N; i++) {
	r = mm_recursion(&sfmt[i], &array[i + POS1 - N], r, u, mask);
	_mm_store_si128(&array[i], r);
	u = _mm_xor_si128(u, r);
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = mm_recursion(&array[i - N], &array[i + POS1 - N], r, u, mask);
	_mm_store_si128(&array[i], r);
	u = _mm_xor_si128(u, r);
    }
    for (j = 0; j < 2 * N - size; j++) {
	r = _mm_load_si128(&array[j + size - N]);
	_mm_store_si128(&sfmt[j], r);
    }
    for (; i < size; i++, j++) {
	r = mm_recursion(&array[i - N], &array[i + POS1 - N], r, u, mask);
	_mm_store_si128(&array[i], r);
	_mm_store_si128(&sfmt[j], r);
	u = _mm_xor_si128(u, r);
    }
    _mm_store_si128(&sfmt[N], u);
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

/**
 * This function certificate the period of 2^{MEXP}
 */
static void period_certification(void) {
    int inner = 0;
    int i, j;
    uint32_t work;
    uint32_t *plung = (uint32_t *)&sfmt[N];

    for (i = 0; i < 4; i++) {
	work = plung[i] & parity[i];
	for (j = 0; j < 32; j++) {
	    inner ^= work & 1;
	    work = work >> 1;
	}
    }
    /* check OK */
    if (inner == 1) {
	return;
    }
    /* check NG, and modification */
    for (i = 0; i < 4; i++) {
	work = 1;
	for (j = 0; j < 32; j++) {
	    if ((work & parity[i]) != 0) {
		plung[i] ^= work;
		return;
	    }
	    work = work << 1;
	}
    }
}
/*----------------
  PUBLIC FUNCTIONS
  ----------------*/
/**
 * This function returns the identification string.
 * The string shows the word size, the mersenne expornent,
 * and all parameters of this generator.
 */
char *get_idstring(void)
{
    return IDSTR;
}

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
    for (i = 1; i < (N + 1) * 4; i++) {
	psfmt32[i] = 1812433253UL * (psfmt32[i - 1] ^ (psfmt32[i - 1] >> 30))
	    + i;
    }
    idx = N32;
    period_certification();
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
    int lag;
    int mid;
    int size = (N + 1) * 4;	/* pulmonary */

    if (size >= 623) {
	lag = 11;
    } else if (size >= 68) {
	lag = 7;
    } else if (size >= 39) {
	lag = 5;
    } else {
	lag = 3;
    }
    mid = (size - lag) / 2;

    memset(sfmt, 0x8b, sizeof(sfmt));
    if (key_length + 1 > size) {
	count = key_length + 1;
    } else {
	count = size;
    }
    r = func1(psfmt32[0] ^ psfmt32[mid % size] ^ psfmt32[size - 1]);
    psfmt32[mid % size] += r;
    r += key_length;
    psfmt32[(mid + lag) % size] += r;
    psfmt32[0] = r;
    count--;
    for (i = 1, j = 0; (j < count) && (j < key_length); j++) {
	r = func1(psfmt32[i] ^ psfmt32[(i + mid) % size] 
		  ^ psfmt32[(i + size - 1) % size]);
	psfmt32[(i + mid) % size] += r;
	r += init_key[j] + i;
	psfmt32[(i + mid + lag) % size] += r;
	psfmt32[i] = r;
	i = (i + 1) % size;
    }
    for (; j < count; j++) {
	r = func1(psfmt32[i] ^ psfmt32[(i + mid) % size] 
		  ^ psfmt32[(i + size - 1) % size]);
	psfmt32[(i + mid) % size] += r;
	r += i;
	psfmt32[(i + mid + lag) % size] += r;
	psfmt32[i] = r;
	i = (i + 1) % size;
    }
    for (j = 0; j < size; j++) {
	r = func2(psfmt32[i] + psfmt32[(i + mid) % size] 
		  + psfmt32[(i + size - 1) % size]);
	psfmt32[(i + mid) % size] ^= r;
	r -= i;
	psfmt32[(i + mid + lag) % size] ^= r;
	psfmt32[i] = r;
	i = (i + 1) % size;
    }

    idx = N32;
    period_certification();
    initialized = 1;
}
