/* SFMT double float version M.Saito 2006.10.16 */

#include <string.h>
#include <stdio.h>
#include <ppc_intrinsics.h>
#include <assert.h>
#include "random.h"
#include "paramsD11.h"
#include "paramsD11-alti.h"

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

static w128_t sfmt[N + 1];
static double *psfmt = &sfmt[0].d[0];
static int idx;

static void gen_rand_array(w128_t array[], int size);
INLINE static void gen_rand_all(void);

INLINE
#if defined(__GNUC__) && (!defined(DEBUG))
__attribute__((always_inline)) 
#endif
    static void convert_co(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].d[0] -= 1.0L;
	array[i].d[1] -= 1.0L;
    }
}

INLINE
#if defined(__GNUC__) && (!defined(DEBUG))
__attribute__((always_inline)) 
#endif
    static void convert_oc(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].d[0] = 2.0L - array[i].d[0];
	array[i].d[1] = 2.0L - array[i].d[1];
    }
}

INLINE
#if defined(__GNUC__) && (!defined(DEBUG))
__attribute__((always_inline)) 
#endif
    static void convert_oo(w128_t array[], int size) {
    int i;
    const vector unsigned int alti_int_one = (vector unsigned int)(0,1,0,1);

    for (i = 0; i < size; i++) {
	array[i].s = vec_or(array[i].s, alti_int_one);
	array[i].d[0] -= 1.0L;
	array[i].d[1] -= 1.0L;
    }
}

/**
 * @param dist posion of array. may equal to a. chaneged.
 * @param a position of array.
 * @param b position of array.
 * @param reg may keep in register. changed.
 * @param lung may keep in register. changed.
 */
INLINE static
#if defined(__GNUC__) && (!defined(DEBUG))
__attribute__((always_inline)) 
#endif
    vector unsigned int vec_recursion(vector unsigned int a,
				      vector unsigned int b,
				      vector unsigned int reg,
				      vector unsigned int lung) {
    vector unsigned int r, s, t, u, v, w, x, y, z;
    const vector unsigned char sl1 = (vector unsigned char)(ALTI_SL1);
    const vector unsigned char sl1_perm = SL1_PERM;
    const vector unsigned int sl1_msk = SL1_MSK;
    const vector unsigned char sl2_perm = SL2_PERM;
    const vector unsigned char sr1 = (vector unsigned char)(ALTI_SR1);
    const vector unsigned int sr1_msk = SR1_MSK;
    const vector unsigned char sr2_perm = SR2_PERM;
    const vector unsigned char perm = ALTI_PERM;
    const vector unsigned int low_mask = ALTI_LOW_MSK;
    const vector unsigned int high_const = ALTI_HIGH_CONST;

    x = vec_perm(a, (vector unsigned int)sl2_perm, sl2_perm);
    y = vec_srl(b, sr1);
    y = vec_and(y, sr1_msk);
    z = vec_perm(reg, (vector unsigned int)sl1_perm, sl1_perm);
    z = vec_sll(z, sl1);
    z = vec_and(z, sl1_msk);
    w = vec_perm(reg, (vector unsigned int)sr2_perm, sr2_perm);
    v = vec_perm(lung, (vector unsigned int)perm, perm);
    s = vec_xor(a, x);
    t = vec_xor(y, z);
    u = vec_xor(w, v);
    r = vec_xor(s, t);
    r = vec_xor(r, u);
    r = vec_and(r, low_mask);
    r = vec_or(r, high_const);
    return r;
}

INLINE static
#if defined(__GNUC__) && (!defined(DEBUG))
__attribute__((always_inline)) 
#endif
    void gen_rand_all(void) {
    int i;
    vector unsigned int r, lung;

    lung = sfmt[N].s;
    r = sfmt[N - 1].s;
    for (i = 0; i < N - POS1; i++) {
	r = vec_recursion(sfmt[i].s, sfmt[i + POS1].s, r, lung);
	sfmt[i].s = r;
	lung = vec_xor(lung, r);
    }
    for (; i < N; i++) {
	r = vec_recursion(sfmt[i].s, sfmt[i + POS1 - N].s, r, lung);
	sfmt[i].s = r;
	lung = vec_xor(lung, r);
    }
    sfmt[N].s = lung;
}

static void gen_rand_array(w128_t array[], int size)
{
    int i, j;
    vector unsigned int r, lung;

    /* read from sfmt */
    lung = sfmt[N].s;
    r = sfmt[N - 1].s;
    for (i = 0; i < N - POS1; i++) {
	r = vec_recursion(sfmt[i].s, sfmt[i + POS1].s, r, lung);
	array[i].s = r;
	lung = vec_xor(lung, r);
    }
    for (; i < N; i++) {
	r = vec_recursion(sfmt[i].s, array[i + POS1 - N].s, r, lung);
	array[i].s = r;
	lung = vec_xor(lung, r);
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = vec_recursion(array[i - N].s, array[i + POS1 - N].s, r, lung);
	array[i].s = r;
	lung = vec_xor(lung, r);
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j].s = array[j + size - N].s;
    }
    for (; i < size; i++) {
	r = vec_recursion(array[i - N].s, array[i + POS1 - N].s, r, lung);
	array[i].s = r;
	lung = vec_xor(lung, r);
	sfmt[j++].s = r;
    }
    sfmt[N].s = lung;
}

INLINE
#if defined(__GNUC__) && (!defined(DEBUG))
__attribute__((always_inline)) 
#endif
    double genrand_close_open(void)
{
    double r;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r = psfmt[idx++];
    return r - 1.0L;
}

INLINE
#if defined(__GNUC__) && (!defined(DEBUG))
__attribute__((always_inline)) 
#endif
    double genrand_open_close(void) {
    double r;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r = psfmt[idx++];
    return 2.0L - r;
}

INLINE
#if defined(__GNUC__) && (!defined(DEBUG))
__attribute__((always_inline)) 
#endif
    double genrand_open_open(void) {
    union {
	uint64_t u;
	double d;
    } conv;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    conv.d = psfmt[idx++];
    conv.u |= 1;
    return conv.d - 1.0L;
}

INLINE
#if defined(__GNUC__) && (!defined(DEBUG))
__attribute__((always_inline)) 
#endif
    double genrand_close1_open2(void) {
    double r;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r = psfmt[idx++];
    return r;
}

void fill_array_open_close(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array((w128_t *)array, size / 2);
    convert_oc((w128_t *)array, size / 2);
}

void fill_array_close_open(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array((w128_t *)array, size / 2);
    convert_co((w128_t *)array, size / 2);
}

void fill_array_open_open(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array((w128_t *)array, size / 2);
    convert_oo((w128_t *)array, size / 2);
}

void fill_array_close1_open2(double array[], int size)
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

#include "test_time3.c"

