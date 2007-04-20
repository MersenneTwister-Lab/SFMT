/* Simple and Fast Twister */

#include <string.h>
#if defined(__GNUC__)
  #include <stdint.h>
#endif
#include <stdio.h>
#include <assert.h>
#include "random.h"
#include "speed.h"

#if defined(__BIG_ENDIAN__) && defined(__amd64)
#undef __BIG_ENDIAN__
#endif

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

#define M_RAN_INVM32 2.32830643653869628906e-010

#define LOW_MASK  0x000FFFFFFFFFFFFFULL
#define HIGH_CONST 0x3FF0000000000000ULL
#define LOW_MASK32_1 0x000fffffU
#define LOW_MASK32_2 0xffffffffU
#define HIGH_CONST32 0x3ff00000U

union W128_T {
    double d[2];
    uint64_t u[2];
    uint32_t a[4];
};
typedef union W128_T w128_t;

union W64_T {
    double d;
    uint64_t u;
    uint32_t a[2];
};
typedef union W64_T w64_t;

static w128_t sfmt[N];
static uint32_t *psfmt = (uint32_t *)sfmt;
static w64_t *dsfmt = (w64_t *)sfmt;
static int idx;

#if defined(__GNUC__) && (!defined(DEBUG))
#define ALWAYSINLINE __attribute__((always_inline))
#else
#define ALWAYSINLINE
#endif
INLINE static void convert_12(w128_t array[], int size) ALWAYSINLINE;
INLINE static void convert_co(w128_t array[], int size) ALWAYSINLINE;
INLINE static void convert_oc(w128_t array[], int size) ALWAYSINLINE;
INLINE static void convert_oo(w128_t array[], int size) ALWAYSINLINE;

#if defined(__BIG_ENDIAN__)
INLINE static void convert_12(w128_t array[], int size) {
    uint32_t r;
    int i;
    for (i = 0; i < size; i++) {
	r = (array[i].a[1] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[1] = array[i].a[0];
	array[i].a[0] = r;
	r = (array[i].a[3] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[3] = array[i].a[2];
	array[i].a[2] = r;
    }
}
#else
INLINE static void convert_12(w128_t array[], int size) {
    int i;
    for (i = 0; i < size; i++) {
	array[i].u[0] = (array[i].u[0] & LOW_MASK) | HIGH_CONST;
	array[i].u[1] = (array[i].u[1] & LOW_MASK) | HIGH_CONST;
    }
}
#endif

#if defined(__BIG_ENDIAN__)
INLINE static void convert_co(w128_t array[], int size) {
    uint32_t r;
    int i;

    for (i = 0; i < size; i++) {
	r = (array[i].a[1] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[1] = array[i].a[0];
	array[i].a[0] = r;
	r = (array[i].a[3] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[3] = array[i].a[2];
	array[i].a[2] = r;
	array[i].d[0] -= 1.0;
	array[i].d[1] -= 1.0;
    }
}
#else
INLINE static void convert_co(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].u[0] = (array[i].u[0] & LOW_MASK) | HIGH_CONST;
	array[i].u[1] = (array[i].u[1] & LOW_MASK) | HIGH_CONST;
	array[i].d[0] -= 1.0;
	array[i].d[1] -= 1.0;
    }
}
#endif

#if defined(__BIG_ENDIAN__)
INLINE static void convert_oc(w128_t array[], int size) {
    uint32_t r;
    int i;

    for (i = 0; i < size; i++) {
	r = (array[i].a[1] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[1] = array[i].a[0];
	array[i].a[0] = r;
	r = (array[i].a[3] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[3] = array[i].a[2];
	array[i].a[2] = r;
	array[i].d[0] = 2.0 - array[i].d[0];
	array[i].d[1] = 2.0 - array[i].d[1];
    }
}
#else
INLINE static void convert_oc(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].u[0] = (array[i].u[0] & LOW_MASK) | HIGH_CONST;
	array[i].u[1] = (array[i].u[1] & LOW_MASK) | HIGH_CONST;
	array[i].d[0] = 2.0 - array[i].d[0];
	array[i].d[1] = 2.0 - array[i].d[1];
    }
}
#endif

#if defined(__BIG_ENDIAN__)
INLINE static void convert_oo(w128_t array[], int size) {
    int i;
    uint32_t r;
    for (i = 0; i < size; i++) {
	r = (array[i].a[1] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[1] = array[i].a[0] | 1;
	array[i].a[0] = r;
	r = (array[i].a[3] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[3] = array[i].a[2] | 1;
	array[i].a[2] = r;
	array[i].d[0] -= 1.0;
	array[i].d[1] -= 1.0;
    }
}
#else
INLINE static void convert_oo(w128_t array[], int size) {
    int i;
    for (i = 0; i < size; i++) {
	array[i].u[0] = (array[i].u[0] & LOW_MASK) | HIGH_CONST | 1;
	array[i].u[1] = (array[i].u[1] & LOW_MASK) | HIGH_CONST | 1;
	array[i].d[0] -= 1.0;
	array[i].d[1] -= 1.0;
    }
}
#endif

INLINE static
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
#if !defined(__BIG_ENDIAN__)
void lshift128(w128_t *out, const w128_t *in, int shift) {
    out->u[0] = in->u[0];
    out->u[1] = in->u[1];
    out->u[0] = out->u[0] << (shift * 8);
    out->u[1] = out->u[1] << (shift * 8);
    out->u[1] |= in->u[0] >> (64 - shift * 8);
}
#else
void lshift128(w128_t *out, const w128_t *in, int shift) {
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in->a[3] << 32) | ((uint64_t)in->a[2]);
    tl = ((uint64_t)in->a[1] << 32) | ((uint64_t)in->a[0]);

    oh = th << (shift * 8);
    ol = tl << (shift * 8);
    oh |= tl >> (64 - shift * 8);
    out->a[0] = (uint32_t)ol;
    out->a[1] = (uint32_t)(ol >> 32);
    out->a[2] = (uint32_t)oh;
    out->a[3] = (uint32_t)(oh >> 32);
}
#endif

INLINE static
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
 void rshift128(w128_t *out, const w128_t *in,
			     int shift) {
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

INLINE static
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    void gen_rand_all(void) {
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
	sfmt[j].a[0] = array[j + size - N].a[0];
	sfmt[j].a[1] = array[j + size - N].a[1];
	sfmt[j].a[2] = array[j + size - N].a[2];
	sfmt[j].a[3] = array[j + size - N].a[3];
    }
    for (; i < size; i++, j++) {
	do_recursion(&array[i], &array[i - N], &array[i + POS1 - N],
		     &array[i - 2], &array[i - 1]);
	sfmt[j].a[0] = array[i].a[0];
	sfmt[j].a[1] = array[i].a[1];
	sfmt[j].a[2] = array[i].a[2];
	sfmt[j].a[3] = array[i].a[3];
    }
}

INLINE
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    uint32_t genrand_int32(void)
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
    uint64_t genrand_int64(void)
{
    uint64_t r;

    if (idx >= N * 4) {
	gen_rand_all();
	idx = 0;
    }
    r = psfmt[idx] | ((uint64_t)psfmt[idx+1] << 32);
    idx += 2;
    return r;
}

INLINE
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    void fill_array_int32(uint32_t array[], int size)
{
    assert(size % 4 == 0);
    assert(size >= 2 * N * 4);
    gen_rand_array((w128_t *)array, size / 4);
}

INLINE double gen_rand(void)
{
    w64_t r;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r.u = dsfmt[idx].u;
    idx++;
    r.u &= LOW_MASK;
    r.u |= HIGH_CONST;
    return r.d;
}

#if defined(__BIG_ENDIAN__)
INLINE double genrand_close1_open2(void)
{
    w64_t r;
    uint32_t t;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r = dsfmt[idx++];
    t = (r.a[1] & LOW_MASK32_1) | HIGH_CONST32;
    r.a[1] = r.a[0];
    r.a[0] = t;
    t = (r.a[3] & LOW_MASK32_1) | HIGH_CONST32;
    r.a[3] = r.a[2];
    r.a[2] = t;
    return r.d;
}
#else
INLINE double genrand_close1_open2(void)
{
    w64_t r;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r = dsfmt[idx++];
    r.u &= LOW_MASK;
    r.u |= HIGH_CONST;
    return r.d;
}
#endif

#if defined(__BIG_ENDIAN__)
INLINE double genrand_open_open(void)
{
    w64_t r;
    uint32_t t;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r = dsfmt[idx++];
    t = (r.a[1] & LOW_MASK32_1) | HIGH_CONST32;
    r.a[1] = r.a[0] | 1;
    r.a[0] = t;
    t = (r.a[3] & LOW_MASK32_1) | HIGH_CONST32;
    r.a[3] = r.a[2] | 1;
    r.a[2] = t;
    return r.d - 1.0;
}
#else
INLINE double genrand_open_open(void)
{
    w64_t r;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r = dsfmt[idx++];
    r.u &= LOW_MASK;
    r.u |= (HIGH_CONST | 1);
    return r.d - 1.0;
}
#endif

INLINE void fill_array_close1_open2(double array[], int size)
{
    assert(size >= N * 2);
    assert(size % 2 == 0);

    gen_rand_array((w128_t *)array, size / 2);
    convert_12((w128_t *)array, size / 2);
}

INLINE void fill_array_open_close(double array[], int size)
{
    assert(size >= N * 2);
    assert(size % 2 == 0);

    gen_rand_array((w128_t *)array, size / 2);
    convert_oc((w128_t *)array, size / 2);
}

INLINE void fill_array_close_open(double array[], int size)
{
    assert(size >= N * 2);
    assert(size % 2 == 0);

    gen_rand_array((w128_t *)array, size / 2);
    convert_co((w128_t *)array, size / 2);
}

INLINE void fill_array_open_open(double array[], int size)
{
    assert(size >= N * 2);
    assert(size % 2 == 0);

    gen_rand_array((w128_t *)array, size / 2);
    convert_oo((w128_t *)array, size / 2);
}

INLINE void init_gen_rand(uint64_t seed)
{
    int i;

    psfmt[0] = (uint32_t)seed;
    for (i = 1; i < N * 4; i++) {
	psfmt[i] = 1812433253UL * (psfmt[i - 1] ^ (psfmt[i - 1] >> 30)) + i;
    }
    idx = N * 4;
}

INLINE void init_genrand(uint32_t seed) {
    init_gen_rand(seed);
}

/* generates a random number on [0,1]-real-interval */
INLINE double genrand_real1(void)
{
    return genrand_int32()*M_RAN_INVM32;
    /* divided by 2^32-1 */ 
}

/* JURGEN A DOORNIK */
INLINE double genrand_real2(void)
{
    return ((int)genrand_int32()) * M_RAN_INVM32 + 0.5;
}

/* generates a random number on [0,1) with 53-bit resolution*/
INLINE double genrand_res53() 
{ 
    //return genrand_int64() * (1.0/18446744073709551616.0L);
    return genrand_int64() * (1.0/18446744073709551616.0);
}
#ifdef BLOCK
#include "test_time3.c"
#else
#include "test_time2.c"
#endif
