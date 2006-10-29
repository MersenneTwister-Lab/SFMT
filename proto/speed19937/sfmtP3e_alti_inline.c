/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdio.h>
#include <ppc_intrinsics.h>
#include <assert.h>
#include "random-inline.h"

#define MEXP 19937

#define WORDSIZE 128
#define N (MEXP / WORDSIZE)
#define MAXDEGREE (WORDSIZE * (N + 1))

static vector unsigned int sfmt[N + 1];
static unsigned int idx;
static uint32_t *sfmt32 = (uint32_t *)sfmt;

#define POS1 89
#define SL1 18
#define SL2 1
#define SR1 3
#define SR2 2
#define PM1 1
#define PM2 0
#define PM3 2
#define PM4 3
#define MSK1 0x7f7fffffU
#define MSK2 0xfffbffffU
#define MSK3 0xeffffbffU
#define MSK4 0xfefe7befU

#define PERM (PM1 * 4, PM1 * 4 + 1, PM1 * 4 + 2, PM1 * 4 + 3, \
	      PM2 * 4, PM2 * 4 + 1, PM2 * 4 + 2, PM2 * 4 + 3, \
	      PM3 * 4, PM3 * 4 + 1, PM3 * 4 + 2, PM3 * 4 + 3, \
	      PM4 * 4, PM4 * 4 + 1, PM4 * 4 + 2, PM4 * 4 + 3)

INLINE static void gen_rand_array(vector unsigned int array[], int size);
INLINE static void gen_rand_all(void);
INLINE static vector unsigned int vec_recursion(vector unsigned int a,
						vector unsigned int b,
						vector unsigned int c,
						vector unsigned int d,
						vector unsigned int sl1,
						vector unsigned int sr1,
						vector unsigned int mask,
						vector unsigned char perm_sl,
						vector unsigned char perm);

INLINE static __attribute__((always_inline))
    vector unsigned int vec_recursion(vector unsigned int a,
				      vector unsigned int b,
				      vector unsigned int c,
				      vector unsigned int d,
				      vector unsigned int sl1,
				      vector unsigned int sr1,
				      vector unsigned int mask,
				      vector unsigned char perm_sl,
				      vector unsigned char perm) {

    vector unsigned int v, w, x, y, z;
    x = vec_perm(a, perm_sl, perm_sl);
    v = a;
    y = vec_sr(b, sr1);
    z = vec_perm(d, perm, perm);
    w = vec_sl(c, sl1);
    z = vec_xor(z, w);
    y = vec_and(y, mask);
    v = vec_xor(v, x);
    z = vec_xor(z, y);
    z = vec_xor(z, v);
    return z;
}

INLINE void gen_rand_all(void) {
    int i;
    vector unsigned int r, lung;

    const vector unsigned int sl1 = (vector unsigned int)(SL1, SL1, SL1, SL1);
    const vector unsigned int sr1 = (vector unsigned int)(SR1, SR1, SR1, SR1);
    const vector unsigned int mask = (vector unsigned int)
	(MSK1, MSK2, MSK3, MSK4);
    const vector unsigned char perm_sl = (vector unsigned char)
	(1, 2, 3, 23, 5, 6, 7, 0, 9, 10, 11, 4, 13, 14, 15, 8);
    const vector unsigned char perm = (vector unsigned char)PERM;

    lung = sfmt[N];
    r = sfmt[N - 1];
    for (i = 0; i < N - POS1; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1], r, lung, sl1, sr1, 
			  mask, perm_sl, perm);
	sfmt[i] = r;
	lung = vec_xor(lung, r);
    }
    for (; i < N; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1 - N], r, lung, sl1, sr1,
			  mask, perm_sl, perm);
	sfmt[i] = r;
	lung = vec_xor(lung, r);
    }
    sfmt[N] = lung;
}

INLINE static void gen_rand_array(vector unsigned int array[], int size)
{
    int i, j;
    vector unsigned int r, lung;
 
    const vector unsigned int sl1 = (vector unsigned int)(SL1, SL1, SL1, SL1);
    const vector unsigned int sr1 = (vector unsigned int)(SR1, SR1, SR1, SR1);
    const vector unsigned int mask = (vector unsigned int)
	(MSK1, MSK2, MSK3, MSK4);
    const vector unsigned char perm_sl = (vector unsigned char)
	(1, 2, 3, 23, 5, 6, 7, 0, 9, 10, 11, 4, 13, 14, 15, 8);
    const vector unsigned char perm = (vector unsigned char)PERM;

    /* read from sfmt */
    lung = sfmt[N];
    r = sfmt[N - 1];
    for (i = 0; i < N - POS1; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1], r, lung, sl1, sr1, 
			  mask, perm_sl, perm);
	array[i] = r;
	lung = vec_xor(lung, r);
    }
    for (; i < N; i++) {
	r = vec_recursion(sfmt[i], array[i + POS1 - N], r, lung, sl1, sr1,
			  mask, perm_sl, perm);
	array[i] = r;
	lung = vec_xor(lung, r);
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = vec_recursion(array[i - N], array[i + POS1 - N], r, lung, sl1, sr1,
			  mask, perm_sl, perm);
	array[i] = r;
	lung = vec_xor(lung, r);
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j] = array[j + size - N];
    }
    for (; i < size; i++) {
	r = vec_recursion(array[i - N], array[i + POS1 - N], r, lung, sl1, sr1,
			  mask, perm_sl, perm);
	array[i] = r;
	sfmt[j++] = r;
	lung = vec_xor(lung, r);
    }
    sfmt[N] = lung;
}

INLINE uint32_t gen_rand(void)
{
    uint32_t r;

    if (idx >= N * 4) {
	gen_rand_all();
	idx = 0;
    }
    r = sfmt32[idx++];
    return r;
}

INLINE void fill_array(uint32_t array[], int size)
{
    assert(size >= 2 * N * 4);
    assert(size % 4 == 0);

    gen_rand_array((vector unsigned int *)array, size / 4);

}

INLINE void init_gen_rand(uint32_t seed)
{
    int i;

    sfmt32[0] = seed;
    for (i = 1; i < (N + 1) * 4; i++) {
	sfmt32[i] = 1812433253UL 
	    * (sfmt32[i - 1] ^ (sfmt32[i - 1] >> 30)) + i;
    }
    idx = N * 4;
}

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif
