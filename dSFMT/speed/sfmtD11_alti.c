/* SFMT double float version M.Saito 2006.10.16 */

#include <string.h>
#include <stdio.h>
#include <ppc_intrinsics.h>
#include <assert.h>
#include "random.h"
#include "paramsD11.h"
#include "paramsD11-alti.h"

static vector unsigned int sfmt[N + 1];
static double *dsfmt = (double *)&(sfmt[0]);
static int idx;

static void gen_rand_array(vector unsigned int array[], int size);
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

static void gen_rand_array(vector unsigned int array[], int size)
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

