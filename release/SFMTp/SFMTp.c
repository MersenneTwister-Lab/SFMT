/** 
 * @file  SFMTp.c
 * @brief SIMD-oriented Fast Mersenne Twister Pulmonary version (SFMTp)
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
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
/** a parity check vector which certificate the period of 2^{MEXP}-1. */
static uint32_t parity[4] = {PARITY1, PARITY2, PARITY3, PARITY4};
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
inline static void lshift128(uint32_t out[4], const uint32_t in[4],
			     int shift);
inline static void gen_rand_all(void);
inline static void gen_rand_array(w128_t array[], int size);
inline static uint32_t func1(uint32_t x);
inline static uint32_t func2(uint32_t x);
inline static void assign128(uint32_t to[4], uint32_t from[4]);
inline static void xor128(uint32_t to[4], uint32_t from[4]);
static void endian_check(void);
static void period_certification(void);

/**
 * This function simulates SIMD 128-bit left shift by the standard C.
 * The 128-bit integer given in in[4] is shifted by (shift * 8) bits.
 * This function simulates the LITTLE ENDIAN SIMD.
 * @param out the output of this function
 * @param in the 128-bit data to be shifted
 * @param shift the shift value
 */
static inline
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    void lshift128(uint32_t out[4], const uint32_t in[4], int shift) {
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
 * This function simulates SIMD 128-bit assign by the standard C.
 * @param to the output of this function
 * @param from the 128-bit data to be assigned
 */
static inline
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    void assign128(uint32_t to[4], uint32_t from[4]) {
    to[0] = from[0];
    to[1] = from[1];
    to[2] = from[2];
    to[3] = from[3];
}

/**
 * This function simulates SIMD 128-bit xor and assign by the standard C.
 * @param to the output of this function
 * @param from the 128-bit data to be assigned
 */
static inline
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    void xor128(uint32_t to[4], uint32_t from[4]) {
    to[0] ^= from[0];
    to[1] ^= from[1];
    to[2] ^= from[2];
    to[3] ^= from[3];
}

/**
 * This function represents the recursion formula.
 * @param r output
 * @param a a 128-bit part of the interal state array
 * @param b a 128-bit part of the interal state array
 * @param c a 128-bit part of the interal state array
 * @param d a 128-bit part of the interal state array
 */
static inline
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    void do_recursion(uint32_t r[4], uint32_t a[4], uint32_t b[4],
		      uint32_t c[4], uint32_t d[4]) {
    uint32_t x[4];

    lshift128(x, a, SL2);
    r[0] = a[0] ^ x[0] ^ ((b[0] >> SR1) & MSK1) ^ (c[0] >> SR2) ^ (c[0] << SL1)
	^ d[3];
    r[1] = a[1] ^ x[1] ^ ((b[1] >> SR1) & MSK2) ^ (c[1] >> SR2) ^ (c[1] << SL1)
	^ d[2];
    r[2] = a[2] ^ x[2] ^ ((b[2] >> SR1) & MSK3) ^ (c[2] >> SR2) ^ (c[2] << SL1)
	^ d[0];
    r[3] = a[3] ^ x[3] ^ ((b[3] >> SR1) & MSK4) ^ (c[3] >> SR2) ^ (c[3] << SL1)
	^ d[1];
}

/**
 * This function fills the internal state array with psedorandom
 * integers.
 */
static inline void gen_rand_all(void) {
    int i;
    uint32_t lung[4];

    assign128(lung, sfmt[N]);
    do_recursion(sfmt[0], sfmt[0], sfmt[POS1], sfmt[N - 1], lung);
    xor128(lung, sfmt[0]);
    for (i = 1; i + POS1 < N; i++) {
	do_recursion(sfmt[i], sfmt[i], sfmt[i + POS1], sfmt[i - 1],
		     lung);
	xor128(lung, sfmt[i]);
    }
    for (; i < N; i++) {
	do_recursion(sfmt[i], sfmt[i], sfmt[i + POS1 - N], sfmt[i - 1],
		     lung);
	xor128(lung, sfmt[i]);
    }
    assign128(sfmt[N], lung);
}

/**
 * This function fills the user-specified array with psedorandom
 * integers.
 *
 * @param array an 128-bit array to be filled by pseudorandom numbers.  
 * @param size number of 128-bit pesudorandom numbers to be generated.
 */
inline static void gen_rand_array(w128_t array[], int size) {
    int i;
    uint32_t lung[4];

    assign128(lung, sfmt[N]);
    do_recursion(array[0].a, sfmt[0], sfmt[POS1], sfmt[N - 1], lung);
    xor128(lung, array[0].a);
    for (i = 1; i < N - POS1; i++) {
	do_recursion(array[i].a, sfmt[i], sfmt[i + POS1], array[i - 1].a, lung);
	xor128(lung, array[i].a);
    }
    for (; i < N; i++) {
	do_recursion(array[i].a, sfmt[i], array[i + POS1 - N].a, array[i - 1].a,
		     lung);
	xor128(lung, array[i].a);
    }
    for (; i < size; i++) {
	do_recursion(array[i].a, array[i - N].a, array[i + POS1 - N].a,
		     array[i - 1].a, lung);
	xor128(lung, array[i].a);
    }
    assign128(sfmt[N], lung);
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

/**
 * This function certificate the period of 2^{MEXP}-1.
 */
static void period_certification(void) {
    int inner = 0;
    int i, j;
    uint32_t work;

    for (i = 0; i < 4; i++) {
	work = sfmt[N][i] & parity[i];
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
		sfmt[N][i] ^= work;
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
 * @return id string.
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
inline uint64_t gen_rand64(void)
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
 * is specified by the argument size, which must be at least (MEXP /
 * 128) * 4 and a multiple of four.  The generation by this function
 * is much faster than the following gen_rand function.
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
 * to (MEXP / 128) * 4.
 *
 * @note \b memalign or \b posix_memalign is available to get aligned
 * memory. Mac OSX doesn't have these functions, but \b malloc of OSX
 * returns the pointer to the aligned memory block.
 */
inline void fill_array32(uint32_t array[], int size)
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
 *
 * @note \b memalign or \b posix_memalign is available to get aligned
 * memory. Mac OSX doesn't have these functions, but \b malloc of OSX
 * returns the pointer to the aligned memory block.
 */
inline void fill_array64(uint64_t array[], int size)
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
    endian_check();
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
    endian_check();
    period_certification();
    initialized = 1;
}
