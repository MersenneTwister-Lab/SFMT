/** 
 * @file sfmt19937-alti64.c 
 *
 * @brief SIMD oriented Fast Mersenne Twister(SFMT) for 64-bit output
 * for PowerPC G4, G5
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * @date 2007-01-10
 *
 * Copyright (C) 2006, 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 *
 * @note We assume BIG ENDIAN in this file.  \b init_gen_rand, \b
 * init_by_array and \b vec_recursion functions are optimized for 64-bit
 * output for BIG ENDIAN machine.
 */
#include <string.h>
#include <assert.h>
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
/** This definitions is a part of a 128-bit period certification vector.*/
#define PARITY1	0x00000001U
/** This definitions is a part of a 128-bit period certification vector.*/
#define PARITY2	0x00000000U
/** This definitions is a part of a 128-bit period certification vector.*/
#define PARITY3	0x00000000U
/** This definitions is a part of a 128-bit period certification vector.*/
#define PARITY4	0x13c9e684U

/*--------------------------------------
  FILE GLOBAL VARIABLES
  internal state, index counter and flag 
  --------------------------------------*/
/** the 128-bit internal state array */
static vector unsigned int sfmt[N];
/** the 32-bit interger pointer to the 128-bit internal state array */
static uint32_t *psfmt32 = (uint32_t *)&sfmt[0];
/** the 64-bit interger pointer to the 128-bit internal state array */
static uint64_t *psfmt64 = (uint64_t *)&sfmt[0];
/** index counter to the 32-bit internal state array */
static int idx;
/** a flag: it is 0 if and only if the internal state is not yet
 * initialized. */
static int initialized = 0;
/** a parity check vector which certificate the period of 2^{MEXP}-1. */
static uint32_t parity[4] = {PARITY1, PARITY2, PARITY3, PARITY4};

/*----------------
  STATIC FUNCTIONS
  ----------------*/
static void gen_rand_all(void);
static void gen_rand_array(vector unsigned int array[], int size);
static uint32_t func1(uint32_t x);
static uint32_t func2(uint32_t x);
INLINE static int idxof(int i);
INLINE static vector unsigned int vec_recursion(vector unsigned int a,
						vector unsigned int b,
						vector unsigned int c,
						vector unsigned int d);

static void period_certification(void);
/**
 * This function represents the recursion formula.
 * This function is optimized for generate 64-bit pseudorandom number
 * in BIG ENDIAN machine.
 * @param a a 128-bit part of the interal state array
 * @param b a 128-bit part of the interal state array
 * @param c a 128-bit part of the interal state array
 * @param d a 128-bit part of the interal state array
 * @return output
 */
INLINE static __attribute__((always_inline)) 
    vector unsigned int vec_recursion(vector unsigned int a,
				      vector unsigned int b,
				      vector unsigned int c,
				      vector unsigned int d) {
    const vector unsigned int sl1 = (vector unsigned int)(SL1, SL1, SL1, SL1);
    const vector unsigned int sr1 = (vector unsigned int)(SR1, SR1, SR1, SR1);
    const vector unsigned int mask = (vector unsigned int)
	(MSK2, MSK1, MSK4, MSK3);
    const vector unsigned char perm_sl = (vector unsigned char)
	(1, 2, 3, 4, 5, 6, 7, 31, 9, 10, 11, 12, 13, 14, 15, 0);
    const vector unsigned char perm_sr = (vector unsigned char)
	(15, 0, 1, 2, 3, 4, 5, 6, 17, 8, 9, 10, 11, 12, 13, 14);

    vector unsigned int v, w, x, y, z;

    x = vec_perm(a, perm_sl, perm_sl);
    v = a;
    y = vec_sr(b, sr1);
    z = vec_perm(c, perm_sr, perm_sr);
    w = vec_sl(d, sl1);
    z = vec_xor(z, w);
    y = vec_and(y, mask);
    v = vec_xor(v, x);
    z = vec_xor(z, y);
    z = vec_xor(z, v);
    return z;
}

/**
 * This function fills the internal state array with psedorandom
 * integers.
 */
INLINE void gen_rand_all(void) {
    int i;
    vector unsigned int r, r1, r2;

    r1 = sfmt[N - 2];
    r2 = sfmt[N - 1];
    for (i = 0; i < N - POS1; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1], r1, r2);
	sfmt[i] = r;
	r1 = r2;
	r2 = r;
    }
    for (; i < N; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1 - N], r1, r2);
	sfmt[i] = r;
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
static void gen_rand_array(vector unsigned int array[], int size)
{
    int i, j;
    vector unsigned int r, r1, r2;
 
    r1 = sfmt[N - 2];
    r2 = sfmt[N - 1];
    for (i = 0; i < N - POS1; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1], r1, r2);
	array[i] = r;
	r1 = r2;
	r2 = r;
    }
    for (; i < N; i++) {
	r = vec_recursion(sfmt[i], array[i + POS1 - N], r1, r2);
	array[i] = r;
	r1 = r2;
	r2 = r;
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = vec_recursion(array[i - N], array[i + POS1 - N], r1, r2);
	array[i] = r;
	r1 = r2;
	r2 = r;
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j] = array[j + size - N];
    }
    for (; i < size; i++) {
	r = vec_recursion(array[i - N], array[i + POS1 - N], r1, r2);
	array[i] = r;
	sfmt[j++] = r;
	r1 = r2;
	r2 = r;
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
 * This function simulate a 64-bit index of LITTLE ENDIAN 
 * in BIG ENDIAN machine.
 */
INLINE static int idxof(int i) {
    return i ^ 1;
}

/**
 * This function certificate the period of 2^19937-1.
 */
static void period_certification(void) {
    int inner = 0;
    int i, j;
    uint32_t work;

    for (i = 0; i < 4; i++) {
	work = psfmt32[idxof(i)] & parity[i];
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
 * This function generates and returns 64-bit pseudorandom number.
 * init_gen_rand or init_by_array must be called before this function.
 * The function gen_rand64 should not be called after gen_rand32,
 * unless an initialization is again executed. 
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
 */
INLINE void fill_array64(uint64_t array[], int size)
{
    assert(initialized);
    assert((uint32_t)array % 16 == 0);
    assert(idx == N32);
    assert(size % 2 == 0);
    assert(size >= N64);

    gen_rand_array((vector unsigned int *)array, size / 2);
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

    psfmt32[idxof(0)] = seed;
    for (i = 1; i < N32; i++) {
	psfmt32[idxof(i)] = 1812433253UL * (psfmt32[idxof(i - 1)] 
					    ^ (psfmt32[idxof(i - 1)] >> 30))
	    + i;
    }

    idx = N32;
    period_certification();
    initialized = 1;
}

/**
 * This function initializes the internal state array,
 * with an array of 32-bit integers used as the seeds
 * This function is optimized for 64-bit output in BIG ENDIAN machine.
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
    r = func1(psfmt32[idxof(0)] ^ psfmt32[idxof(MID)] 
	      ^ psfmt32[idxof(N32 - 1)]);
    psfmt32[idxof(MID)] += r;
    r += key_length;
    psfmt32[idxof(MID + LAG)] += r;
    psfmt32[idxof(0)] = r;
    i = 1;
    count--;
    for (i = 1, j = 0; (j < count) && (j < key_length); j++) {
	r = func1(psfmt32[idxof(i)] ^ psfmt32[idxof((i + MID) % N32)] 
		  ^ psfmt32[idxof((i + N32 - 1) % N32)]);
	psfmt32[idxof((i + MID) % N32)] += r;
	r += init_key[j] + i;
	psfmt32[idxof((i + MID + LAG) % N32)] += r;
	psfmt32[idxof(i)] = r;
	i = (i + 1) % N32;
    }
    for (; j < count; j++) {
	r = func1(psfmt32[idxof(i)] ^ psfmt32[idxof((i + MID) % N32)] 
		  ^ psfmt32[idxof((i + N32 - 1) % N32)]);
	psfmt32[idxof((i + MID) % N32)] += r;
	r += i;
	psfmt32[idxof((i + MID + LAG) % N32)] += r;
	psfmt32[idxof(i)] = r;
	i = (i + 1) % N32;
    }
    for (j = 0; j < N32; j++) {
	r = func2(psfmt32[idxof(i)] + psfmt32[idxof((i + MID) % N32)] 
		  + psfmt32[idxof((i + N32 - 1) % N32)]);
	psfmt32[idxof((i + MID) % N32)] ^= r;
	r -= i;
	psfmt32[idxof((i + MID + LAG) % N32)] ^= r;
	psfmt32[idxof(i)] = r;
	i = (i + 1) % N32;
    }

    idx = N32;
    period_certification();
    initialized = 1;
}

