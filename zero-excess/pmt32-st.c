/** 
 * @file  pmt32-st.c
 * @brief Pulmonary Mersenne Twister(PMT) pseudorandom number generator.
 * This version uses the data structure.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * @date 2006-09-14
 *
 * Copyright (C) 2006 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */
#include <string.h>
#include <assert.h>
#include "params32.h"
#include "pmt32-st.h"

/*----------------
  STATIC FUNCTIONS
  ----------------*/
static void gen_rand_all(pmt32_t *pmt);
static uint32_t func1(uint32_t x);
static uint32_t func2(uint32_t x);

/*----------------------------------
  IMPLEMENTATION OF PUBLIC FUNCTIONS
  ----------------------------------*/
/**
 * This function returns the identification string.
 * For example, "PMT32-521:9-4-9-3-15:4d4e4850".
 * The string shows the word size, the mersenne expornent,
 * and all parameters to identify this generator.
 */
char *get_idstring(void)
{
    return IDSTRING;
}

/**
 * This function fills the internal state array with psedorandom
 * integers.
 * @param pmt the pointer to the data structure.
 */
static void gen_rand_all(pmt32_t *pmt) {
    uint32_t u;
    int i;
  
    u = pmt->gx[N];
    for (i = 0; i < N - GMM; i++) {
	u ^= (pmt->gx[i] >> GROT1) ^ (pmt->gx[i] << GROT2);
	u ^= pmt->gx[i + GMM];
	u ^= u << GS2;
	pmt->gx[i] ^=  u ^ (u << GS3);
    }
    for (; i < N; i++) {
	u ^= (pmt->gx[i] >> GROT1) ^ (pmt->gx[i] << GROT2);
	u ^= pmt->gx[i + GMM - N];
	u ^= u << GS2;
	pmt->gx[i] ^=  u ^ (u << GS3);
    }
    pmt->gx[N] = u;
}

/**
 * This function generates and returns 32-bit pseudorandom number.
 * The data structure must be initialized by init_gen_rand or
 * init_by_array before this function called.
 * @param pmt the pointer to the data structure of pmt32.
 * @return 32-bit pseudorandom number
 */
uint32_t gen_rand_int32(pmt32_t *pmt)
{
    if (pmt->idx >= N) {
	gen_rand_all(pmt);
	pmt->idx = 0;
    }
    return pmt->gx[pmt->idx++];
}

/**
 * This function initializes the data strucure with a 32-bit
 * integer seed.
 * @param pmt the pointer to the data structure of pmt32.
 * @param seed a 32-bit integer used as the seed.
 */
void init_gen_rand(pmt32_t *pmt, uint32_t seed)
{
    int i;
    pmt->gx[0] = seed;
    for (i = 1; i < N; i++) {
	pmt->gx[i] = (1812433253UL * (pmt->gx[i - 1] ^ (pmt->gx[i - 1] >> 30))
		      + i); 
    }
    pmt->gx[N] = INITIAL_LUNG;
    pmt->idx = N;
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
 * This function initializes the data structure with an array of
 * 32-bit integers used as the seeds
 * @param pmt the pointer to the data structure of pmt32.
 * @param init_key the array of 32-bit integers, used as a seed.
 * @param key_length the length of init_key.
 */
void init_by_array(pmt32_t *pmt, uint32_t init_key[], int key_length) {
    int i, j, count;
    uint32_t r;
    const int MID = 306;
    const int LAG = 11;

    memset(pmt->gx, 0x8b, sizeof(pmt->gx));
    if (key_length + 1 > N) {
	count = key_length + 1;
    } else {
	count = N;
    }
    r = func1(pmt->gx[0] ^ pmt->gx[MID] ^ pmt->gx[N - 1]);
    pmt->gx[MID] += r;
    r += key_length;
    pmt->gx[MID + LAG] = r;
    pmt->gx[0] = r;
    count--;
    for (i = 1, j = 0; (j < count) && (j < key_length); j++) {
	r = func1(pmt->gx[i] ^ pmt->gx[(i + MID) % N] 
		  ^ pmt->gx[(i + N - 1) % N]);
	pmt->gx[(i + MID) % N] += r;
	r += init_key[j] + i;
	pmt->gx[(i + MID + LAG) % N] += r;
	pmt->gx[i] = r;
	i = (i + 1) % N;
    }
    for (; j < count; j++) {
	r = func1(pmt->gx[i] ^ pmt->gx[(i + MID) % N] 
		  ^ pmt->gx[(i + N - 1) % N]);
	pmt->gx[(i + MID) % N] += r;
	r += i;
	pmt->gx[(i + MID + LAG) % N] = r;
	pmt->gx[i] = r;
	i = (i + 1) % N;
    }
    for (j = 0; j < N; j++) {
	r = func2(pmt->gx[i] + pmt->gx[(i + MID) % N] 
		  + pmt->gx[(i + N - 1) % N]);
	pmt->gx[(i + MID) % N] ^= r;
	r -= i;
	pmt->gx[(i + MID + LAG) % N] ^= r;
	pmt->gx[i] = r;
	i = (i + 1) % N;
    }

    pmt->gx[N] = INITIAL_LUNG;
    pmt->idx = N;
}


