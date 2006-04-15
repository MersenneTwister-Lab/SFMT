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

#define POS1 124
#define SL1 1
#define SR1 7
#define MSK1 0xf7f0bfffUL
#define MSK2 0xffad7fddUL
#define MSK3 0xefef7f33UL
#define MSK4 0x2d7ce57fUL

#define MAX_BLOCKS (DST_MAX_BLOCK + 2)

INLINE static void gen_rand_array(vector unsigned int array[], uint32_t blocks);
INLINE static void gen_rand_all(void);
INLINE static vector unsigned int vec_recursion(vector unsigned int a,
						vector unsigned int b,
						vector unsigned int c);

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
    fprintf(fp, "SR1 = %u\n", SR1);
    fprintf(fp, "MSK1 = %lu\n", MSK1);
    fprintf(fp, "MSK2 = %lu\n", MSK2);
    fprintf(fp, "MSK3 = %lu\n", MSK3);
    fprintf(fp, "MSK4 = %lu\n", MSK4);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SR1, MSK1, MSK2, MSK3, MSK4] = "
	    "[%u,%u,%u,%lu,%lu,%lu,%lu]\n", 
	    POS1, SL1, SR1, MSK1, MSK2, MSK3, MSK4);
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
						vector unsigned int c) {
    vector unsigned int sl = (vector unsigned int)(SL1, SL1, SL1, SL1);
    vector unsigned int sr = (vector unsigned int)(SR1, SR1, SR1, SR1);
    vector unsigned int mask = (vector unsigned int)(MSK1, MSK2, MSK3, MSK4);
    vector unsigned char perm = (vector unsigned char)
	(4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3);
    return vec_xor(vec_xor(vec_xor(a, vec_perm(b, b, perm)),
			   vec_sl(b, sl)), 
		   vec_xor(vec_and(c, mask), vec_sr(c, sr)));
}

INLINE void gen_rand_all(void) {
    int i;
    vector unsigned int r;

    //vec_dst(sfmt, DST_TOUCH_BLOCK(1), 3);
    r = vec_recursion(sfmt[0], sfmt[POS1], sfmt[N - 1]);
    sfmt[0] = r;
    for (i = 1; i < N - POS1; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1], r);
	sfmt[i] = r;
    }
    for (; i < N; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1 - N], r);
	sfmt[i] = r;
    }
    //vec_dss(3);
}

INLINE static void gen_rand_array(vector unsigned int array[], uint32_t blocks)
{
    int i, j;
    vector unsigned int r;
 
    /* read from sfmt */
    //vec_dstst(&array[0], DST_TOUCH_BLOCK(1), 0);
    //vec_dst(&sfmt[0], DST_TOUCH_BLOCK(1), 0);
    r = vec_recursion(sfmt[0], sfmt[POS1], sfmt[N - 1]);
    array[0] = r;
    for (i = 1; i < N - POS1; i++) {
	r = vec_recursion(sfmt[i], sfmt[i + POS1], r);
	array[i] = r;
    }
    for (; i < N; i++) {
	r = vec_recursion(sfmt[i], array[i + POS1 - N], r);
	array[i] = r;
    }
    /* main loop */
    for (; i < N * (blocks - 1); i++) {
	r = vec_recursion(array[i - N], array[i + POS1 - N], r);
	array[i] = r;
    }
    for (j = 0; i < N * blocks; i++) {
	r = vec_recursion(array[i - N], array[i + POS1 - N], r);
	array[i] = r;
	sfmt[j++] = r;
    }
}

INLINE uint32_t gen_rand(void)
{
    uint32_t r;
    uint32_t *sfmtp = (uint32_t *)sfmt;

    if (idx >= N * 4) {
	gen_rand_all();
	idx = 0;
    }
    r = sfmtp[idx++];
    return r;
}

INLINE void fill_array_block(uint32_t array[], uint32_t block_num)
{
#if 0
    while (block_num > MAX_BLOCKS) {
	//memcpy(array, sfmt, sizeof(sfmt));
	gen_rand_array((vector unsigned int *)array, MAX_BLOCKS);
	//memcpy(sfmt, &array[N * (MAX_BLOCKS - 1)], sizeof(sfmt));
	array += N * MAX_BLOCKS;
	block_num -= MAX_BLOCKS;
    }
#endif
    if (block_num == 0) {
	return;
    } else if (block_num == 1) {
	gen_rand_all();
	memcpy(array, sfmt, sizeof(sfmt));
    } else {
	//memcpy(array, sfmt, sizeof(sfmt));
	gen_rand_array((vector unsigned int *)array, block_num);
	//memcpy(sfmt, &array[N * (block_num-1)], sizeof(sfmt));
    }
}

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
