/**
 * @file  SFMT.c
 * @brief SIMD oriented Fast Mersenne Twister(SFMT)
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2006,2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */
#include <string.h>
#include <assert.h>
#include "SFMTst.h"
#include "SFMT-params.h"

#if defined(__BIG_ENDIAN__) && !defined(__amd64) && !defined(BIG_ENDIAN64)
#define BIG_ENDIAN64 1
#endif
#if defined(HAVE_ALTIVEC) && !defined(BIG_ENDIAN64)
#define BIG_ENDIAN64 1
#endif
#if defined(ONLY64) && !defined(BIG_ENDIAN64)
  #if defined(__GNUC__)
    #error "-DONLY64 must be specified with -DBIG_ENDIAN64"
  #endif
#undef ONLY64
#endif

w128_t sse2_param_mask = {{SFMT_MSK2, SFMT_MSK1, SFMT_MSK4, SFMT_MSK3}};
/*--------------------------------------
  FILE GLOBAL VARIABLES
  internal state, index counter and flag
  --------------------------------------*/
/** a parity check vector which certificate the period of 2^{MEXP} */
static const uint32_t parity[4] = {SFMT_PARITY1, SFMT_PARITY2,
				   SFMT_PARITY3, SFMT_PARITY4};
#include "SFMT-hardware.h"

/*----------------
  STATIC FUNCTIONS
  ----------------*/
inline static int idxof(int i);
inline static uint32_t func1(uint32_t x);
inline static uint32_t func2(uint32_t x);
static void period_certification(sfmt_t * sfmt);
#if defined(BIG_ENDIAN64) && !defined(ONLY64)
inline static void swap(w128_t *array, int size);
#endif

/**
 * This function simulate a 64-bit index of LITTLE ENDIAN
 * in BIG ENDIAN machine.
 */
inline static int idxof(int i) {
    return i;
}
/**
 * This function simulates SIMD 128-bit right shift by the standard C.
 * The 128-bit integer given in in is shifted by (shift * 8) bits.
 * This function simulates the LITTLE ENDIAN SIMD.
 * @param out the output of this function
 * @param in the 128-bit data to be shifted
 * @param shift the shift value
 */
/**
 * This function represents the recursion formula.
 * @param r output
 * @param a a 128-bit part of the internal state array
 * @param b a 128-bit part of the internal state array
 * @param c a 128-bit part of the internal state array
 * @param d a 128-bit part of the internal state array
 */

/**
 * This function fills the internal state array with pseudorandom
 * integers.
 */
inline static void gen_rand_all(sfmt_t * sfmt) {
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


/**
 * This function fills the user-specified array with pseudorandom
 * integers.
 *
 * @param array an 128-bit array to be filled by pseudorandom numbers.
 * @param size number of 128-bit pseudorandom numbers to be generated.
 */
inline static void gen_rand_array(w128_t * array, int size, sfmt_t * sfmt) {
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

#if defined(BIG_ENDIAN64)
inline static void swap(w128_t *array, int size) {
    int i;
    uint32_t x, y;

    for (i = 0; i < size; i++) {
	x = array[i].u[0];
	y = array[i].u[2];
	array[i].u[0] = array[i].u[1];
	array[i].u[2] = array[i].u[3];
	array[i].u[1] = x;
	array[i].u[3] = y;
    }
}
#endif
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

/*----------------
  PUBLIC FUNCTIONS
  ----------------*/
/**
 * This function returns the identification string.
 * The string shows the word size, the Mersenne exponent,
 * and all parameters of this generator.
 */
const char *get_idstring(sfmt_t * sfmt) {
    return SFMT_IDSTR;
}

/**
 * This function returns the minimum size of array used for \b
 * fill_array32() function.
 * @return minimum size of array used for fill_array32() function.
 */
int get_min_array_size32(sfmt_t * sfmt) {
    return SFMT_N32;
}

/**
 * This function returns the minimum size of array used for \b
 * fill_array64() function.
 * @return minimum size of array used for fill_array64() function.
 */
int get_min_array_size64(sfmt_t * sfmt) {
    return SFMT_N64;
}

#ifndef ONLY64
/**
 * This function generates and returns 32-bit pseudorandom number.
 * init_gen_rand or init_by_array must be called before this function.
 * @return 32-bit pseudorandom number
 */
uint32_t gen_rand32(sfmt_t * sfmt) {
    uint32_t r;
    uint32_t * psfmt32 = (uint32_t *)(sfmt->state);

    if (sfmt->idx >= SFMT_N32) {
	gen_rand_all(sfmt);
	sfmt->idx = 0;
    }
    r = psfmt32[sfmt->idx++];
    return r;
}
#endif
/**
 * This function generates and returns 64-bit pseudorandom number.
 * init_gen_rand or init_by_array must be called before this function.
 * The function gen_rand64 should not be called after gen_rand32,
 * unless an initialization is again executed.
 * @return 64-bit pseudorandom number
 */
uint64_t gen_rand64(sfmt_t * sfmt) {
#if defined(BIG_ENDIAN64) && !defined(ONLY64)
    uint32_t r1, r2;
    uint32_t * psfmt32 = (uint32_t *)(sfmt->state);
#else
    uint64_t r;
    uint64_t * psfmt64 = (uint64_t *)(sfmt->state);
#endif

    assert(sfmt->idx % 2 == 0);

    if (sfmt->idx >= SFMT_N32) {
	gen_rand_all(sfmt);
	sfmt->idx = 0;
    }
#if defined(BIG_ENDIAN64) && !defined(ONLY64)
    r1 = psfmt32[sfmt->idx];
    r2 = psfmt32[sfmt->idx + 1];
    sfmt->idx += 2;
    return ((uint64_t)r2 << 32) | r1;
#else
    r = psfmt64[sfmt->idx / 2];
    sfmt->idx += 2;
    return r;
#endif
}

#ifndef ONLY64
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
void fill_array32(uint32_t *array, int size, sfmt_t * sfmt) {
    assert(sfmt->idx == SFMT_N32);
    assert(size % 4 == 0);
    assert(size >= SFMT_N32);

    gen_rand_array((w128_t *)array, size / 4, sfmt);
    sfmt->idx = SFMT_N32;
}
#endif

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
void fill_array64(uint64_t *array, int size, sfmt_t * sfmt) {
    assert(sfmt->idx == SFMT_N32);
    assert(size % 2 == 0);
    assert(size >= SFMT_N64);

    gen_rand_array((w128_t *)array, size / 2, sfmt);
    sfmt->idx = SFMT_N32;

#if defined(BIG_ENDIAN64) && !defined(ONLY64)
    swap((w128_t *)array, size /2);
#endif
}

/**
 * This function initializes the internal state array with a 32-bit
 * integer seed.
 *
 * @param seed a 32-bit integer used as the seed.
 */
void init_gen_rand(uint32_t seed, sfmt_t * sfmt) {
    int i;
    uint32_t * psfmt32 = (uint32_t *)(sfmt->state);

    psfmt32[idxof(0)] = seed;
    for (i = 1; i < SFMT_N32; i++) {
	psfmt32[idxof(i)] = 1812433253UL * (psfmt32[idxof(i - 1)]
					    ^ (psfmt32[idxof(i - 1)] >> 30))
	    + i;
    }
    sfmt->idx = SFMT_N32;
    period_certification(sfmt);
}

/**
 * This function initializes the internal state array,
 * with an array of 32-bit integers used as the seeds
 * @param init_key the array of 32-bit integers, used as a seed.
 * @param key_length the length of init_key.
 */
void init_by_array(uint32_t *init_key, int key_length, sfmt_t * sfmt) {
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


