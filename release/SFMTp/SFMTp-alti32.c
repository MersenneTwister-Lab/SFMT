/** 
 * @file SFMTp-alti32.c 
 * @brief SIMD oriented Fast Mersenne Twister Pulmonary version (SFMTp)
 * for PowerPC G4, G5
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 *
 * @note We assume BIG ENDIAN in this file.
 */
#include <string.h>
#include <assert.h>
#include "SFMTp.h"
#include "SFMTp-params.h"

/*--------------------------------------
  FILE GLOBAL VARIABLES
  internal state, index counter and flag 
  --------------------------------------*/
/** the 128-bit internal state array */
static vector unsigned int sfmt[N + 1];
/** the 32bit interger pointer to the 128-bit internal state array */
static uint32_t *psfmt32 = (uint32_t *)&sfmt[0];
/** index counter to the 32-bit internal state array */
static int idx;
/** a parity check vector which certificate the period of 2^{MEXP}-1. */
static uint32_t parity[4] = {PARITY1, PARITY2, PARITY3, PARITY4};
/** a flag: it is 0 if and only if the internal state is not yet
 * initialized. */
static int initialized = 0;

/*----------------
  STATIC FUNCTIONS
  ----------------*/
inline static void gen_rand_all(void);
inline static void gen_rand_array(vector unsigned int array[], int size);
static uint32_t func1(uint32_t x);
static uint32_t func2(uint32_t x);
inline static vector unsigned int vec_recursion(vector unsigned int a,
						vector unsigned int b,
						vector unsigned int c,
						vector unsigned int d,
						vector unsigned int sl1,
						vector unsigned int sr1,
						vector unsigned int sr2,
						vector unsigned int mask,
						vector unsigned char perm_sl,
						vector unsigned char perm);
/**
 * This function represents the recursion formula in AltiVec and BIG ENDIAN.
 * @param a a 128-bit part of the interal state array
 * @param b a 128-bit part of the interal state array
 * @param c a 128-bit part of the interal state array
 * @param d a 128-bit part of the interal state array
 * @return output
 */
inline static __attribute__((always_inline))
    vector unsigned int vec_recursion(vector unsigned int a,
				      vector unsigned int b,
				      vector unsigned int c,
				      vector unsigned int d,
				      vector unsigned int sl1,
				      vector unsigned int sr1,
				      vector unsigned int sr2,
				      vector unsigned int mask,
				      vector unsigned char perm_sl,
				      vector unsigned char perm) {

    vector unsigned int v, w, x, y, z;
    x = vec_perm(a, perm_sl, perm_sl);
    v = a;
    y = vec_sr(b, sr1);
    z = vec_perm(d, perm, perm);
    w = vec_xor(vec_sl(c, sl1), vec_sr(c, sr2));
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
inline void gen_rand_all(void) {
    int i;
    vector unsigned int r, lung;

    const vector unsigned int sl1 = (vector unsigned int)(SL1, SL1, SL1, SL1);
    const vector unsigned int sr1 = (vector unsigned int)(SR1, SR1, SR1, SR1);
    const vector unsigned int sr2 = (vector unsigned int)(SR2, SR2, SR2, SR2);
    const vector unsigned int mask = (vector unsigned int)
	(MSK1, MSK2, MSK3, MSK4);
    const vector unsigned char perm_sl = ALTI_SL2_PERM;
    const vector unsigned char perm = (vector unsigned char)
	(12, 13, 14, 15, 8, 9, 10, 11, 0, 1, 2, 3, 4, 5, 6, 7);

    lung = sfmt[N];
    r = sfmt[N - 1];
    for (i = 0; i < N - POS1; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1], r, lung, sl1, sr1, sr2,
			  mask, perm_sl, perm);
	sfmt[i] = r;
	lung = vec_xor(lung, r);
    }
    for (; i < N; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1 - N], r, lung, sl1, sr1, sr2,
			  mask, perm_sl, perm);
	sfmt[i] = r;
	lung = vec_xor(lung, r);
    }
    sfmt[N] = lung;
}

/**
 * This function fills the user-specified array with psedorandom
 * integers.
 *
 * @param array an 128-bit array to be filled by pseudorandom numbers.  
 * @param size number of 128-bit pesudorandom numbers to be generated.
 */
inline static void gen_rand_array(vector unsigned int array[], int size)
{
    int i, j;
    vector unsigned int r, lung;
 
    const vector unsigned int sl1 = (vector unsigned int)(SL1, SL1, SL1, SL1);
    const vector unsigned int sr1 = (vector unsigned int)(SR1, SR1, SR1, SR1);
    const vector unsigned int sr2 = (vector unsigned int)(SR2, SR2, SR2, SR2);
    const vector unsigned int mask = (vector unsigned int)
	(MSK1, MSK2, MSK3, MSK4);
    const vector unsigned char perm_sl = ALTI_SL2_PERM;
    const vector unsigned char perm = (vector unsigned char)
	(12, 13, 14, 15, 8, 9, 10, 11, 0, 1, 2, 3, 4, 5, 6, 7);

    /* read from sfmt */
    lung = sfmt[N];
    r = sfmt[N - 1];
    for (i = 0; i < N - POS1; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1], r, lung, sl1, sr1, sr2,
			  mask, perm_sl, perm);
	array[i] = r;
	lung = vec_xor(lung, r);
    }
    for (; i < N; i++) {
	r = vec_recursion(sfmt[i], array[i + POS1 - N], r, lung, sl1, sr1, sr2,
			  mask, perm_sl, perm);
	array[i] = r;
	lung = vec_xor(lung, r);
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = vec_recursion(array[i - N], array[i + POS1 - N], r, lung, sl1, sr1,
			  sr2, mask, perm_sl, perm);
	array[i] = r;
	lung = vec_xor(lung, r);
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j] = array[j + size - N];
    }
    for (; i < size; i++) {
	r = vec_recursion(array[i - N], array[i + POS1 - N], r, lung, sl1, sr1,
			  sr2, mask, perm_sl, perm);
	array[i] = r;
	sfmt[j++] = r;
	lung = vec_xor(lung, r);
    }
    sfmt[N] = lung;
}

/**
 * This function swaps high and low 32-bit of 64-bit integers in user
 * specified array.
 *
 * @param array an 128-bit array to be swaped.
 * @param size size of 128-bit array.
 */
inline static void vec_swap(vector unsigned int array[], uint32_t size)
{
    int i;
    const vector unsigned char perm = (vector unsigned char)
	(4, 5, 6, 7, 0, 1, 2, 3, 12, 13, 14, 15, 8, 9, 10, 11);

    for (i = 0; i < size; i++) {
	array[i] = vec_perm(array[i], perm, perm);
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
 * This function certificate the period of 2^{MEXP}-1.
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
 * This function returns the minimum size of array used for \b fill_array32.
 * @return minimum size of array used for fill_array32.
 */
int get_min_array_size32(void)
{
    return N32;
}

/**
 * This function returns the minimum size of array used for \b fill_array64.
 * @return minimum size of array used for fill_array64.
 */
int get_min_array_size64(void)
{
    return N64;
}

/**
 * This function generates and returns 32-bit pseudorandom number.
 * init_gen_rand or init_by_array must be called before this function.
 * @return 32-bit pseudorandom number
 */
inline uint32_t gen_rand32(void)
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
 * This function generates and returns 64-bit pseudorandom number.
 * init_gen_rand or init_by_array must be called before this function.
 * The function gen_rand64 should not be called after gen_rand32,
 * unless an initialization is again executed. 
 * @return 64-bit pseudorandom number
 */
inline uint64_t gen_rand64(void)
{
    uint32_t r1, r2;

    assert(initialized);
    assert(idx % 2 == 0);

    if (idx >= N32) {
	gen_rand_all();
	idx = 0;
    }
    r1 = psfmt32[idx];
    r2 = psfmt32[idx + 1];
    idx += 2;
    return ((uint64_t)r2 << 32) | r1;
}

/**
 * This function generates pseudorandom 32-bit integers in the
 * specified array[] by one call. The number of pseudorandom integers
 * is specified by the argument size, which must be at least (MEXP /
 * 128) * 4 and a multiple of four.  The generation by this function
 * is much faster than the following gen_rand function.
 *
 * For initialization, init_gen_rand or init_by_array must be called
 * before the first call of this function. This function can not be
 * used after calling gen_rand function, without initialization.
 *
 * @param array an array where pseudorandom 32-bit integers are filled
 * by this function.  The pointer to the array must be "aligned"
 * (namely, must be a multiple of 16) in the SIMD version, since it
 * refers to the address of a 128-bit integer.  In the standard C
 * version, the pointer is arbitrary.
 *
 * @param size the number of 32-bit pseudorandom integers to be
 * generated.  size must be a multiple of 4, and greater than or equal
 * to (MEXP / 128) * 4.
 */
inline void fill_array32(uint32_t array[], int size)
{
    assert(initialized);
    assert((uint32_t)array % 16 == 0);
    assert(idx == N32);
    assert(size % 4 == 0);
    assert(size >= N32);

    gen_rand_array((vector unsigned int *)array, size / 4);
    idx = N32;
}

/**
 * This function generates pseudorandom 64-bit integers in the
 * specified array[] by one call. The number of pseudorandom integers
 * is specified by the argument size, which must be at least (MEXP /
 * 128) * 2 and a multiple of two.  The generation by this function is
 * much faster than the following gen_rand function.
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
 * to (MEXP / 128) * 2.
 */
inline void fill_array64(uint64_t array[], int size)
{
    assert(initialized);
    assert((uint32_t)array % 16 == 0);
    assert(idx == N32);
    assert(size % 2 == 0);
    assert(size >= N64);

    gen_rand_array((vector unsigned int *)array, size / 2);
    idx = N32;
    vec_swap((vector unsigned int *)array, size / 2);
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

