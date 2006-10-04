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
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

#define POS1 4
#define SL1 20
#define SL2 1
#define SR1 1
#define SR2 1
#define MSK1 0xfffef7f9U
#define MSK2 0xffdff7fbU
#define MSK3 0xcbfff7feU
#define MSK4 0xedfefffdU

static uint32_t sfmt[N][4];
static uint32_t *psfmt = &sfmt[0][0];
static unsigned int idx;
static const uint32_t mask[4] = {MSK1, MSK2, MSK3, MSK4};

struct W128_T {
    uint32_t a[4];
};
typedef struct W128_T w128_t;

INLINE static
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
#if defined(LITTLE_ENDIAN)
void rshift128(uint32_t *out, const uint32_t *in, int shift) {
    uint64_t *op;
    uint64_t *ip;

    op = (uint64_t *)out;
    ip = (uint64_t *)in;
    op[0] = ip[0];
    op[1] = ip[1];
    op[0] = op[0] >> (shift * 8);
    op[1] = op[1] >> (shift * 8);
    op[0] |= ip[1] << (64 - shift * 8);
}
#else
 void rshift128(uint32_t out[4], const uint32_t in[4], int shift) {
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
#endif

INLINE static
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
#if defined(LITTLE_ENDIAN)
void lshift128(uint32_t *out, const uint32_t *in, int shift) {
    uint64_t *op;
    uint64_t *ip;

    op = (uint64_t *)out;
    ip = (uint64_t *)in;
    op[0] = ip[0];
    op[1] = ip[1];
    op[0] = op[0] << (shift * 8);
    op[1] = op[1] << (shift * 8);
    op[1] |= ip[0] >> (64 - shift * 8);
}
#else
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
#endif

INLINE static
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    void do_recursion(uint32_t r[4], uint32_t a[4], uint32_t b[4],
		      uint32_t c[4], uint32_t d[4]) {
    uint32_t x[4];
    uint32_t y[4];

    lshift128(x, a, SL2);
    rshift128(y, c, SR2);
    r[0] = a[0] ^ x[0] ^ ((b[0] >> SR1) & mask[0]) ^ y[0] ^ (d[0] << SL1);
    r[1] = a[1] ^ x[1] ^ ((b[1] >> SR1) & mask[1]) ^ y[1] ^ (d[1] << SL1);
    r[2] = a[2] ^ x[2] ^ ((b[2] >> SR1) & mask[2]) ^ y[2] ^ (d[2] << SL1);
    r[3] = a[3] ^ x[3] ^ ((b[3] >> SR1) & mask[3]) ^ y[3] ^ (d[3] << SL1);
}

INLINE static void gen_rand_all(void) {
    int i;

    do_recursion(sfmt[0], sfmt[0], sfmt[POS1], sfmt[N - 2], sfmt[N - 1]);
    do_recursion(sfmt[1], sfmt[1], sfmt[1 + POS1], sfmt[N - 1], sfmt[0]);
    for (i = 2; i < N - POS1; i++) {
	do_recursion(sfmt[i], sfmt[i], sfmt[i + POS1], sfmt[i - 2],
		     sfmt[i - 1]);
    }
    for (; i < N; i++) {
	do_recursion(sfmt[i], sfmt[i], sfmt[i + POS1 - N], sfmt[i - 2],
		     sfmt[i - 1]);
    }
}

INLINE static void gen_rand_array(w128_t array[], int size) {
    int i, j;

    do_recursion(array[0].a, sfmt[0], sfmt[POS1], sfmt[N - 2], sfmt[N - 1]);
    do_recursion(array[1].a, sfmt[1], sfmt[1 + POS1], sfmt[N - 1], array[0].a);
    for (i = 2; i < N - POS1; i++) {
	do_recursion(array[i].a, sfmt[i], sfmt[i + POS1], array[i - 2].a,
		     array[i -1].a);
    }
    for (; i < N; i++) {
	do_recursion(array[i].a, sfmt[i], array[i + POS1 - N].a, array[i - 2].a,
		     array[i - 1].a);
    }
    for (; i < size - N; i++) {
	do_recursion(array[i].a, array[i - N].a, array[i + POS1 - N].a, 
		     array[i - 2].a, array[i - 1].a);
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j][0] = array[j + size - N].a[0];
	sfmt[j][1] = array[j + size - N].a[1];
	sfmt[j][2] = array[j + size - N].a[2];
	sfmt[j][3] = array[j + size - N].a[3];
    }
    for (; i < size; i++, j++) {
	do_recursion(array[i].a, array[i - N].a, array[i + POS1 - N].a,
		     array[i - 2].a, array[i - 1].a);
	sfmt[j][0] = array[i].a[0];
	sfmt[j][1] = array[i].a[1];
	sfmt[j][2] = array[i].a[2];
	sfmt[j][3] = array[i].a[3];
    }
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
