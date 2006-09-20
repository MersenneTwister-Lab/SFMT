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
#define NN (N + 1)
#define MAXDEGREE (WORDSIZE * NN)

static uint32_t sfmt[NN][4];
static uint32_t *psfmt = &sfmt[0][0];
static unsigned int idx;

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

INLINE void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    fprintf(fp, "SL2 = %u\n", SL2);
    fprintf(fp, "SR1 = %u\n", SR1);
    fprintf(fp, "SR2 = %u\n", SR2);
    fprintf(fp, "MSK1 = %08x\n", MSK1);
    fprintf(fp, "MSK2 = %08x\n", MSK2);
    fprintf(fp, "MSK3 = %08x\n", MSK3);
    fprintf(fp, "MSK4 = %08x\n", MSK4);
    fflush(fp);
}

INLINE void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SL3, SL4, SL5, SL6, SL7, SL8,"
	    " SR1, SR2, SR3, SR4] = "
	    "[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u]\n", 
	    POS1, SL1, SR1, MSK1, MSK2, MSK3, MSK4, 0, 0, 
	    0, 0, 0, 0);
    fflush(fp);
}

INLINE void print_state(FILE *fp) {
    int i, j;
    for (i = 0; i < N; i++) {
	for (j = 0; j < 4; j++) {
	    fprintf(fp, "%08x ", sfmt[i][j]);
	}
	if (i % 2 == 1) {
	    fprintf(fp, "\n");
	}
    }
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

INLINE static
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
void lshift128(uint32_t out[4], const uint32_t in[4],
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

INLINE static
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
    uint32_t lung[4];

    assign128(lung, sfmt[N]);
    do_recursion(sfmt[0], sfmt[0], sfmt[POS1], sfmt[N -1], lung);
    xor128(lung, sfmt[0]);
    for (i = 1; i < N - POS1; i++) {
	do_recursion(sfmt[i], sfmt[i], sfmt[i + POS1], sfmt[i - 1], lung);
	xor128(lung, sfmt[i]);
    }
    for (; i < N; i++) {
	do_recursion(sfmt[i], sfmt[i], sfmt[i + POS1 - N], sfmt[i - 1], lung);
	xor128(lung, sfmt[i]);
    }
    assign128(sfmt[N], lung);
}

INLINE static void gen_rand_array(w128_t array[], int size) {
    int i, j;
    uint32_t lung[4];

    assign128(lung, sfmt[N]);
    do_recursion(array[0].a, sfmt[0], sfmt[POS1], sfmt[N - 1], lung);
    xor128(lung, array[0].a);
    for (i = 1; i < N - POS1; i++) {
	do_recursion(array[i].a, sfmt[i], sfmt[i + POS1], array[i - 1].a, lung);
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

INLINE void fill_array(uint32_t array[], int size)
{
    assert(size % 4 == 0);
    assert(size >= 2 * N * 4);
    gen_rand_array((w128_t *)array, size / 4);
}

INLINE void init_gen_rand(uint32_t seed)
{
    int i;

    psfmt[0] = seed;
    for (i = 1; i < N * 4; i++) {
	psfmt[i] = 1812433253UL * (psfmt[i - 1] ^ (psfmt[i - 1] >> 30)) + i;
    }
    idx = N * 4;
}

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif
