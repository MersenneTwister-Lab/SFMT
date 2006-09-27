/* Simple and Fast Twister */

#include <string.h>
#if defined(__GNUC__)
  #include <stdint.h>
#endif
#include <stdio.h>
#include <assert.h>
#include "random-inline.h"

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 128
#define N (MEXP / WORDSIZE)
#define MAXDEGREE (WORDSIZE * (N + 1))

#define POS1 89
#define SL1 18
#define SL2 1
#define SR1 3
#define SR2 2
#define MSK1 0x7f7fffffU
#define MSK2 0xfffbffffU
#define MSK3 0xeffffbffU
#define MSK4 0xfefe7befU

struct W128_T {
    uint32_t a[4];
};

typedef struct W128_T w128_t;

static uint32_t sfmt[N + 1][4];
static uint32_t *psfmt = &sfmt[0][0];
static int idx;

INLINE unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

INLINE unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

INLINE unsigned int get_onetime_rnds(void) {
    return N * 4;
}

INLINE static
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
    out[0] = (uint32_t)ol;
    out[1] = (uint32_t)(ol >> 32);
    out[2] = (uint32_t)oh;
    out[3] = (uint32_t)(oh >> 32);
}

INLINE static
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    void do_recursion(uint32_t r[4], uint32_t a[4], uint32_t b[4],
		      uint32_t c[4], uint32_t d[4]) {
    uint32_t x[4];

    lshift128(x, a, SL2);
    r[0] = a[0] ^ x[0] ^ ((b[0] >> SR1) & MSK1) ^ (c[0] >> SR2)
	^ (c[0] << SL1) ^ d[3];
    r[1] = a[1] ^ x[1] ^ ((b[1] >> SR1) & MSK2) ^ (c[1] >> SR2)
	^ (c[1] << SL1) ^ d[2];
    r[2] = a[2] ^ x[2] ^ ((b[2] >> SR1) & MSK3) ^ (c[2] >> SR2)
	^ (c[2] << SL1) ^ d[0];
    r[3] = a[3] ^ x[3] ^ ((b[3] >> SR1) & MSK4) ^ (c[3] >> SR2)
	^ (c[3] << SL1) ^ d[1];
}

INLINE static void assign128(uint32_t to[4], uint32_t from[4]) {
	to[0] = from[0];
	to[1] = from[1];
	to[2] = from[2];
	to[3] = from[3];
}

INLINE static void xor128(uint32_t to[4], uint32_t from[4]) {
	to[0] ^= from[0];
	to[1] ^= from[1];
	to[2] ^= from[2];
	to[3] ^= from[3];
}

INLINE static void gen_rand_all(void) {
    int i;
    w128_t lung;

    assign128(lung.a, sfmt[N]);
    do_recursion(sfmt[0], sfmt[0], sfmt[POS1], sfmt[N -1], lung.a);
    xor128(lung.a, sfmt[0]);
    for (i = 1; i < N - POS1; i++) {
	do_recursion(sfmt[i], sfmt[i], sfmt[i + POS1], sfmt[i - 1], lung.a);
	xor128(lung.a, sfmt[i]);
    }
    for (; i < N; i++) {
	do_recursion(sfmt[i], sfmt[i], sfmt[i + POS1 - N], sfmt[i - 1],
		     lung.a);
	xor128(lung.a, sfmt[i]);
    }
    assign128(sfmt[N], lung.a);
}

INLINE static void gen_rand_array(w128_t array[], int size) {
    int i, j;
    uint32_t lung[4];

    assign128(lung, sfmt[N]);
    do_recursion(array[0].a, sfmt[0], sfmt[POS1], sfmt[N - 1], lung);
    xor128(lung, array[0].a);
    for (i = 1; i < N - POS1; i++) {
	do_recursion(array[i].a, sfmt[i], sfmt[i + POS1], array[i - 1].a,
		     lung);
	xor128(lung, array[i].a);
    }
    for (; i < N; i++) {
	do_recursion(array[i].a, sfmt[i], array[i + POS1 - N].a,
		     array[i - 1].a, lung);
	xor128(lung, array[i].a);
    }
    for (; i < size - N; i++) {
	do_recursion(array[i].a, array[i - N].a, array[i + POS1 - N].a,
		     array[i - 1].a, lung);
	xor128(lung, array[i].a);
    }
    for (j = 0; j < 2 * N - size; j++) {
	assign128(sfmt[j], array[j + size - N].a);
    }
    for (; i < size; i++, j++) {
	do_recursion(array[i].a, array[i - N].a, array[i + POS1 - N].a,
		     array[i - 1].a, lung);
	xor128(lung, array[i].a);
	assign128(sfmt[j], array[i].a);
    }
    assign128(sfmt[N], lung);
}

INLINE
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    uint32_t gen_rand(void)
{
    uint32_t r;

    if (idx >= N * 4) {
	gen_rand_all();
	idx = 0;
    }
    r = psfmt[idx++];
    return r;
}

INLINE
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    void fill_array(uint32_t array[], int size)
{
    assert(size % 4 == 0);
    assert(size >= 2 * N * 4);
    gen_rand_array((w128_t *)array, size / 4);
}

INLINE void init_gen_rand(uint32_t seed)
{
    int i;

    psfmt[0] = seed;
    for (i = 1; i < (N + 1) * 4; i++) {
	psfmt[i] = 1812433253UL * (psfmt[i - 1] ^ (psfmt[i - 1] >> 30)) + i;
    }
    idx = N * 4;
}

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif
