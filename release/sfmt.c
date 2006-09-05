/** 
 * @file  sfmt.c
 * @brief SIMD oriented Fast Mersenne Twister(SFMT)
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
 */

/**
 * \mainpage 
 *
 * This is SIMD oriented Fast Mersenne Twister(SFMT) pseudorandom
 * number generator.
 *
 * This file provides:
 *
 * - INLINE void init_gen_rand() initialize pseudorandom number sequence.
 * - INLINE void init_by_array() initialize by array of seeds.
 * - INLINE uint32_t gen_rand() generates and returns pseudorandom number.
 * - INLINE void fill_array() fill given array with pseudorandom numbers. 
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
 * The new BSD License is applied to this software.
 * \verbinclude LICENSE.txt
 */
#include <string.h>
#include <assert.h>
#include "sfmt.h"


/*-----------------
  BASIC DEFINITIONS
  -----------------*/
/** Expornent part of Mersenne Prime. 2^MEXP -1 is the least period of
 * this PRNG */
#define MEXP 19937
/** word size of SFMT is 128 bit */
#define WORDSIZE 128
/** N is the size of 128 bit internal state array */
#define N (MEXP / WORDSIZE + 1)
/** N32 is the size of internal state array regard it as if 32 bit array */
#define N32 (N * 4)

/*----------------------
  the parameters of SFMT
  ----------------------*/
/** the pick up position of the array. */
const int POS1 = 122;
/** the parameter of shift left as four 32 bit registers. */
const int SL1 = 18;
/** the parameter of shift left as one 128 bit register. 
 * SL2 * 8 bits are shifted.
 */
const int SL2 = 1;
/** the parameter of shift right as four 32 bit registers. */
const int SR1 = 11;
/** the parameter of shift right as one 128 bit register. 
 * SR2 * 8 bits are shifted.
 */
const int SR2 = 1;
/** the parameter 1 of mask. These parameters are introduced to break
 * symmetry of SIMD.*/
const uint32_t MSK1 = 0xdfffffefU;
/** the parameter 2 of mask. These parameters are introduced to break
 * symmetry of SIMD.*/
const uint32_t MSK2 = 0xddfecb7fU;
/** the parameter 3 of mask. These parameters are introduced to break
 * symmetry of SIMD.*/
const uint32_t MSK3 = 0xbffaffffU;
/** the parameter 4 of mask. These parameters are introduced to break
 * symmetry of SIMD.*/
const uint32_t MSK4 = 0xbffffff6U;
/** the initial parameter. This parameter is needed for the assurance
 * of the period. */
const uint32_t INIT_LUNG = 0x6d736d6dU;

/*--------------------------------------
  FILE GLOBAL VARIABLES
  internal state, index counter and flag 
  --------------------------------------*/
/** the 128 bit internal state array */
static uint32_t sfmt[N][4];
/** the 32bit interger pointer to the 128 bit internal state array */
static uint32_t *psfmt = &sfmt[0][0];
/** index counter to the 32 bit internal state array */
static int idx;
/** the flag to show that the interal state is initialized or not */
static int initialized = 0;

/*------------------------------------------
  128 bit SIMD like data type for standard C
  ------------------------------------------*/
/** 128 bit data structure */
struct W128_T {
    uint32_t a[4];
};
/** 128 bit data type */
typedef struct W128_T w128_t;

/*----------------
  STATIC FUNCTIONS
  ----------------*/
INLINE static void rshift128(uint32_t out[4], const uint32_t in[4],
			     int shift);
INLINE static void lshift128(uint32_t out[4], const uint32_t in[4],
			     int shift);
INLINE static void do_recursion(uint32_t r[4], const uint32_t a[4], 
				const uint32_t b[4], const uint32_t c[4],
				const uint32_t d[4]);
INLINE static void gen_rand_all(void);
INLINE static void gen_rand_array(w128_t array[], uint32_t size);
INLINE static uint32_t func1(uint32_t x);
INLINE static uint32_t func2(uint32_t x);

/**
 * This function simulates SIMD 128 bit right shift in standard C.
 * shift * 8 bits are shifted.
 * This function simulates the LITTLE ENDIAN SIMD.
 * @param out the out put of this function
 * @param in the 128 bit data to be shifted
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
 * This function simulates SIMD 128 bit left shift in standard C.
 * shift * 8 bits are shifted.
 * This function simulates the LITTLE ENDIAN SIMD.
 * @param out the out put of this function
 * @param in the 128 bit data to be shifted
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
 * @param r the out put of this function
 * @param a a 128 bit data of interal state
 * @param b a 128 bit data of interal state
 * @param c a 128 bit data of interal state
 * @param d a 128 bit data of interal state
 */
INLINE static void do_recursion(uint32_t r[4], const uint32_t a[4], 
				const uint32_t b[4], const uint32_t c[4],
				const uint32_t d[4]) {
    uint32_t x[4];
    uint32_t y[4];

    lshift128(x, a, SL2);
    rshift128(y, c, SR2);
    r[0] = a[0] ^ x[0] ^ ((b[0] >> SR1) & MSK1) ^ y[0] ^ (d[0] << SL1);
    r[1] = a[1] ^ x[1] ^ ((b[1] >> SR1) & MSK2) ^ y[1] ^ (d[1] << SL1);
    r[2] = a[2] ^ x[2] ^ ((b[2] >> SR1) & MSK3) ^ y[2] ^ (d[2] << SL1);
    r[3] = a[3] ^ x[3] ^ ((b[3] >> SR1) & MSK4) ^ y[3] ^ (d[3] << SL1);
}

/**
 * This function makes N32 pseudorandom numbers in internal state
 * array at once.
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
 * This function makes size * 4 of 32 bit pseudorandom numbers in
 * given array at once.  
 * @param array an 128 bit array to be filled by
 * pseudorandom numbers.  
 * @param size number of 128 bit pesudorandom
 * numbers to be generated.
 */
INLINE static void gen_rand_array(w128_t array[], uint32_t size) {
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
 * This function represents recursion formula I used in init_by_array.
 * @param x 32 bit integer
 * @return 32 bit integer
 */
INLINE static uint32_t func1(uint32_t x) {
    return (x ^ (x >> 27)) * (uint32_t)1664525UL;
}

/**
 * This function represents recursion formula II used in init_by_array.
 * @param x 32 bit integer
 * @return 32 bit integer
 */
INLINE static uint32_t func2(uint32_t x) {
    return (x ^ (x >> 27)) * (uint32_t)1566083941UL;
}


/*----------------
  PUBLIC FUNCTIONS
  ----------------*/
/**
 * This function generates and returns 32 bit pseudorandom number.
 * init_gen_rand or init_by_array must be called before this function.
 * @return 32 bit pseudorandom number
 */
INLINE uint32_t gen_rand(void)
{
    uint32_t r;

    assert(initialized);
    if (idx >= N32) {
	gen_rand_all();
	idx = 0;
    }
    r = psfmt[idx++];
    return r;
}

/**
 * This function makes size of pseudorandom numbers in given array at
 * once.
 * init_gen_rand or init_by_array must be called before this function.
 * gen_rand should not be called after initialization and before this function.
 * In other words, this function can only follows to init_gen_rand,
 * init_by_array or this function.
 * @param array an 32 bit array to be filled by pseudorandom numbers.
 * The pointer to the array must be aligned in SIMD version.
 * The pointer to the array don't have to be aligned in standard C version.
 * @param size number of 32 bit pesudorandom numbers to be generated.
 * size must be multiple of 4.
 * size must be greater than or equal to 624.
 */
INLINE void fill_array(uint32_t array[], int size)
{
    assert(initialized);
    /* assert(array % 16 == 0); */
    assert(idx == N32);
    assert(size % 4 == 0);
    assert(size >= N32);

    gen_rand_array((w128_t *)array, size / 4);
    memcpy(psfmt, array + size - N32, sizeof(uint32_t) * N32);
    idx = N32;
}

/**
 * This function initialize the internal state array. 
 * @param seed the seed of pseudorandom numbers.
 */
INLINE void init_gen_rand(uint32_t seed)
{
    int i;

    psfmt[0] = seed;
    for (i = 1; i < N32; i++) {
	psfmt[i] = 1812433253UL * (psfmt[i - 1] ^ (psfmt[i - 1] >> 30)) + i;
    }
    psfmt[3] = INIT_LUNG;
    idx = N32;
    initialized = 1;
}

/**
 * This function initialize the internal state array. 
 * @param init_key the seed array of pseudorandom numbers.
 * @param key_length the length of init_key.
 */
INLINE void init_by_array(uint32_t init_key[], int key_length) {
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
    r = func1(psfmt[0] ^ psfmt[MID] ^ psfmt[N32 - 1]);
    psfmt[MID] += r;
    r += key_length;
    psfmt[MID + LAG] = r;
    psfmt[0] = r;
    i = 1;
    count--;
    for (i = 1, j = 0; (j < count) && (j < key_length); j++) {
	r = func1(psfmt[i] ^ psfmt[(i + MID) % N32] 
		  ^ psfmt[(i + N32 - 1) % N32]);
	psfmt[(i + MID) % N32] += r;
	r += init_key[j] + i;
	psfmt[(i + MID + LAG) % N32] += r;
	psfmt[i] = r;
	i = (i + 1) % N32;
    }
    for (; j < count; j++) {
	r = func1(psfmt[i] ^ psfmt[(i + MID) % N32] 
		  ^ psfmt[(i + N32 - 1) % N32]);
	psfmt[(i + MID) % N32] += r;
	r += i;
	psfmt[(i + MID + LAG) % N32] = r;
	psfmt[i] = r;
	i = (i + 1) % N32;
    }
    for (j = 0; j < N32; j++) {
	r = func2(psfmt[i] + psfmt[(i + MID) % N32] 
		  + psfmt[(i + N32 - 1) % N32]);
	psfmt[(i + MID) % N32] ^= r;
	r -= i;
	psfmt[(i + MID + LAG) % N32] ^= r;
	psfmt[i] = r;
	i = (i + 1) % N32;
    }

    psfmt[3] = INIT_LUNG;
    idx = N32;
    initialized = 1;
}
