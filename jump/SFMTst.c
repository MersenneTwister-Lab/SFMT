/**
 * @file SFMTst.c
 *
 * @brief SIMD oriented Fast Mersenne Twister(SFMT) pseudorandom
 * number generator using C structure.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (The University of Tokyo)
 *
 * Copyright (C) 2006, 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University.
 * Copyright (C) 2012 Mutsuo Saito, Makoto Matsumoto, Hiroshima
 * University and The University of Tokyo.
 * All rights reserved.
 *
 * The 3-clause BSD License is applied to this software, see
 * LICENSE.txt
 */
#include <string.h>
#include <assert.h>
#include "SFMTst.h"
#include "SFMT-params.h"
#include "SFMT-common.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*--------------------------------------
  FILE GLOBAL CONSTANTS
  --------------------------------------*/
/**
 * parameters used by sse2.
 */
static const w128_t sse2_param_mask = {{SFMT_MSK1, SFMT_MSK2,
					SFMT_MSK3, SFMT_MSK4}};
/**
 * a parity check vector which certificate the period of 2^{MEXP}-1.
 */
static const uint32_t parity[4] = {SFMT_PARITY1, SFMT_PARITY2,
				   SFMT_PARITY3, SFMT_PARITY4};

/*----------------
  STATIC FUNCTIONS
  ----------------*/
inline static int idxof(int i);
inline static uint32_t func1(uint32_t x);
inline static uint32_t func2(uint32_t x);
static void period_certification(sfmt_t * sfmt);
static void gen_rand_array(sfmt_t * sfmt, w128_t * array, int size);

/**
 * This function simulate a 64-bit index of LITTLE ENDIAN
 * in BIG ENDIAN machine.
 */
inline static int idxof(int i) {
    return i;
}

/**
 * This function represents a function used in the initialization
 * by init_by_array
 * @param x 32-bit integer
 * @return 32-bit integer
 */
static uint32_t func1(uint32_t x) {
    return (x ^ (x >> 27)) * (uint32_t)1664525UL;
}

/**
 * This function represents a function used in the initialization
 * by init_by_array
 * @param x 32-bit integer
 * @return 32-bit integer
 */
static uint32_t func2(uint32_t x) {
    return (x ^ (x >> 27)) * (uint32_t)1566083941UL;
}

/**
 * This function certificate the period of 2^{MEXP}
 * @param sfmt SFMT internal state
 */
static void period_certification(sfmt_t * sfmt) {
    int inner = 0;
    int i, j;
    uint32_t work;
    uint32_t * psfmt32 = (uint32_t *)&sfmt->state[0];

    for (i = 0; i < 4; i++)
	inner ^= psfmt32[idxof(i)] & parity[i];
    for (i = 16; i > 0; i >>= 1)
	inner ^= inner >> i;
    inner &= 1;
    /* check OK */
    if (inner == 1) {
	return;
    }
    /* check NG, and modification */
    for (i = 0; i < 4; i++) {
	work = 1;
	for (j = 0; j < 32; j++) {
	    if ((work & parity[i]) != 0) {
		psfmt32[idxof(i)] ^= work;
		return;
	    }
	    work = work << 1;
	}
    }
}

#if defined(HAVE_SSE2)
/* --------
   SSE2
   -------- */
inline static void mm_recursion(__m128i * r, __m128i a, __m128i b,
				__m128i c, __m128i d);

/**
 * This function represents the recursion formula.
 * @param r an output
 * @param a a 128-bit part of the interal state array
 * @param b a 128-bit part of the interal state array
 * @param c a 128-bit part of the interal state array
 * @param d a 128-bit part of the interal state array
 */
inline static void mm_recursion(__m128i * r, __m128i a, __m128i b,
				__m128i c, __m128i d)
{
    __m128i v, x, y, z;

    y = _mm_srli_epi32(b, SFMT_SR1);
    z = _mm_srli_si128(c, SFMT_SR2);
    v = _mm_slli_epi32(d, SFMT_SL1);
    z = _mm_xor_si128(z, a);
    z = _mm_xor_si128(z, v);
    x = _mm_slli_si128(a, SFMT_SL2);
    y = _mm_and_si128(y, sse2_param_mask.si);
    z = _mm_xor_si128(z, x);
    z = _mm_xor_si128(z, y);
    *r = z;
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
    __m128i r1, r2;
    w128_t * pstate = sfmt->state;

    r1 = pstate[SFMT_N - 2].si;
    r2 = pstate[SFMT_N - 1].si;
    for (i = 0; i < SFMT_N - SFMT_POS1; i++) {
	mm_recursion(&array[i].si, pstate[i].si,
		     pstate[i + SFMT_POS1].si, r1, r2);
	r1 = r2;
	r2 = array[i].si;
    }
    for (; i < SFMT_N; i++) {
	mm_recursion(&array[i].si, pstate[i].si,
		     array[i + SFMT_POS1 - SFMT_N].si, r1, r2);
	r1 = r2;
	r2 = array[i].si;
    }
    for (; i < size - SFMT_N; i++) {
	mm_recursion(&array[i].si, array[i - SFMT_N].si,
		     array[i + SFMT_POS1 - SFMT_N].si, r1, r2);
	r1 = r2;
	r2 = array[i].si;
    }
    for (j = 0; j < 2 * SFMT_N - size; j++) {
	pstate[j] = array[j + size - SFMT_N];
    }
    for (; i < size; i++, j++) {
	mm_recursion(&array[i].si, array[i - SFMT_N].si,
		     array[i + SFMT_POS1 - SFMT_N].si, r1, r2);
	r1 = r2;
	r2 = array[i].si;
	pstate[j] = array[i];
    }
}

#else /* C99 */
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
    w128_t *r1, *r2;
    w128_t * pstate = sfmt->state;

    r1 = &pstate[SFMT_N - 2];
    r2 = &pstate[SFMT_N - 1];
    for (i = 0; i < SFMT_N - SFMT_POS1; i++) {
	do_recursion(&array[i], &pstate[i], &pstate[i + SFMT_POS1], r1, r2);
	r1 = r2;
	r2 = &array[i];
    }
    for (; i < SFMT_N; i++) {
	do_recursion(&array[i], &pstate[i],
		     &array[i + SFMT_POS1 - SFMT_N], r1, r2);
	r1 = r2;
	r2 = &array[i];
    }
    for (; i < size - SFMT_N; i++) {
	do_recursion(&array[i], &array[i - SFMT_N],
		     &array[i + SFMT_POS1 - SFMT_N], r1, r2);
	r1 = r2;
	r2 = &array[i];
    }
    for (j = 0; j < 2 * SFMT_N - size; j++) {
	pstate[j] = array[j + size - SFMT_N];
    }
    for (; i < size; i++, j++) {
	do_recursion(&array[i], &array[i - SFMT_N],
		     &array[i + SFMT_POS1 - SFMT_N], r1, r2);
	r1 = r2;
	r2 = &array[i];
	pstate[j] = array[i];
    }
}
#endif

/*----------------
  PUBLIC FUNCTIONS
  ----------------*/
#define UNUSED_VARIABLE(x) (void)(x)

/**
 * This function returns the identification string.
 * The string shows the word size, the Mersenne exponent,
 * and all parameters of this generator.
 * @param sfmt SFMT internal state
 * @return identification string.
 */
const char *sfmt_get_idstring(sfmt_t * sfmt) {
    UNUSED_VARIABLE(sfmt);
    return SFMT_IDSTR;
}

/**
 * This function returns the minimum size of array used for \b
 * fill_array32() function.
 * @param sfmt SFMT internal state.
 * @return minimum size of array used for fill_array32() function.
 */
int sfmt_get_min_array_size32(sfmt_t * sfmt) {
    UNUSED_VARIABLE(sfmt);
    return SFMT_N32;
}

/**
 * This function returns the minimum size of array used for \b
 * fill_array64() function.
 * @param sfmt SFMT internal state.
 * @return minimum size of array used for fill_array64() function.
 */
int sfmt_get_min_array_size64(sfmt_t * sfmt) {
    UNUSED_VARIABLE(sfmt);
    return SFMT_N64;
}

#if defined(HAVE_SSE2)
/**
 * This function fills the internal state array with pseudorandom
 * integers.
 * @param sfmt SFMT internal state
 */
void sfmt_gen_rand_all(sfmt_t * sfmt) {
    int i;
    __m128i r1, r2;
    w128_t * pstate = sfmt->state;

    r1 = pstate[SFMT_N - 2].si;
    r2 = pstate[SFMT_N - 1].si;
    for (i = 0; i < SFMT_N - SFMT_POS1; i++) {
	mm_recursion(&pstate[i].si, pstate[i].si,
		     pstate[i + SFMT_POS1].si, r1, r2);
	r1 = r2;
	r2 = pstate[i].si;
    }
    for (; i < SFMT_N; i++) {
	mm_recursion(&pstate[i].si, pstate[i].si,
		     pstate[i + SFMT_POS1 - SFMT_N].si,
		     r1, r2);
	r1 = r2;
	r2 = pstate[i].si;
    }
}
#else
/**
 * This function fills the internal state array with pseudorandom
 * integers.
 * @param sfmt SFMT internal state
 */
void sfmt_gen_rand_all(sfmt_t * sfmt) {
    int i;
    w128_t *r1, *r2;
    w128_t * pstate = sfmt->state;

    r1 = &pstate[SFMT_N - 2];
    r2 = &pstate[SFMT_N - 1];
    for (i = 0; i < SFMT_N - SFMT_POS1; i++) {
	do_recursion(&pstate[i], &pstate[i], &pstate[i + SFMT_POS1], r1, r2);
	r1 = r2;
	r2 = &pstate[i];
    }
    for (; i < SFMT_N; i++) {
	do_recursion(&pstate[i], &pstate[i], &pstate[i + SFMT_POS1 - SFMT_N],
		     r1, r2);
	r1 = r2;
	r2 = &pstate[i];
    }
}

#endif


/**
 * This function generates pseudorandom 32-bit integers in the
 * specified array[] by one call. The number of pseudorandom integers
 * is specified by the argument size, which must be at least 624 and a
 * multiple of four.  The generation by this function is much faster
 * than the following gen_rand function.
 *
 * For initialization, init_gen_rand or init_by_array must be called
 * before the first call of this function. This function can not be
 * used after calling gen_rand function, without initialization.
 *
 * @param sfmt SFMT internal state.
 * @param array an array where pseudorandom 32-bit integers are filled
 * by this function.  The pointer to the array must be \b "aligned"
 * (namely, must be a multiple of 16) in the SIMD version, since it
 * refers to the address of a 128-bit integer.  In the standard C
 * version, the pointer is arbitrary.
 *
 * @param size the number of 32-bit pseudorandom integers to be
 * generated.  size must be a multiple of 4, and greater than or equal
 * to (MEXP / 128 + 1) * 4.
 *
 * @note \b memalign or \b posix_memalign is available to get aligned
 * memory. Mac OSX doesn't have these functions, but \b malloc of OSX
 * returns the pointer to the aligned memory block.
 */
void sfmt_fill_array32(sfmt_t * sfmt, uint32_t *array, int size) {
    assert(sfmt->idx == SFMT_N32);
    assert(size % 4 == 0);
    assert(size >= SFMT_N32);

    gen_rand_array(sfmt, (w128_t *)array, size / 4);
    sfmt->idx = SFMT_N32;
}

/**
 * This function generates pseudorandom 64-bit integers in the
 * specified array[] by one call. The number of pseudorandom integers
 * is specified by the argument size, which must be at least 312 and a
 * multiple of two.  The generation by this function is much faster
 * than the following gen_rand function.
 *
 * For initialization, init_gen_rand or init_by_array must be called
 * before the first call of this function. This function can not be
 * used after calling gen_rand function, without initialization.
 *
 * @param sfmt SFMT internal state.
 * @param array an array where pseudorandom 64-bit integers are filled
 * by this function.  The pointer to the array must be "aligned"
 * (namely, must be a multiple of 16) in the SIMD version, since it
 * refers to the address of a 128-bit integer.  In the standard C
 * version, the pointer is arbitrary.
 *
 * @param size the number of 64-bit pseudorandom integers to be
 * generated.  size must be a multiple of 2, and greater than or equal
 * to (MEXP / 128 + 1) * 2
 *
 * @note \b memalign or \b posix_memalign is available to get aligned
 * memory. Mac OSX doesn't have these functions, but \b malloc of OSX
 * returns the pointer to the aligned memory block.
 */
void sfmt_fill_array64(sfmt_t * sfmt, uint64_t *array, int size)
{
    assert(sfmt->idx == SFMT_N32);
    assert(size % 2 == 0);
    assert(size >= SFMT_N64);

    gen_rand_array(sfmt, (w128_t *)array, size / 2);
    sfmt->idx = SFMT_N32;
}

/**
 * This function initializes the internal state array with a 32-bit
 * integer seed.
 *
 * @param sfmt SFMT internal state.
 * @param seed a 32-bit integer used as the seed.
 */
void sfmt_init(sfmt_t * sfmt, uint32_t seed) {
    int i;
    uint32_t * psfmt32 = (uint32_t *)(sfmt->state);

    psfmt32[idxof(0)] = seed;
    for (i = 1; i < SFMT_N32; i++) {
	psfmt32[idxof(i)] = i + 1812433253UL
	    * (psfmt32[idxof(i - 1)]
	       ^ (psfmt32[idxof(i - 1)] >> 30));
    }
    sfmt->idx = SFMT_N32;
    period_certification(sfmt);
}

/**
 * This function initializes the internal state array,
 * with an array of 32-bit integers used as the seeds
 * @param sfmt SFMT internal state.
 * @param init_key the array of 32-bit integers, used as a seed.
 * @param key_length the length of init_key.
 */
void sfmt_init_by_array(sfmt_t * sfmt, uint32_t *init_key, int key_length) {
    int i, j, count;
    uint32_t r;
    int lag;
    int mid;
    int size = SFMT_N * 4;
    uint32_t * psfmt32 = (uint32_t *)(sfmt->state);

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

    memset(sfmt, 0x8b, sizeof(sfmt_t));
    if (key_length + 1 > SFMT_N32) {
	count = key_length + 1;
    } else {
	count = SFMT_N32;
    }
    r = func1(psfmt32[idxof(0)] ^ psfmt32[idxof(mid)]
	      ^ psfmt32[idxof(SFMT_N32 - 1)]);
    psfmt32[idxof(mid)] += r;
    r += key_length;
    psfmt32[idxof(mid + lag)] += r;
    psfmt32[idxof(0)] = r;

    count--;
    for (i = 1, j = 0; (j < count) && (j < key_length); j++) {
	r = func1(psfmt32[idxof(i)] ^ psfmt32[idxof((i + mid) % SFMT_N32)]
		  ^ psfmt32[idxof((i + SFMT_N32 - 1) % SFMT_N32)]);
	psfmt32[idxof((i + mid) % SFMT_N32)] += r;
	r += init_key[j] + i;
	psfmt32[idxof((i + mid + lag) % SFMT_N32)] += r;
	psfmt32[idxof(i)] = r;
	i = (i + 1) % SFMT_N32;
    }
    for (; j < count; j++) {
	r = func1(psfmt32[idxof(i)] ^ psfmt32[idxof((i + mid) % SFMT_N32)]
		  ^ psfmt32[idxof((i + SFMT_N32 - 1) % SFMT_N32)]);
	psfmt32[idxof((i + mid) % SFMT_N32)] += r;
	r += i;
	psfmt32[idxof((i + mid + lag) % SFMT_N32)] += r;
	psfmt32[idxof(i)] = r;
	i = (i + 1) % SFMT_N32;
    }
    for (j = 0; j < SFMT_N32; j++) {
	r = func2(psfmt32[idxof(i)] + psfmt32[idxof((i + mid) % SFMT_N32)]
		  + psfmt32[idxof((i + SFMT_N32 - 1) % SFMT_N32)]);
	psfmt32[idxof((i + mid) % SFMT_N32)] ^= r;
	r -= i;
	psfmt32[idxof((i + mid + lag) % SFMT_N32)] ^= r;
	psfmt32[idxof(i)] = r;
	i = (i + 1) % SFMT_N32;
    }

    sfmt->idx = SFMT_N32;
    period_certification(sfmt);
}

#if defined(__cplusplus)
}
#endif

