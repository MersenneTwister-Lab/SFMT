/* SFMT double float version M.Saito 2006.10.16 */

#include <string.h>
#include <stdio.h>
#include <ppc_intrinsics.h>
#include <assert.h>
#include "random.h"
#include "paramsD01.h"

static vector unsigned int sfmt[N + 1];
static double *dsfmt = (double *)&(sfmt[0]);
static int idx;

INLINE static void gen_rand_array(vector unsigned int array[], int size);
INLINE static void gen_rand_all(void);

INLINE static
#if defined(__GNUC__) && (!defined(DEBUG))
__attribute__((always_inline)) 
#endif
    void convert(vector unsigned int *x) {
    double *d = (double *)x;
    
    d[0] += 2.0L;
    d[1] += 2.0L;
}

/**
 * @param dist posion of array. may equal to a. chaneged.
 * @param a position of array.
 * @param b position of array.
 * @param reg may keep in register. changed.
 * @param lung may keep in register. changed.
 */
#if 0
#define vec_recursion(dist, a, b, reg, lung)	\
    do {					\
	vector unsigned int v, w, x, y, z;	\
	x = vec_perm(a, perm_sl, perm_sl);	\
	v = a;					\
	y = vec_srl(b, sr1);			\
	z = vec_and(vec_sll(reg, sl1), sl1_msk);		\
	w = vec_and(vec_srl(reg, sr2), sr2_msk);		\
	w = vec_xor(w, z);					\
	z = vec_perm(lung, perm, perm);				\
	z = vec_xor(z, w);					\
	y = vec_and(vec_and(y, sr1_msk), mask);			\
	v = vec_xor(v, x);					\
	z = vec_xor(z, y);					\
	z = vec_xor(z, v);					\
	reg = vec_or(vec_and(z, low_mask), high_const);		\
	dist = reg;						\
	lung = vec_xor(lung, reg);				\
    } while(0)
#else
INLINE static
#if defined(__GNUC__) && (!defined(DEBUG))
__attribute__((always_inline)) 
#endif
    vector unsigned int vec_recursion(vector unsigned int a,
				      vector unsigned int b,
				      vector unsigned int reg,
				      vector unsigned int lung) {
    vector unsigned int v, w, x, y, z;
    const vector unsigned char sl1 = (vector unsigned char)(ALTI_SL1);
    const vector unsigned char sl1_perm = SL1_PERM;
    const vector unsigned int sl1_msk = SL1_MSK;
    const vector unsigned char sr1 = (vector unsigned char)(ALTI_SR1);
    const vector unsigned char sr1_perm = SR1_PERM;
    const vector unsigned int sr1_msk = SR1_MSK;
    const vector unsigned char sr2 = (vector unsigned char)(ALTI_SR2);
    const vector unsigned char sr2_perm = SR2_PERM;
    const vector unsigned int sr2_msk = SR2_MSK;
    const vector unsigned int mask = ALTI_MSK;
    const vector unsigned char perm_sl = SL2_PERM;
    const vector unsigned char perm = ALTI_PERM;
    const vector unsigned int low_mask = ALTI_LOW_MSK;
    const vector unsigned int high_const = ALTI_HIGH_CONST;

    x = vec_perm(a, perm_sl, perm_sl);
    v = a;
    y = vec_srl(vec_perm(b, sr1_perm, sr1_perm), sr1);
    z = vec_and(vec_sll(vec_perm(reg, sl1_perm, sl1_perm), sl1), sl1_msk);
    w = vec_and(vec_srl(vec_perm(reg, sr2_perm, sr2_perm), sr2), sr2_msk);
    w = vec_xor(w, z);
    z = vec_perm(lung, perm, perm);
    z = vec_xor(z, w);
    y = vec_and(vec_and(y, sr1_msk), mask);// ikkai de dekiru
    v = vec_xor(v, x);
    z = vec_xor(z, y);
    z = vec_xor(z, v);
    return vec_or(vec_and(z, low_mask), high_const);
}
#endif

INLINE static
#if defined(__GNUC__) && (!defined(DEBUG))
__attribute__((always_inline)) 
#endif
    void gen_rand_all(void) {
    int i;
    vector unsigned int r, lung;

    lung = sfmt[N];
    r = sfmt[N - 1];
    for (i = 0; i < N - POS1; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1], r, lung);
	sfmt[i] = r;
	lung = vec_xor(lung, r);
    }
    for (; i < N; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1 - N], r, lung);
	sfmt[i] = r;
	lung = vec_xor(lung, r);
    }
    sfmt[N] = lung;
}

INLINE static void gen_rand_array(vector unsigned int array[], int size)
{
    int i, j;
    vector unsigned int r, lung;

    /* read from sfmt */
    lung = sfmt[N];
    r = sfmt[N - 1];
    for (i = 0; i < N - POS1; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1], r, lung);
	array[i] = r;
	lung = vec_xor(lung, r);
    }
    for (; i < N; i++) {
	r = vec_recursion(sfmt[i], array[i + POS1 - N], r, lung);
	array[i] = r;
	lung = vec_xor(lung, r);
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = vec_recursion(array[i - N], array[i + POS1 - N], r, lung);
	array[i] = r;
	lung = vec_xor(lung, r);
	convert(&array[i - N]);
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j] = array[j + size - N];
    }
    for (; i < size; i++) {
	r = vec_recursion(array[i - N], array[i + POS1 - N], r, lung);
	array[i] = r;
	lung = vec_xor(lung, r);
	sfmt[j++] = r;
	convert(&array[i - N]);
    }
    for (j = size - N; j < size; j++) {
	convert(&array[j]);
    }
    sfmt[N] = lung;
}

INLINE static
#if defined(__GNUC__) && (!defined(DEBUG))
__attribute__((always_inline)) 
#endif
    double gen_rand(void)
{
    double r;
    
    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r = dsfmt[idx++];
    r += 2.0L;
    return r;
}

INLINE void fill_array(double array[], int size)
{
    assert(size >= 2 * N * 2);
    assert(size % 2 == 0);

    gen_rand_array((vector unsigned int *)array, size / 2);
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

