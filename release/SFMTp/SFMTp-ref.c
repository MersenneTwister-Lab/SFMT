/** 
 * @file  SFMTp-ref.c
 * @brief SIMD-oriented Fast Mersenne Twister Pulmonary version (SFMTp)
 * reference code (not optimized).
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * @date 2007-01-04
 *
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */
#include <string.h>
#include <stdio.h>
#include "SFMTp.h"
#include "SFMTp-params.h"

/*--------------------------------------
  FILE GLOBAL VARIABLES
  internal state, index counter and flag 
  --------------------------------------*/
/** the 128-bit internal state array */
static uint32_t sfmt[N + 1][4];
/** the 32bit interger pointer to the 128-bit internal state array */
static uint32_t *sfmt32 = &sfmt[0][0];
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
static void lshift128(uint32_t out[4], const uint32_t in[4], int shift);
static void gen_rand_all(void);
static uint32_t func1(uint32_t x);
static uint32_t func2(uint32_t x);
static void xor128(uint32_t to[4], uint32_t from[4]);
static void period_certification(void);

/**
 * This function simulates SIMD 128-bit left shift by the standard C.
 * The 128-bit integer given in in[4] is shifted by (shift * 8) bits.
 * This function simulates the LITTLE ENDIAN SIMD.
 * @param out the output of this function
 * @param in the 128-bit data to be shifted
 * @param shift the shift value
 */
static void lshift128(uint32_t out[4], const uint32_t in[4], int shift) {
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
 * This function simulates SIMD 128-bit xor and assign by the standard C.
 * @param to the output of this function
 * @param from the 128-bit data to be assigned
 */
static void xor128(uint32_t to[4], uint32_t from[4]) {
    to[0] ^= from[0];
    to[1] ^= from[1];
    to[2] ^= from[2];
    to[3] ^= from[3];
}

/**
 * This function represents the recursion formula.
 * @param a a 128-bit part of the interal state array
 * @param b a 128-bit part of the interal state array
 * @param c a 128-bit part of the interal state array
 * @param d a 128-bit part of the interal state array
 */
static void do_recursion(uint32_t a[4], uint32_t b[4],
			 uint32_t c[4], uint32_t d[4]) {
    uint32_t x[4];

    lshift128(x, a, SL2);
    a[0] = a[0] ^ x[0] ^ ((b[0] >> SR1) & MSK1) ^ (c[0] >> SR2) ^ (c[0] << SL1)
	^ d[3];
    a[1] = a[1] ^ x[1] ^ ((b[1] >> SR1) & MSK2) ^ (c[1] >> SR2) ^ (c[1] << SL1)
	^ d[2];
    a[2] = a[2] ^ x[2] ^ ((b[2] >> SR1) & MSK3) ^ (c[2] >> SR2) ^ (c[2] << SL1)
	^ d[0];
    a[3] = a[3] ^ x[3] ^ ((b[3] >> SR1) & MSK4) ^ (c[3] >> SR2) ^ (c[3] << SL1)
	^ d[1];
}

/**
 * This function fills the internal state array with psedorandom
 * integers.
 */
static void gen_rand_all(void) {
    int i;

    for (i = 0; i < N; i++) {
	do_recursion(sfmt[i], sfmt[(i + POS1) % N], sfmt[(i + N -1) % N],
		     sfmt[N]);
	xor128(sfmt[N], sfmt[i]);
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
 * This function certificate the period of 2^{MEXP}
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
 * This function generates and returns 32-bit pseudorandom number.
 * init_gen_rand or init_by_array must be called before this function.
 * @return 32-bit pseudorandom number
 */
uint32_t gen_rand32(void)
{
    uint32_t r;

    if (idx >= N32) {
	gen_rand_all();
	idx = 0;
    }
    r = sfmt32[idx];
    idx++;
    return r;
} 

/**
 * This function initializes the internal state array with a 32-bit
 * integer seed.
 * @param seed a 32-bit integer used as the seed.
 */
void init_gen_rand(uint32_t seed)
{
    int i;

    sfmt32[0] = seed;
    for (i = 1; i < (N + 1) * 4; i++) {
	sfmt32[i] = 1812433253UL * (sfmt32[i - 1] ^ (sfmt32[i - 1] >> 30)) + i;
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
    r = func1(sfmt32[0] ^ sfmt32[mid % size] ^ sfmt32[size - 1]);
    sfmt32[mid % size] += r;
    r += key_length;
    sfmt32[(mid + lag) % size] += r;
    sfmt32[0] = r;
    count--;
    for (i = 1, j = 0; (j < count) && (j < key_length); j++) {
	r = func1(sfmt32[i] ^ sfmt32[(i + mid) % size] 
		  ^ sfmt32[(i + size - 1) % size]);
	sfmt32[(i + mid) % size] += r;
	r += init_key[j] + i;
	sfmt32[(i + mid + lag) % size] += r;
	sfmt32[i] = r;
	i = (i + 1) % size;
    }
    for (; j < count; j++) {
	r = func1(sfmt32[i] ^ sfmt32[(i + mid) % size] 
		  ^ sfmt32[(i + size - 1) % size]);
	sfmt32[(i + mid) % size] += r;
	r += i;
	sfmt32[(i + mid + lag) % size] += r;
	sfmt32[i] = r;
	i = (i + 1) % size;
    }
    for (j = 0; j < size; j++) {
	r = func2(sfmt32[i] + sfmt32[(i + mid) % size] 
		  + sfmt32[(i + size - 1) % size]);
	sfmt32[(i + mid) % size] ^= r;
	r -= i;
	sfmt32[(i + mid + lag) % size] ^= r;
	sfmt32[i] = r;
	i = (i + 1) % size;
    }

    idx = N32;
    period_certification();
    initialized = 1;
}

#if defined(MAIN)
int main(void) {
    int i;
    uint32_t ini[4] = {0x1234, 0x5678, 0x9abc, 0xdef0};

    init_gen_rand(1234);
    printf("init_gen_rand__________\n");
    for (i = 0; i < 1000; i++) {
	printf("%10u ", gen_rand32());
	if (i % 5 == 4) {
	    printf("\n");
	}
    }
    printf("\n");
    init_by_array(ini, 4);
    printf("init_by_array__________\n");
    for (i = 0; i < 1000; i++) {
	printf("%10u ", gen_rand32());
	if (i % 5 == 4) {
	    printf("\n");
	}
    }
    printf("\n");
    return 0;
}
#endif
