/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdio.h>
#include <ppc_intrinsics.h>
#include <assert.h>
#include "random.h"
#include "speed.h"

#define MEXP 19937

#define WORDSIZE 128
#define N (MEXP / WORDSIZE)
#define MAXDEGREE (WORDSIZE * (N + 1))

#define M_RAN_INVM32 2.32830643653869628906e-010

static vector unsigned int sfmt[N + 1];
static int idx;
static int idx64;
static uint32_t *sfmt32 = (uint32_t *)sfmt;
static uint64_t *sfmt64 = (uint64_t *)sfmt;

#define POS1 4
#define SL1 20
#define SL2 1
#define SR1 1
#define SR2 1
#define MSK1 0xfffef7f9U
#define MSK2 0xffdff7fbU
#define MSK3 0xcbfff7feU
#define MSK4 0xedfefffdU

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
						vector unsigned char perm_sr);

INLINE static __attribute__((always_inline))
    vector unsigned int vec_recursion(vector unsigned int a,
				      vector unsigned int b,
				      vector unsigned int c,
				      vector unsigned int d,
				      vector unsigned int sl1,
				      vector unsigned int sr1,
				      vector unsigned int mask,
				      vector unsigned char perm_sl,
				      vector unsigned char perm_sr
) {

    vector unsigned int v, w, x, y, z;
    x = vec_perm(a, perm_sl, perm_sl);
    z = vec_perm(c, perm_sr, perm_sr);
    y = vec_sr(b, sr1);
    v = a;
    w = vec_sl(d, sl1);
    z = vec_xor(z, w);
    y = vec_and(y, mask);
    v = vec_xor(v, x);
    z = vec_xor(z, y);
    z = vec_xor(z, v);
    return z;
}

INLINE void gen_rand_all(void) {
    int i;
    vector unsigned int r, r1, r2;
    const vector unsigned int sl1 = (vector unsigned int)(SL1, SL1, SL1, SL1);
    const vector unsigned int sr1 = (vector unsigned int)(SR1, SR1, SR1, SR1);
    const vector unsigned int mask = (vector unsigned int)
    (MSK1, MSK2, MSK3, MSK4);
    const vector unsigned char perm_sl = (vector unsigned char)
    (1, 2, 3, 23, 5, 6, 7, 0, 9, 10, 11, 4, 13, 14, 15, 8);
        const vector unsigned char perm_sr = (vector unsigned char)
    (7, 0, 1, 2, 11, 4, 5, 6, 15, 8, 9, 10, 17, 12, 13, 14);

    r1 = sfmt[N - 2];
    r2 = sfmt[N - 1];
    for (i = 0; i < N - POS1; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1], r1, r2, sl1, sr1, 
			  mask, perm_sl, perm_sr);
	sfmt[i] = r;
	r1 = r2;
	r2 = r;
    }
    for (; i < N; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1 - N], r1, r2, sl1, sr1,
			  mask, perm_sl, perm_sr);
	sfmt[i] = r;
	r1 = r2;
	r2 = r;
    }
}

INLINE static void gen_rand_array(vector unsigned int array[], int size)
{
    int i, j;
    vector unsigned int r, r1, r2;
 
    const vector unsigned int sl1 = (vector unsigned int)(SL1, SL1, SL1, SL1);
    const vector unsigned int sr1 = (vector unsigned int)(SR1, SR1, SR1, SR1);
    const vector unsigned int mask = (vector unsigned int)
    (MSK1, MSK2, MSK3, MSK4);
    const vector unsigned char perm_sl = (vector unsigned char)
    (1, 2, 3, 23, 5, 6, 7, 0, 9, 10, 11, 4, 13, 14, 15, 8);
    const vector unsigned char perm_sr = (vector unsigned char)
    (7, 0, 1, 2, 11, 4, 5, 6, 15, 8, 9, 10, 17, 12, 13, 14);

    /* read from sfmt */
    //vec_dstst(&array[0], DST_TOUCH_BLOCK(1), 0);
    //vec_dst(&sfmt[0], DST_TOUCH_BLOCK(1), 0);
    r1 = sfmt[N - 2];
    r2 = sfmt[N - 1];
    for (i = 0; i < N - POS1; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1], r1, r2, sl1, sr1,
			  mask, perm_sl, perm_sr);
	array[i] = r;
	r1 = r2;
	r2 = r;
    }
    for (; i < N; i++) {
	r = vec_recursion(sfmt[i], array[i + POS1 - N], r1, r2, sl1, sr1,
			  mask, perm_sl, perm_sr);
	array[i] = r;
	r1 = r2;
	r2 = r;
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = vec_recursion(array[i - N], array[i + POS1 - N], r1, r2, sl1,
			  sr1, mask, perm_sl, perm_sr);
	array[i] = r;
	r1 = r2;
	r2 = r;
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j] = array[j + size - N];
    }
     for (; i < size; i++) {
	r = vec_recursion(array[i - N], array[i + POS1 - N], r1, r2, sl1,
			  sr1, mask, perm_sl, perm_sr);
	array[i] = r;
	sfmt[j++] = r;
	r1 = r2;
	r2 = r;
    }
}

INLINE uint32_t genrand_int32(void)
{
    uint32_t r;

    if (idx >= N * 4) {
	gen_rand_all();
	idx = 0;
    }
    r = sfmt32[idx++];
    return r;
}

INLINE void fill_array(double array[], int size)
{
    int i, j;
    vector unsigned int low_mask = (vector unsigned int)
	(0x000fffff, 0xffffffff, 0x000fffff, 0xffffffff);
    vector unsigned int high_const = (vector unsigned int)
	(0xbff00000, 0, 0xbff00000, 0);
    vector unsigned int *ap;
    vector unsigned int r;

    assert(size >= N * 2);
    assert(size % 2 == 0);

    gen_rand_array((vector unsigned int *)array, size / 2);
    ap = (vector unsigned int *)array;
    for (i = 0, j = 0; i < size / 2; i++) {
	r = ap[i];
	r = vec_and(r, low_mask);
	r = vec_or(r, high_const);
	ap[i] = r;
	array[j++] += 2.0L;
	array[j++] += 2.0L;
    }
}

INLINE void init_gen_rand(uint64_t seed)
{
    int i;

    sfmt32[0] = (uint32_t)seed;
    for (i = 1; i < (N + 1) * 4; i++) {
	sfmt32[i] = 1812433253UL 
	    * (sfmt32[i - 1] ^ (sfmt32[i - 1] >> 30)) + i;
    }
    idx = N * 4;
    idx64 = N * 2;
}

INLINE void init_genrand(uint32_t seed) {
    init_gen_rand(seed);
}

/* generates a random number on [0,1]-real-interval */
INLINE  __attribute__((always_inline))
    double gen_rand(void)
{
#define LOW_MASK  0x000FFFFFFFFFFFFFULL
#define HIGH_CONST 0xBFF0000000000000ULL
    uint64_t r;
    double *dp = (double *)&r;

    if (idx64 >= N * 2) {
	gen_rand_all();
	idx = 0;
	idx64 = 0;
    }
    r = sfmt64[idx64++];
    r &= LOW_MASK;
    r |= HIGH_CONST;
    *dp += 2.0L;
    return *dp;
}

/* JURGEN A DOORNIK */
INLINE double genrand_real2(void)
{
    return ((int)genrand_int32()) * M_RAN_INVM32 + 0.5;
}

/* generates a random number on [0,1) with 53-bit resolution*/
double genrand_res53(void) 
{ 
    unsigned long a=genrand_int32()>>5, b=genrand_int32()>>6; 
    return(a*67108864.0+b)*(1.0/9007199254740992.0); 
} 
#include "test_time.c"