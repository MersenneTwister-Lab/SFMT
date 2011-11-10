/** 
 * @file dSFMT-ref.c 
 * @brief double precision SIMD-oriented Fast Mersenne Twister (dSFMT)
 * based on IEEE 754 format.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2007, 2008 Mutsuo Saito, Makoto Matsumoto and
 * Hiroshima University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "dSFMT-params.h"

#if 0
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

const char *get_idstring(void);
int get_min_array_size(void);
void init_gen_rand(uint32_t seed);

#if 0
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

#endif

/*--------------------------------------
  FILE GLOBAL VARIABLES
  internal state, index counter and flag 
  --------------------------------------*/
/** the 128-bit internal state array */
static w128_t sfmt[DSFMT_N + 1];
/** the double pointer to the 128-bit internal state array */
static double *psfmt64 = &sfmt[0].d[0];
/** index counter to the internal state array as double */
static int sfmt_idx;
/** a flag: it is 0 if and only if the internal state is not yet
 * initialized. */
static int is_sfmt_initialized = 0;
/** a period certification vector which certificate the period of 2^{MEXP}-1. */
static uint64_t sfmt_pcv[2] = {DSFMT_PCV1, DSFMT_PCV2};

/*----------------
  STATIC FUNCTIONS
  ----------------*/
static void gen_rand_all(void);
static void gen_rand_array(w128_t array[], int size);
static void initial_mask(void);
static void period_certification(void);

/**
 * This function represents the recursion formula.
 * @param r output 128-bit
 * @param a a 128-bit part of the internal state array
 * @param b a 128-bit part of the internal state array
 * @param c a 128-bit part of the internal state array
 * @param lung a 128-bit part of the internal state array (I/O)
 */
inline static void do_recursion(w128_t *r, w128_t *a, w128_t * b,
				w128_t *lung) {
    uint64_t t0, t1, L0, L1;

    t0 = a->u[0];
    t1 = a->u[1];
    L0 = lung->u[0];
    L1 = lung->u[1];
    lung->u[0] = (t0 << DSFMT_SL1) ^ (L1 >> 32) ^ (L1 << 32) ^ b->u[0];
    lung->u[1] = (t1 << DSFMT_SL1) ^ (L0 >> 32) ^ (L0 << 32) ^ b->u[1];
    r->u[0] = (lung->u[0] >> DSFMT_SR) ^ (lung->u[0] & DSFMT_MSK1) ^ t0;
    r->u[1] = (lung->u[1] >> DSFMT_SR) ^ (lung->u[1] & DSFMT_MSK2) ^ t1;
}

/**
 * This function fills the internal state array with double precision
 * floating point pseudorandom numbers of the IEEE 754 format.
 */
inline static void gen_rand_all(void) {
    int i;
    w128_t lung;

    lung = sfmt[DSFMT_N];
    do_recursion(&sfmt[0], &sfmt[0], &sfmt[DSFMT_POS1], &lung);
    for (i = 1; i < DSFMT_N - DSFMT_POS1; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + DSFMT_POS1], &lung);
    }
    for (; i < DSFMT_N; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + DSFMT_POS1 - DSFMT_N],
		     &lung);
    }
    sfmt[DSFMT_N] = lung;
}

/**
 * This function initializes the internal state array to fit the IEEE
 * 754 format.
 * @param dsfmt dsfmt state vector.
 */
static void initial_mask() {
    int i;
    uint64_t *psfmt;

    psfmt = &sfmt[0].u[0];
    for (i = 0; i < DSFMT_N * 2; i++) {
        psfmt[i] = (psfmt[i] & DSFMT_LOW_MASK) | DSFMT_HIGH_CONST;
    }
}

/**
 * This function certificate the period of 2^{SFMT_MEXP}-1.
 * @param dsfmt dsfmt state vector.
 */
static void period_certification() {
    int i, j;
    uint64_t pcv[2] = {DSFMT_PCV1, DSFMT_PCV2};
    uint64_t tmp[2];
    uint64_t inner, work;

    tmp[0] = (sfmt[DSFMT_N].u[0] ^ DSFMT_FIX1);
    tmp[1] = (sfmt[DSFMT_N].u[1] ^ DSFMT_FIX2);

    inner = tmp[0] & pcv[0];
    inner ^= tmp[1] & pcv[1];
    for (i = 32; i > 0; i >>= 1) {
        inner ^= inner >> i;
    }
    inner &= 1;
    /* check OK */
    if (inner == 1) {
	return;
    }
    /* check NG, and modification */
    for (i = 1; i >= 0; i--) {
	work = 1;
	for (j = 0; j < 64; j++) {
	    if ((work & pcv[i]) != 0) {
		sfmt[DSFMT_N].u[i] ^= work;
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
 * This function generates and returns double precision pseudorandom
 * number which distributes uniformly in the range [1, 2).
 * init_gen_rand() or init_by_array() must be called before this
 * function.
 * @return double precision floating point pseudorandom number
 */
inline double genrand_close1_open2(void) {
    double r;

    assert(is_sfmt_initialized);

    if (sfmt_idx >= DSFMT_N * 2) {
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
    for (i = 1; i < (DSFMT_N + 1) * 4; i++) {
	psfmt[sfmt_idxof(i)] = 1812433253UL 
	    * (psfmt[sfmt_idxof(i - 1)] 
	       ^ (psfmt[sfmt_idxof(i - 1)] >> 30)) + i;
    }
    initial_mask();
    period_certification();
    sfmt_idx = DSFMT_N64;
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
    int size = (DSFMT_N + 1) * 4;	/* pulmonary */


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
    sfmt_idx = DSFMT_N64;
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
