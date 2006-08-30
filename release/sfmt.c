/*
 * SIMD based Fast Mersenne Twister (SFMT)
 * written by M.Saito 2006.08.29
 * Copyright 2006
 */

#include <string.h>
#include <stdio.h>
#include "sfmt.h"

#define MEXP 19937

#define WORDSIZE 128
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

static uint32_t sfmt[N][4];
static uint32_t *psfmt = &sfmt[0][0];
static unsigned int idx;

#define POS1 122
#define SL1 18
#define SL2 1
#define SR1 11
#define SR2 1
#define MSK1 0xdfffffefU
#define MSK2 0xddfecb7fU
#define MSK3 0xbffaffffU
#define MSK4 0xbffffff6U
#define INIT_LUNG 0x6d736d6dU

struct W128_T {
    uint32_t a[4];
};

typedef struct W128_T w128_t;

INLINE unsigned int get_onetime_rnds(void) {
    return N * 4;
}

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

INLINE static void do_recursion(uint32_t r[4], uint32_t a[4], uint32_t b[4],
				uint32_t c[4], uint32_t d[4]) {
    uint32_t x[4];
    uint32_t y[4];

    lshift128(x, a, SL2);
    rshift128(y, c, SR2);
    r[0] = a[0] ^ x[0] ^ ((b[0] >> SR1) & MSK1) ^ y[0] ^ (d[0] << SL1);
    r[1] = a[1] ^ x[1] ^ ((b[1] >> SR1) & MSK2) ^ y[1] ^ (d[1] << SL1);
    r[2] = a[2] ^ x[2] ^ ((b[2] >> SR1) & MSK3) ^ y[2] ^ (d[2] << SL1);
    r[3] = a[3] ^ x[3] ^ ((b[3] >> SR1) & MSK4) ^ y[3] ^ (d[3] << SL1);
}

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

INLINE static void gen_rand_array(w128_t array[], uint32_t blocks) {
    int i, j;
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
    for (; i < N * (blocks - 1); i++) {
	do_recursion(array[i].a, array[i - N].a, array[i + POS1 - N].a, r1, r2);
	r1 = r2;
	r2 = array[i].a;
    }
    for (j = 0; i < N * blocks; i++, j++) {
	do_recursion(array[i].a, array[i - N].a, array[i + POS1 - N].a, r1, r2);
	r1 = r2;
	r2 = array[i].a;
	sfmt[j][0] = array[i].a[0];
	sfmt[j][1] = array[i].a[1];
	sfmt[j][2] = array[i].a[2];
	sfmt[j][3] = array[i].a[3];
    }
}

INLINE uint32_t gen_rand(void)
{
    uint32_t r;

    if (idx >= N * 4) {
	gen_rand_all();
	idx = 0;
    }
    r = psfmt[idx++];
    return r;
}

INLINE void fill_array_block(uint32_t array[], uint32_t block_num)
{
    if (block_num == 0) {
	return;
    } else if (block_num == 1) {
	gen_rand_all();
	memcpy(array, sfmt, sizeof(sfmt));
    } else {
	gen_rand_array((w128_t *)array, block_num);
    }
}

INLINE void init_gen_rand(uint32_t seed)
{
    int i;

    psfmt[0] = seed;
    for (i = 1; i < N * 4; i++) {
	psfmt[i] = 1812433253UL * (psfmt[i - 1] ^ (psfmt[i - 1] >> 30)) + i;
    }
    psfmt[3] = INIT_LUNG;
    idx = N * 4;
}
