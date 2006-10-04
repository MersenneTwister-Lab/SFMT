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

struct W128_T {
    uint32_t a[4];
};
typedef struct W128_T w128_t;

static w128_t sfmt[N];
static uint32_t *psfmt = (uint32_t *)&sfmt[0];
static unsigned int idx;

INLINE unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

INLINE unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

INLINE static
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
#if defined(LITTLE_ENDIAN)
void rshift128(w128_t *out, const w128_t *in, int shift) {
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
 void rshift128(w128_t *out, const w128_t *in, int shift) {
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in->a[3] << 32) | ((uint64_t)in->a[2]);
    tl = ((uint64_t)in->a[1] << 32) | ((uint64_t)in->a[0]);

    oh = th >> (shift * 8);
    ol = tl >> (shift * 8);
    ol |= th << (64 - shift * 8);
    out->a[1] = (uint32_t)(ol >> 32);
    out->a[0] = (uint32_t)ol;
    out->a[3] = (uint32_t)(oh >> 32);
    out->a[2] = (uint32_t)oh;
}
#endif

INLINE static
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
#if defined(LITTLE_ENDIAN)
void lshift128(w128_t *out, const w128_t *in, int shift) {
    uint64_t *op;
    uint64_t *ip;

    op = (uint64_t *)out->a;
    ip = (uint64_t *)in->a;
    op[0] = ip[0];
    op[1] = ip[1];
    op[0] = op[0] << (shift * 8);
    op[1] = op[1] << (shift * 8);
    op[1] |= ip[0] >> (64 - shift * 8);
}
#else
 void lshift128(w128_t *out, const w128_t *in, int shift) {
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in->a[3] << 32) | ((uint64_t)in->a[2]);
    tl = ((uint64_t)in->a[1] << 32) | ((uint64_t)in->a[0]);

    oh = th << (shift * 8);
    ol = tl << (shift * 8);
    oh |= tl >> (64 - shift * 8);
    out->a[1] = (uint32_t)(ol >> 32);
    out->a[0] = (uint32_t)ol;
    out->a[3] = (uint32_t)(oh >> 32);
    out->a[2] = (uint32_t)oh;
}
#endif

INLINE static
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *c, w128_t *d) {
    w128_t x;
    w128_t y;

    lshift128(&x, a, SL2);
    rshift128(&y, c, SR2);
    r->a[0] = a->a[0] ^ x.a[0] ^ ((b->a[0] >> SR1) & MSK1) ^ y.a[0] 
	^ (d->a[0] << SL1);
    r->a[1] = a->a[1] ^ x.a[1] ^ ((b->a[1] >> SR1) & MSK2) ^ y.a[1]
	^ (d->a[1] << SL1);
    r->a[2] = a->a[2] ^ x.a[2] ^ ((b->a[2] >> SR1) & MSK3) ^ y.a[2]
	^ (d->a[2] << SL1);
    r->a[3] = a->a[3] ^ x.a[3] ^ ((b->a[3] >> SR1) & MSK4) ^ y.a[3]
	^ (d->a[3] << SL1);
}

INLINE static void gen_rand_all(void) {
    int i;

    do_recursion(&sfmt[0], &sfmt[0], &sfmt[POS1], &sfmt[N - 2], &sfmt[N - 1]);
    do_recursion(&sfmt[1], &sfmt[1], &sfmt[1 + POS1], &sfmt[N - 1], &sfmt[0]);
    for (i = 2; i < N - POS1; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + POS1], &sfmt[i - 2],
		     &sfmt[i - 1]);
    }
    for (; i < N; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + POS1 - N], &sfmt[i - 2],
		     &sfmt[i - 1]);
    }
}

INLINE static void gen_rand_array(w128_t array[], int size) {
    int i, j;

    do_recursion(&array[0], &sfmt[0], &sfmt[POS1], &sfmt[N - 2], &sfmt[N - 1]);
    do_recursion(&array[1], &sfmt[1], &sfmt[1 + POS1], &sfmt[N - 1], &array[0]);
    for (i = 2; i < N - POS1; i++) {
	do_recursion(&array[i], &sfmt[i], &sfmt[i + POS1], &array[i - 2],
		     &array[i -1]);
    }
    for (; i < N; i++) {
	do_recursion(&array[i], &sfmt[i], &array[i + POS1 - N], &array[i - 2],
		     &array[i - 1]);
    }
    for (; i < size - N; i++) {
	do_recursion(&array[i], &array[i - N], &array[i + POS1 - N], 
		     &array[i - 2], &array[i - 1]);
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j] = array[j + size - N];
    }
    for (; i < size; i++, j++) {
	do_recursion(&array[i], &array[i - N], &array[i + POS1 - N],
		     &array[i - 2], &array[i - 1]);
	sfmt[j] = array[i];
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
