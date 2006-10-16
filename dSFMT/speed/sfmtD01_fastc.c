/* Simple and Fast Twister */

#include <string.h>
#if defined(__GNUC__)
  #include <stdint.h>
#endif
#include <stdio.h>
#include <assert.h>
#include "random.h"

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 104
#define N (MEXP / WORDSIZE)
#define MAXDEGREE (WORDSIZE * (N + 1))

#define LOW_MASK  ((uint64_t)0x000FFFFFFFFFFFFFULL)
#define HIGH_CONST ((uint64_t)0xBFF0000000000000ULL)
//#define HIGH_CONST ((uint64_t)0x0000000000000ULL)

#define POS1 89
#define SL1 18
#define SL2 1
#define SR1 3
#define SR2 2
#define MSK1 0x7f7fffffU
#define MSK2 0xfffbffffU

union W128_T {
    uint64_t a[2];
    double d[2];
};
typedef union W128_T w128_t;

static w128_t sfmt[N + 1];
static double *psfmt = (double *)&(sfmt[0].d[0]);
static int idx;

INLINE static
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
void lshift128(w128_t *out, const w128_t *in, int shift) {
    out->a[0] = in->a[0] << (shift * 8);
    out->a[1] = in->a[1] << (shift * 8);
    out->a[1] |= in->a[0] >> (64 - shift * 8);
}

INLINE static
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *c, w128_t *lung){
    w128_t x;

    lshift128(&x, a, SL2);
    r->a[0] = a->a[0] ^ x.a[0] ^ ((b->a[0] >> SR1) & MSK1) ^ (c->a[0] >> SR2)
	^ (c->a[0] << SL1) ^ lung->a[1];
    r->a[1] = a->a[1] ^ x.a[1] ^ ((b->a[1] >> SR1) & MSK2) ^ (c->a[1] >> SR2)
	^ (c->a[1] << SL1) ^ lung->a[0];
    r->a[0] = (r->a[0] & LOW_MASK) | HIGH_CONST;
    r->a[1] = (r->a[1] & LOW_MASK) | HIGH_CONST;
    lung->a[0] ^= r->a[0];
    lung->a[1] ^= r->a[1];
}

INLINE static void convert(w128_t *x) {
    x->d[0] += 2.0L;
    x->d[1] += 2.0L;
}

INLINE static void gen_rand_all(void) {
    int i;
    w128_t lung;

    lung = sfmt[N];
    do_recursion(&sfmt[0], &sfmt[0], &sfmt[POS1], &sfmt[N -1], &lung);
    for (i = 1; i < N - POS1; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + POS1], &sfmt[i - 1], &lung);
    }
    for (; i < N; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + POS1 - N], &sfmt[i - 1],
		     &lung);
    }
    sfmt[N] = lung;
}

INLINE static void gen_rand_array(w128_t array[], int size) {
    int i, j;
    w128_t lung;

    lung = sfmt[N];
    do_recursion(&array[0], &sfmt[0], &sfmt[POS1], &sfmt[N - 1], &lung);
    for (i = 1; i < N - POS1; i++) {
	do_recursion(&array[i], &sfmt[i], &sfmt[i + POS1], &array[i - 1],
		     &lung);
    }
    for (; i < N; i++) {
	do_recursion(&array[i], &sfmt[i], &array[i + POS1 - N],
		     &array[i - 1], &lung);
    }
    for (; i < size - N; i++) {
	do_recursion(&array[i], &array[i - N], &array[i + POS1 - N],
		     &array[i - 1], &lung);
	convert(&array[i - N]);
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j] = array[j + size - N];
    }
    for (; i < size; i++, j++) {
	do_recursion(&array[i], &array[i - N], &array[i + POS1 - N],
		     &array[i - 1], &lung);
	sfmt[j] = array[i];
	convert(&array[i - N]);
    }
    for (j = size - N; j < size; j++) {
	convert(&array[j]);
    }
    sfmt[N] = lung;
}

INLINE
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    double gen_rand(void)
{
    double r;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r = psfmt[idx++];
    r += 2.0L;
    return r;
}

INLINE
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    void fill_array(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array((w128_t *)array, size / 2);
}

void init_gen_rand(uint64_t seed)
{
    int i;
    uint64_t *psfmt;

    psfmt = (uint64_t *)&sfmt[0];
    psfmt[0] = (seed & LOW_MASK) | HIGH_CONST;
    for (i = 1; i < (N + 1) * 2; i++) {
        psfmt[i] = 1812433253UL * (psfmt[i - 1] ^ (psfmt[i - 1] >> 30)) + i;
        psfmt[i] = (psfmt[i] & LOW_MASK) | HIGH_CONST;
    }
    idx = N * 2;
}

#include "test_time.c"
