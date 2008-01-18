/* SFMT double float version M.Saito 2006.10.16 */

#include <string.h>
#include <stdio.h>
#include <ppc_intrinsics.h>
#include <assert.h>
#include "random.h"
#include "paramsDPf.h"

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 104
#define N ((MEXP - 128) / WORDSIZE)
#define MAXDEGREE (WORDSIZE * N + 128)

void fill_array_open_close(double array[], int size);
void fill_array_close_open(double array[], int size);
void fill_array_open_open(double array[], int size);
void fill_array_close1_open2(double array[], int size);
INLINE double genrand_open_close(void);
INLINE double genrand_close_open(void);
INLINE double genrand_open_open(void);
INLINE double genrand_close1_open2(void);

union W128_T {
    vector unsigned int s;
    uint64_t a[2];
    double d[2];
};
typedef union W128_T w128_t;

union W64_T {
    double d;
    uint64_t u;
};
typedef union W64_T w64_t;

static w128_t sfmt[N + 1];
static double *psfmt = &sfmt[0].d[0];
static int idx;

static void gen_rand_array(w128_t array[], int size);
INLINE static void gen_rand_all(void);

#if defined(__GNUC__)
#define ALWAYSINLINE __attribute__((always_inline)) 
#else
#define ALWAYSINLINE
#endif
INLINE static void convert_oc(w128_t array[], int size) ALWAYSINLINE;
INLINE static void convert_oo(w128_t array[], int size) ALWAYSINLINE;

#if (defined(__BIG_ENDIAN__) || defined(BIG_ENDIAN)) && !defined(__amd64)
inline static int idxof(int i) {
    return i ^ 1;
}
#else
inline static int idxof(int i) {
    return i;
}
#endif

INLINE static void convert_co(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].d[0] -= 1.0;
	array[i].d[1] -= 1.0;
    }
}

INLINE static void convert_oc(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].d[0] = 2.0 - array[i].d[0];
	array[i].d[1] = 2.0 - array[i].d[1];
    }
}

INLINE static void convert_oo(w128_t array[], int size) {
    int i;
    const vector unsigned int alti_int_one = (vector unsigned int)(0,1,0,1);

    for (i = 0; i < size; i++) {
	array[i].s = vec_or(array[i].s, alti_int_one);
	array[i].d[0] -= 1.0;
	array[i].d[1] -= 1.0;
    }
}

/**
 * @param dist posion of array. may equal to a. chaneged.
 * @param r position of array.
 * @param a position of array.
 * @param b may keep in register. changed.
 * @param lung may keep in register. changed.
 */
#if 0
#define  vec_recursion(m, a, r, lung) \
do { \
    vector unsigned int s, t, u, v, w, x; \
    const vector unsigned char sl1 = (vector unsigned char)(ALTI_SL1); \
    const vector unsigned char sl1_perm = ALTI_SL1_PERM; \
    const vector unsigned int sl1_msk = ALTI_SL1_MSK; \
    const vector unsigned char sl2 = (vector unsigned char)(ALTI_SL2); \
    const vector unsigned char sl2_perm = ALTI_SL2_PERM; \
    const vector unsigned int sl2_msk = ALTI_SL2_MSK; \
    const vector unsigned char sr = (vector unsigned char)(ALTI_SR); \
    const vector unsigned char sr_perm = ALTI_SR_PERM; \
    const vector unsigned int sr_msk = ALTI_SR_MSK; \
    const vector unsigned char perm = ALTI_PERM; \
 \
s = vec_perm(lung, (vector unsigned int)perm, perm); \
t = vec_add(s, a); \
u = vec_perm(t, (vector unsigned int)sl1_perm, sl1_perm); \
u = vec_sll(u, sl1); \
u = vec_and(u, sl1_msk); \
v = vec_perm(r, (vector unsigned int)sl2_perm, sl2_perm); \
v = vec_sll(v, sl2); \
v = vec_and(v, sl2_msk); \
w = vec_xor(v, u); \
lung = vec_xor(lung, w); \
x = vec_perm(lung, (vector unsigned int)sr_perm, sr_perm); \
x = vec_sll(x, sr); \
x = vec_and(x, sr_msk); \
r = vec_and(a, x); \
m = r; \
} while (0)
#else
void vec_recursion(vector unsigned int *m,
		   vector unsigned int a,
		   vector unsigned int *r,
		   vector unsigned int *lung) { 
    vector unsigned int s, t, u, v, w, x; 
    const vector unsigned char sl1 = (vector unsigned char)(ALTI_SL1); 
    const vector unsigned char sl1_perm = ALTI_SL1_PERM; 
    const vector unsigned int sl1_msk = ALTI_SL1_MSK; 
    const vector unsigned char sl2 = (vector unsigned char)(ALTI_SL2);
    const vector unsigned char sl2_perm = ALTI_SL2_PERM;
    const vector unsigned int sl2_msk = ALTI_SL2_MSK;
    const vector unsigned char sr = (vector unsigned char)(ALTI_SR);
    const vector unsigned char sr_perm = ALTI_SR_PERM;
    const vector unsigned int sr_msk = ALTI_SR_MSK;
    const vector unsigned char perm = ALTI_PERM;

    s = vec_perm(*lung, (vector unsigned int)perm, perm);
    t = vec_add(s, a);
    u = vec_perm(t, (vector unsigned int)sl1_perm, sl1_perm);
    u = vec_sll(u, sl1);
    u = vec_and(u, sl1_msk);
    v = vec_perm(*r, (vector unsigned int)sl2_perm, sl2_perm);
    v = vec_sll(v, sl2);
    v = vec_and(v, sl2_msk);
    w = vec_xor(v, u);
    *lung = vec_xor(*lung, w);
    x = vec_perm(*lung, (vector unsigned int)sr_perm, sr_perm);
    x = vec_sll(x, sr);
    x = vec_and(x, sr_msk);
    *r = vec_xor(a, x);
    *m = *r;
}
#endif

INLINE static void gen_rand_all(void) {
    int i;
    vector unsigned int r, lung;

    lung = sfmt[N].s;
    r = sfmt[N - 1].s;
    for (i = 0; i < N; i++) {
	vec_recursion(&sfmt[i].s, sfmt[i].s, &r, &lung);
    }
    sfmt[N].s = lung;
}

static void gen_rand_array12(w128_t array[], int size)
{
    int i, j;
    vector unsigned int r, lung;

    /* read from sfmt */
    lung = sfmt[N].s;
    r = sfmt[N - 1].s;
    for (i = 0; i < N; i++) {
	vec_recursion(&array[i].s, sfmt[i].s, &r, &lung);
    }
    /* main loop */
    for (; i < size - N; i++) {
	vec_recursion(&array[i].s, array[i - N].s, &r, &lung);
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j].s = array[j + size - N].s;
    }
    for (; i < size; i++) {
	vec_recursion(&array[i].s, array[i - N].s, &r, &lung);
	sfmt[j++].s = r;
    }
    sfmt[N].s = lung;
}

static void gen_rand_arrayco(w128_t array[], int size)
{
    int i, j;
    vector unsigned int r, lung;

    /* read from sfmt */
    lung = sfmt[N].s;
    r = sfmt[N - 1].s;
    for (i = 0; i < N; i++) {
	vec_recursion(&array[i].s, sfmt[i].s, &r, &lung);
    }
    /* main loop */
    for (; i < size - N; i++) {
	vec_recursion(&array[i].s, array[i - N].s, &r, &lung);
	array[i - N].d[0] -= 1.0;
	array[i - N].d[1] -= 1.0;
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j].s = array[j + size - N].s;
    }
    for (; i < size; i++) {
	vec_recursion(&array[i].s, array[i - N].s, &r, &lung);
	sfmt[j++].s = r;
	array[i - N].d[0] -= 1.0;
	array[i - N].d[1] -= 1.0;
    }
    for (i = size - N; i < size; i++) {
	array[i].d[0] -= 1.0;
	array[i].d[1] -= 1.0;
    }
    sfmt[N].s = lung;
}

INLINE double genrand_close1_open2(void) {
    double r;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r = psfmt[idx++];
    return r;
}

INLINE double genrand_open_open(void) {
    w64_t r;

    r.d = genrand_close1_open2();
    r.u |= 1;
    return r.d - 1.0;
}

void fill_array_open_close(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array12((w128_t *)array, size / 2);
    convert_oc((w128_t *)array, size / 2);
}

void fill_array_close_open(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_arrayco((w128_t *)array, size / 2);
}

void fill_array_open_open(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array12((w128_t *)array, size / 2);
    convert_oo((w128_t *)array, size / 2);
}

void fill_array_close1_open2(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array12((w128_t *)array, size / 2);
}

void init_gen_rand(uint64_t seed)
{
    int i;
    uint64_t *psfmt;

    psfmt = (uint64_t *)&sfmt[0];
    psfmt[0] = (seed & SFMT_LOW_MASK) | SFMT_HIGH_CONST;
    for (i = 1; i < N * 2; i++) {
	psfmt[i] = 6364136223846793005ULL 
	    * (psfmt[i - 1] ^ (psfmt[i - 1] >> 62)) + i;
	psfmt[i] = (psfmt[i] & SFMT_LOW_MASK) | SFMT_HIGH_CONST;
    }
    for (;i < (N + 1) * 2; i++) {
	psfmt[i] = 6364136223846793005ULL 
	    * (psfmt[i - 1] ^ (psfmt[i - 1] >> 62)) + i;
    }
    idx = N * 2;
}

#include "test_time3.c"

