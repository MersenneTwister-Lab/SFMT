/** 
 * @file dSFMT-ref.c 
 * @brief double precision SIMD-oriented Fast Mersenne Twister (dSFMT)
 * based on IEEE 754 format.
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
#include <stdio.h>
#include "dSFMT-params.h"

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#  include <inttypes.h>
#elif defined(_MSC_VER)
  typedef unsigned int uint32_t;
  typedef unsigned long long uint64_t;
#else
#  include <inttypes.h>
#endif

#ifndef PRIu64
#  if defined(_MSC_VER)
#    define PRIu64 "I64u"
#    define PRIx64 "I64x"
#  else
#    define PRIu64 "llu"
#    define PRIx64 "llx"
#  endif
#endif

#if !defined(UINT64_C)
#  define UINT64_C(v) (v ## ULL) 
#endif

#if defined(__ppc__)
/**
 * This function simulate a 64-bit index of LITTLE ENDIAN 
 * in BIG ENDIAN machine.
 */
inline static int sfmt_idxof(int i) {
    return i ^ 1;
}
#else
inline static int sfmt_idxof(int i) {
    return i;
}
#endif

double genrand_close1_open2(void);

char *get_idstring(void);
int get_min_array_size(void);
void init_gen_rand(uint32_t seed);

/*------------------------------------------
  128-bit SIMD like data type for standard C
  ------------------------------------------*/
/** 128-bit data structure */
union W128_T {
    uint64_t u[2];
    uint32_t u32[4];
    double d[2];
};

/** 128-bit data type */
typedef union W128_T w128_t;

/*--------------------------------------
  FILE GLOBAL VARIABLES
  internal state, index counter and flag 
  --------------------------------------*/
/** the 128-bit internal state array */
static w128_t sfmt[SFMT_N + 1];
/** the double pointer to the 128-bit internal state array */
static double *psfmt64 = &sfmt[0].d[0];
/** index counter to the internal state array as double */
static int sfmt_idx;
/** a flag: it is 0 if and only if the internal state is not yet
 * initialized. */
static int is_sfmt_initialized = 0;
/** a period certification vector which certificate the period of 2^{MEXP}-1. */
static uint64_t sfmt_pcv[2] = {SFMT_PCV1, SFMT_PCV2};

/*----------------
  STATIC FUNCTIONS
  ----------------*/
static void lshift128(w128_t *out, const w128_t *in, int shift);
static void gen_rand_all(void);
static void gen_rand_array(w128_t array[], int size);
static void initial_mask(void);
static void period_certification(void);

/**
 * This function simulates SIMD 128-bit left shift by the standard C.
 * The 128-bit integer given in \b in is shifted by (shift * 8) bits.
 * This function simulates the LITTLE ENDIAN SIMD.
 * @param out the output of this function
 * @param in the 128-bit data to be shifted
 * @param shift the shift value
 */
inline static void lshift128(w128_t *out, const w128_t *in, int shift) {
    out->u[0] = in->u[0] << (shift * 8);
    out->u[1] = in->u[1] << (shift * 8);
    out->u[1] |= in->u[0] >> (64 - shift * 8);
}

/**
 * This function represents the recursion formula.
 * @param r output
 * @param a a 128-bit part of the internal state array
 * @param b a 128-bit part of the internal state array
 * @param c a 128-bit part of the internal state array
 * @param lung a 128-bit part of the internal state array
 */
inline static void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *c,
				w128_t *lung) {
    w128_t x;

    lshift128(&x, a, SFMT_SL2);
    r->u[0] = a->u[0] ^ x.u[0] ^ ((b->u[0] >> SFMT_SR1) & SFMT_MSK1) 
	^ (c->u[0] >> SFMT_SR2) ^ (c->u[0] << SFMT_SL1) ^ lung->u[1];
    r->u[1] = a->u[1] ^ x.u[1] ^ ((b->u[1] >> SFMT_SR1) & SFMT_MSK2) 
	^ (c->u[1] >> SFMT_SR2) ^ (c->u[1] << SFMT_SL1) ^ lung->u[0];
    r->u[0] &= SFMT_LOW_MASK;
    r->u[1] &= SFMT_LOW_MASK;
    lung->u[0] ^= r->u[0];
    lung->u[1] ^= r->u[1];
    r->u[0] |= SFMT_HIGH_CONST;
    r->u[1] |= SFMT_HIGH_CONST;
}

/**
 * This function fills the internal state array with double precision
 * floating point pseudorandom numbers of the IEEE 754 format.
 */
inline static void gen_rand_all(void) {
    int i;
    w128_t lung;

    lung = sfmt[SFMT_N];
    do_recursion(&sfmt[0], &sfmt[0], &sfmt[SFMT_POS1], &sfmt[SFMT_N -1], &lung);
    for (i = 1; i < SFMT_N - SFMT_POS1; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + SFMT_POS1], &sfmt[i - 1],
		     &lung);
    }
    for (; i < SFMT_N; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + SFMT_POS1 - SFMT_N],
		     &sfmt[i - 1], &lung);
    }
    sfmt[SFMT_N] = lung;
}

/**
 * This function initializes the internal state array to fit the IEEE
 * 754 format.
 */
void initial_mask(void) {
    int i;
    uint64_t *psfmt;

    psfmt = &sfmt[0].u[0];
    for (i = 0; i < (SFMT_N + 1) * 2; i++) {
        psfmt[i] = (psfmt[i] & SFMT_LOW_MASK) | SFMT_HIGH_CONST;
    }
}

/**
 * This function certificate the period of 2^{MEXP}-1.
 */
static void period_certification() {
    int inner = 0;
    int i, j;
    uint64_t new[2];
    uint64_t work;
    uint64_t fix[2];

    fix[0] = (((SFMT_HIGH_CONST >> SFMT_SR1) & SFMT_MSK2) 
	      ^ (SFMT_HIGH_CONST >> SFMT_SR2)) | SFMT_HIGH_CONST;
    fix[1] = (((SFMT_HIGH_CONST >> SFMT_SR1) & SFMT_MSK1) 
	      ^ (SFMT_HIGH_CONST >> SFMT_SR2)) | SFMT_HIGH_CONST;
    fix[0] = fix[0] ^ (SFMT_HIGH_CONST >> (64 - 8 * SFMT_SL2));
    new[0] = sfmt[SFMT_N].u[0] ^ fix[0];
    new[1] = sfmt[SFMT_N].u[1] ^ fix[1];
    for (i = 0; i < 2; i++) {
	work = new[i] & sfmt_pcv[i];
	for (j = 0; j < 52; j++) {
	    inner ^= work & 1;
	    work = work >> 1;
	}
    }
    /* check OK, the period is 2^{MEXP}-1 */
    if (inner == 1) {
	return;
    }
    /* check NG, the period may not be 2^{MEXP}-1 then modify */
    for (i = 0; i < 2; i++) {
	work = 1;
	for (j = 0; j < 52; j++) {
	    if ((work & sfmt_pcv[i]) != 0) {
		sfmt[SFMT_N].u[i] ^= work;
		return;
	    }
	    work = work << 1;
	}
    }
}

/**
 * This function represents a function used in the initialization
 * by init_by_array
 * @param x 32-bit integer
 * @return 32-bit integer
 */
static uint32_t ini_func1(uint32_t x) {
    return (x ^ (x >> 27)) * (uint32_t)1664525UL;
}

/**
 * This function represents a function used in the initialization
 * by init_by_array
 * @param x 32-bit integer
 * @return 32-bit integer
 */
static uint32_t ini_func2(uint32_t x) {
    return (x ^ (x >> 27)) * (uint32_t)1566083941UL;
}

/*----------------
  PUBLIC FUNCTIONS
  ----------------*/
/**
 * This function returns the identification string.  The string shows
 * the Mersenne exponent, and all parameters of this generator.
 * @return id string.
 */
char *get_idstring(void) {
    return SFMT_IDSTR;
}

/**
 * This function returns the minimum size of array used for \b
 * fill_array functions.
 * @return minimum size of array used for fill_array functions.
 */
int get_min_array_size(void) {
    return SFMT_N64;
}

/**
 * This function generates and returns double precision pseudorandom
 * number which distributes uniformly in the range [1, 2).
 * init_gen_rand() or init_by_array() must be called before this
 * function.
 * @return double precision floating point pseudorandom number
 */
inline double genrand_close1_open2(void) {
    double r;

    assert(is_sfmt_initialized);

    if (sfmt_idx >= SFMT_N * 2) {
	gen_rand_all();
	sfmt_idx = 0;
    }
    r = psfmt64[sfmt_idx++];
    return r;
}

/**
 * This function initializes the internal state array with a 32-bit
 * integer seed.
 * @param seed a 32-bit integer used as the seed.
 */
void init_gen_rand(uint32_t seed) {
    int i;
    uint32_t *psfmt;

    psfmt = (uint32_t *)&sfmt[0];
    psfmt[sfmt_idxof(0)] = seed;
    for (i = 1; i < (SFMT_N + 1) * 4; i++) {
	psfmt[sfmt_idxof(i)] = 1812433253UL 
	    * (psfmt[sfmt_idxof(i - 1)] 
	       ^ (psfmt[sfmt_idxof(i - 1)] >> 30)) + i;
    }
    initial_mask();
    period_certification();
    sfmt_idx = SFMT_N64;
    is_sfmt_initialized = 1;
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
    uint32_t *psfmt32;
    int lag;
    int mid;
    int size = (SFMT_N + 1) * 4;	/* pulmonary */


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

    psfmt32 = &sfmt[0].u32[0];
    memset(sfmt, 0x8b, sizeof(sfmt));
    if (key_length + 1 > size) {
	count = key_length + 1;
    } else {
	count = size;
    }
    r = ini_func1(psfmt32[sfmt_idxof(0)] ^ psfmt32[sfmt_idxof(mid % size)] 
		  ^ psfmt32[sfmt_idxof((size - 1) % size)]);
    psfmt32[sfmt_idxof(mid % size)] += r;
    r += key_length;
    psfmt32[sfmt_idxof((mid + lag) % size)] += r;
    psfmt32[sfmt_idxof(0)] = r;
    i = 1;
    count--;
    for (i = 1, j = 0; (j < count) && (j < key_length); j++) {
	r = ini_func1(psfmt32[sfmt_idxof(i)] 
		      ^ psfmt32[sfmt_idxof((i + mid) % size)] 
		      ^ psfmt32[sfmt_idxof((i + size - 1) % size)]);
	psfmt32[sfmt_idxof((i + mid) % size)] += r;
	r += init_key[j] + i;
	psfmt32[sfmt_idxof((i + mid + lag) % size)] += r;
	psfmt32[sfmt_idxof(i)] = r;
	i = (i + 1) % size;
    }
    for (; j < count; j++) {
	r = ini_func1(psfmt32[sfmt_idxof(i)] 
		      ^ psfmt32[sfmt_idxof((i + mid) % size)] 
		      ^ psfmt32[sfmt_idxof((i + size - 1) % size)]);
	psfmt32[sfmt_idxof((i + mid) % size)] += r;
	r += i;
	psfmt32[sfmt_idxof((i + mid + lag) % size)] += r;
	psfmt32[sfmt_idxof(i)] = r;
	i = (i + 1) % size;
    }
    for (j = 0; j < size; j++) {
	r = ini_func2(psfmt32[sfmt_idxof(i)] 
		      + psfmt32[sfmt_idxof((i + mid) % size)] 
		      + psfmt32[sfmt_idxof((i + size - 1) % size)]);
	psfmt32[sfmt_idxof((i + mid) % size)] ^= r;
	r -= i;
	psfmt32[sfmt_idxof((i + mid + lag) % size)] ^= r;
	psfmt32[sfmt_idxof(i)] = r;
	i = (i + 1) % size;
    }
    initial_mask();
    period_certification();
    sfmt_idx = SFMT_N64;
    is_sfmt_initialized = 1;
}

#if defined(MAIN)
int main(int argc, char *argv[]) {
    int i;
    double d;
    uint32_t ar[4] = {1, 2, 3, 4};

    if (argc <= 1) {
	printf("%s\n", get_idstring());
	printf("generated randoms [1, 2)\n");
	init_gen_rand(1234);
	for (i = 0; i < 1000; i++) {
	    d = genrand_close1_open2();
	    printf("%1.15f ", d);
	    if (i % 4 == 3) {
		printf("\n");
	    }
	}
    } else {			/* option -ar */
	printf("%s\n", get_idstring());
	printf("init_by_array\n");
	printf("generated randoms [1, 2)\n");
	init_by_array(ar, 4);
	for (i = 0; i < 1000; i++) {
	    d = genrand_close1_open2();
	    printf("%1.15f ", d);
	    if (i % 4 == 3) {
		printf("\n");
	    }
	}
    }
    return 0;
}
#endif
