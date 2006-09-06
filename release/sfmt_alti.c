/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdio.h>
#include <ppc_intrinsics.h>
#include "random-inline.h"

#define MEXP 19937

#define WORDSIZE 128
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)
#define DST_TOUCH_BLOCK(blk) (32 | (((blk) * 20) << 16) | (2 << 24))
#define DST_MAX_BLOCK 12

static vector unsigned int sfmt[N];
static unsigned int idx;
static uint32_t *sfmt32 = (uint32_t *)sfmt;

#define POS1 4
#define SL1 20
#define SL2 1
#define SR1 1
#define SR2 1
#define MSK1 0xfffef7f9U
#define MSK2 0xffdff7fbU
#define MSK3 0xcbfff7feU
#define MSK4 0xedfefffdU

#define MAX_BLOCKS (DST_MAX_BLOCK + 2)

INLINE static void gen_rand_array(vector unsigned int array[], uint32_t blocks);
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

INLINE unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

INLINE unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

INLINE unsigned int get_onetime_rnds(void) {
    return N * 4;
}

void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    fprintf(fp, "SL2 = %u\n", SL2);
    fprintf(fp, "SR1 = %u\n", SR1);
    fprintf(fp, "SR2 = %u\n", SR2);
    fprintf(fp, "MSK1 = %u\n", MSK1);
    fprintf(fp, "MSK2 = %u\n", MSK2);
    fprintf(fp, "MSK3 = %u\n", MSK3);
    fprintf(fp, "MSK4 = %u\n", MSK4);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SR1, SR2, MSK1, MSK2, MSK3, MSK4] = "
	    "[%u,%u,%u,%u,%u,%u,%u,%u,%u]\n", 
	    POS1, SL1, SL2, SR1, SR2, MSK1, MSK2, MSK3, MSK4);
    fflush(fp);
}

INLINE void print_state(FILE *fp) {
    int i;
    for (i = 0; i < N; i++) {
	fprintf(fp, "%08vlx ", sfmt[i]);
	if (i % 2 == 1) {
	    fprintf(fp, "\n");
	}
    }
}

INLINE static vector unsigned int vec_recursion(vector unsigned int a,
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
    v = a;
    y = vec_sr(b, sr1);
    z = vec_perm(c, perm_sr, perm_sr);
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

    //vec_dst(sfmt, DST_TOUCH_BLOCK(1), 3);
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
    //vec_dss(3);
}

INLINE static void gen_rand_array(vector unsigned int array[], uint32_t blocks)
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
    for (; i < N * (blocks - 1); i++) {
	r = vec_recursion(array[i - N], array[i + POS1 - N], r1, r2, sl1,
			  sr1, mask, perm_sl, perm_sr);
	array[i] = r;
	r1 = r2;
	r2 = r;
    }
    for (j = 0; i < N * blocks; i++) {
	r = vec_recursion(array[i - N], array[i + POS1 - N], r1, r2, sl1,
			  sr1, mask, perm_sl, perm_sr);
	array[i] = r;
	sfmt[j++] = r;
	r1 = r2;
	r2 = r;
    }
}

INLINE uint32_t gen_rand(void)
{
    uint32_t r;
    //uint32_t *sfmtp = (uint32_t *)sfmt;

    if (idx >= N * 4) {
	gen_rand_all();
	idx = 0;
    }
    r = sfmt32[idx++];
    return r;
}

INLINE void fill_array_block(uint32_t array[], uint32_t block_num)
{
    if (block_num == 0) {
	return;
    } else if (block_num == 1) {
	gen_rand_all();
	memcpy(array, sfmt, sizeof(sfmt));
    } else {
	gen_rand_array((vector unsigned int *)array, block_num);
    }
}

#if 0
INLINE void fill_array(uint32_t array[], uint32_t size) 
{
    if (size < N * 4 - idx) {
	memcpy(array, sfmt, size * sizeof(uint32_t));
	idx += size;
	return;
    }
    if (idx < N * 4) {
	memcpy(array, sfmt, ((N * 4) - idx) * sizeof(uint32_t));
	array += N * 4 - idx;
	size -= N * 4 - idx;
    }
    while (size >= N * 4) {
	gen_rand_all();
	memcpy(array, sfmt, sizeof(sfmt));
	array += N * 4;
	size -= N * 4;
    }
    if (size > 0) {
	gen_rand_all();
	memcpy(array, sfmt, size * sizeof(uint32_t));
	idx = size;
    } else {
	idx = N * 4;
    }
}
#endif

INLINE void init_gen_rand(uint32_t seed)
{
    int i;
    uint32_t *sfmtp = (uint32_t *)sfmt;

    sfmtp[0] = seed;
    for (i = 1; i < N * 4; i++) {
	sfmtp[i] = 1812433253UL 
	    * (sfmtp[i - 1] ^ (sfmtp[i - 1] >> 30)) + i;
    }
    idx = N * 4;
}

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif
