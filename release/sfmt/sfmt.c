/** 
 * @file  sfmt19937.c
 * @brief SIMD oriented Fast Mersenne Twister(SFMT)
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * @date 2006-08-29
 *
 * Copyright (C) 2006 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */
#include <string.h>
#include <assert.h>
#include "sfmt.h"
#include "params.h"

/*--------------------------------------
  FILE GLOBAL VARIABLES
  internal state, index counter and flag 
  --------------------------------------*/
/** the 128-bit internal state array */
static uint32_t sfmt[N + 1][4];
/** the 32bit interger pointer to the 128-bit internal state array */
static uint32_t *psfmt32 = &sfmt[0][0];
/** the 64bit interger pointer to the 128-bit internal state array */
static uint64_t *psfmt64 = (uint64_t *)&sfmt[0][0];
/** index counter to the 32-bit internal state array */
static int idx;
/** a flag: it is 0 if and only if the internal state is not yet
 * initialized. */
static int initialized = 0;
/** a flag: it is 1 if CPU is BIG ENDIAN. */
static int big_endian;

/*------------------------------------------
  128-bit SIMD like data type for standard C
  ------------------------------------------*/
/** 128-bit data structure */
struct W128_T {
    uint32_t a[4];
};
/** 128-bit data type */
typedef struct W128_T w128_t;

/*----------------
  STATIC FUNCTIONS
  ----------------*/
INLINE static void rshift128(uint32_t out[4], const uint32_t in[4],
			     int shift);
INLINE static void lshift128(uint32_t out[4], const uint32_t in[4],
			     int shift);
INLINE static void gen_rand_all(void);
INLINE static void gen_rand_array(w128_t array[], int size);
INLINE static uint32_t func1(uint32_t x);
INLINE static uint32_t func2(uint32_t x);
static void endian_check(void);

/**
 * This function simulates SIMD 128-bit right shift by the standard C.
 * The 128-bit integer given in in[4] is shifted by (shift * 8) bits.
 * This function simulates the LITTLE ENDIAN SIMD.
 * @param out the output of this function
 * @param in the 128-bit data to be shifted
 * @param shift the shift value
 */
INLINE static void rshift128(uint32_t out[4], const uint32_t in[4],
			     int shift) {
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in[3] << 32) | ((uint64_t)in[2]);
    tl = ((uint64_t)in[1] << 32) | ((uint64_t)in[0]);

    oh = th >> (shift * 8);
    ol = tl >> (shift * 8);
    ol |= th << (64 - shift * 8);
    out[1] = (uint32_t)(ol >> 32);
    out[0] = (uint32_t)ol;
    out[3] = (uint32_t)(oh >> 32);
    out[2] = (uint32_t)oh;
}

/**
 * This function simulates SIMD 128-bit left shift by the standard C.
 * The 128-bit integer given in in[4] is shifted by (shift * 8) bits.
 * This function simulates the LITTLE ENDIAN SIMD.
 * @param out the output of this function
 * @param in the 128-bit data to be shifted
 * @param shift the shift value
 */
INLINE static void lshift128(uint32_t out[4], const uint32_t in[4],
			     int shift) {
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in[3] << 32) | ((uint64_t)in[2]);
    tl = ((uint64_t)in[1] << 32) | ((uint64_t)in[0]);

    oh = th << (shift * 8);
    ol = tl << (shift * 8);
    oh |= tl >> (64 - shift * 8);
    out[1] = (uint32_t)(ol >> 32);
    out[0] = (uint32_t)ol;
    out[3] = (uint32_t)(oh >> 32);
    out[2] = (uint32_t)oh;
}

/**
 * This function represents the recursion formula.
 * @param r output
 * @param a a 128-bit part of the interal state array
 * @param b a 128-bit part of the interal state array
 * @param c a 128-bit part of the interal state array
 * @param d a 128-bit part of the interal state array
 */
#define do_recursion(r, a, b, c, d) \
    do { \
    uint32_t x[4];\
    uint32_t y[4];\
\
    lshift128(x, a, SL2);\
    rshift128(y, c, SR2);\
    r[0] = a[0] ^ x[0] ^ ((b[0] >> SR1) & MSK1) ^ y[0] ^ (d[0] << SL1);\
    r[1] = a[1] ^ x[1] ^ ((b[1] >> SR1) & MSK2) ^ y[1] ^ (d[1] << SL1);\
    r[2] = a[2] ^ x[2] ^ ((b[2] >> SR1) & MSK3) ^ y[2] ^ (d[2] << SL1);\
    r[3] = a[3] ^ x[3] ^ ((b[3] >> SR1) & MSK4) ^ y[3] ^ (d[3] << SL1);\
    } while(0)

/**
 * This function fills the internal state array with psedorandom
 * integers.
 */
INLINE static void gen_rand_all(void) {
    int i;
    uint32_t *r1, *r2;

    r1 = sfmt[N - 2];
    r2 = sfmt[N - 1];
    for (i = 0; i < N - POS1; i++) {
	do_recursion(sfmt[i], sfmt[i], sfmt[i + POS1], r1, r2);
	r1 = r2;
	r2 = sfmt[i];
    }
    for (; i < N; i++) {
	do_recursion(sfmt[i], sfmt[i], sfmt[i + POS1 - N], r1, r2);
	r1 = r2;
	r2 = sfmt[i];
    }
}

/**
 * This function fills the user-specified array with psedorandom
 * integers.
 *
 * @param array an 128-bit array to be filled by pseudorandom numbers.  
 * @param size number of 128-bit pesudorandom numbers to be generated.
 */
INLINE static void gen_rand_array(w128_t array[], int size) {
    int i;
    uint32_t *r1, *r2;

    r1 = sfmt[N - 2];
    r2 = sfmt[N - 1];
    for (i = 0; i < N - POS1; i++) {
	do_recursion(array[i].a, sfmt[i], sfmt[i + POS1], r1, r2);
	r1 = r2;
	r2 = array[i].a;
    }
    for (; i < N; i++) {
	do_recursion(array[i].a, sfmt[i], array[i + POS1 - N].a, r1, r2);
	r1 = r2;
	r2 = array[i].a;
    }
    for (; i < size; i++) {
	do_recursion(array[i].a, array[i - N].a, array[i + POS1 - N].a, r1, r2);
	r1 = r2;
	r2 = array[i].a;
    }
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
 * This function checks ENDIAN of CPU and set big_endian flag.
 */
static void endian_check(void) {
    uint32_t a[2] = {0, 1};
    uint64_t *pa;

    pa = (uint64_t *)a;
    if (*pa == 1) {
	big_endian = 1;
    } else {
	big_endian = 0;
    }
}

/*----------------
  PUBLIC FUNCTIONS
  ----------------*/
/**
 * This function generates and returns 32-bit pseudorandom number.
 * init_gen_rand or init_by_array must be called before this function.
 * @return 32-bit pseudorandom number
 */
INLINE uint32_t gen_rand32(void)
{
    uint32_t r;

    assert(initialized);
    if (idx >= N32) {
	gen_rand_all();
	idx = 0;
    }
    r = psfmt32[idx++];
    return r;
}

/**
 * This function generates and returns 64-bit pseudorandom number.
 * init_gen_rand or init_by_array must be called before this function.
 * The function gen_rand64 should not be called after gen_rand32,
 * unless an initialization is again executed. 
 * @return 64-bit pseudorandom number
 */
INLINE uint64_t gen_rand64(void)
{
    uint32_t r1, r2;
    uint64_t r;

    assert(initialized);
    assert(idx % 2 == 0);

    if (idx >= N32) {
	gen_rand_all();
	idx = 0;
    }
    if (big_endian) {
	r1 = psfmt32[idx];
	r2 = psfmt32[idx + 1];
	idx += 2;
	return ((uint64_t)r2 << 32) | r1;
    } else {
	r = psfmt64[idx / 2];
	idx += 2;
	return r;
    }
}

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
 * to 624.
 *
 * @note ??? \b memalign or \b posix_memalign is available to get aligned
 * memory. Mac OSX doesn't have these functions, but \b malloc of OSX
 * returns the pointer to the aligned memory block.
 */
INLINE void fill_array32(uint32_t array[], int size)
{
    assert(initialized);
    /* assert(array % 16 == 0); */
    assert(idx == N32);
    assert(size % 4 == 0);
    assert(size >= N32);

    gen_rand_array((w128_t *)array, size / 4);
    memcpy(psfmt32, array + size - N32, sizeof(uint32_t) * N32);
    idx = N32;
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
 * @param array an array where pseudorandom 64-bit integers are filled
 * by this function.  The pointer to the array must be "aligned"
 * (namely, must be a multiple of 16) in the SIMD version, since it
 * refers to the address of a 128-bit integer.  In the standard C
 * version, the pointer is arbitrary.
 *
 * @param size the number of 64-bit pseudorandom integers to be
 * generated.  size must be a multiple of 2, and greater than or equal
 * to 312.
 *
 * @note ??? \b memalign or \b posix_memalign is available to get aligned
 * memory. Mac OSX doesn't have these functions, but \b malloc of OSX
 * returns the pointer to the aligned memory block.
 */
INLINE void fill_array64(uint64_t array[], int size)
{
    assert(initialized);
    /* assert(array % 16 == 0); */
    assert(idx == N32);
    assert(size % 2 == 0);
    assert(size >= N64);

    gen_rand_array((w128_t *)array, size / 2);
    memcpy(psfmt64, array + size - N64, sizeof(uint64_t) * N64);
    idx = N32;
    if (big_endian) {
	int i;
	uint32_t x;
	uint32_t *pa;
	pa = (uint32_t *)array;
	for (i = 0; i < size * 2; i += 2) {
	    x = pa[i];
	    pa[i] = pa[i + 1];
	    pa[i + 1] = x;
	}
    }
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
    endian_check();
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
    endian_check();
    initialized = 1;
}
