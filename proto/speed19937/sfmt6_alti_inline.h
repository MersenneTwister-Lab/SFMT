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

#define POS1 71
#define SL1 28
#define SL2 16
#define SL3 22
#define SL4 18
#define SR1 7
#define SR2 2
#define SR3 10
#define SR4 27

#define MAX_BLOCKS (DST_MAX_BLOCK + 2)

INLINE static void gen_rand_array(vector unsigned int array[], uint32_t blocks);
INLINE static void gen_rand_all(void);

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
    fprintf(fp, "SL3 = %u\n", SL3);
    fprintf(fp, "SL4 = %u\n", SL4);
    fprintf(fp, "SR1 = %u\n", SR1);
    fprintf(fp, "SR2 = %u\n", SR2);
    fprintf(fp, "SR3 = %u\n", SR3);
    fprintf(fp, "SR4 = %u\n", SR4);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SL3, SL4, SR1, SR2, SR3, SR4] = "
	    "[%u,%u,%u,%u,%u,%u,%u,%u,%u]\n", 
	    POS1, SL1, SL2, SL3, SL4, SR1, SR2, SR3, SR4);
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

INLINE void gen_rand_all(void) {
    int i;
    vector unsigned int a, b, c, r;
    vector unsigned int sl = (vector unsigned int)(SL1, SL2, SL3, SL4);
    vector unsigned int sr = (vector unsigned int)(SR1, SR2, SR3, SR4);
    vector unsigned char perm = (vector unsigned char)
	(4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3);

    vec_dst(sfmt, DST_TOUCH_BLOCK(1), 3);
    a = sfmt[0];
    b = sfmt[POS1];
    c = sfmt[N - 1];
    r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl), a),
			vec_perm(b, b, perm)),
		vec_xor(vec_sr(c, sr), c));
    sfmt[0] = r;
    for (i = 1; i < N - POS1; i++) {
	a = sfmt[i];
	b = sfmt[i + POS1];
	c = r;
	r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl), a),
			    vec_perm(b, b, perm)),
		    vec_xor(vec_sr(c, sr), c));
	sfmt[i] = r;
    }
    for (; i < N; i++) {
	a = sfmt[i];
	b = sfmt[i + POS1 - N];
	c = r;
	r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl), a),
			    vec_perm(b, b, perm)),
		    vec_xor(vec_sr(c, sr), c));
	sfmt[i] = r;
    }
    vec_dss(3);
}

INLINE static void gen_rand_array(vector unsigned int array[], uint32_t blocks)
{
    int i, j, k;
    vector unsigned int a, b, c, r;
    vector unsigned int sl = (vector unsigned int)(SL1, SL2, SL3, SL4);
    vector unsigned int sr = (vector unsigned int)(SR1, SR2, SR3, SR4);
    vector unsigned char perm = (vector unsigned char)
	(4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3);

    /* read from sfmt */
    vec_dstst(&array[0], DST_TOUCH_BLOCK(1), 0);
    vec_dst(&sfmt[0], DST_TOUCH_BLOCK(1), 0);
    a = sfmt[0];
    b = sfmt[POS1];
    c = sfmt[N - 1];
    r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl), a),
			vec_perm(b, b, perm)),
		vec_xor(vec_sr(c, sr), c));
    array[0] = r;
    for (i = 1; i < N - POS1; i++) {
	a = sfmt[i];
	b = sfmt[i + POS1];
	c = r;
	r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl), a),
			    vec_perm(b, b, perm)),
		    vec_xor(vec_sr(c, sr), c));
	array[i] = r;
    }
    for (; i < N; i++) {
	a = sfmt[i];
	b = array[i + POS1 - N];
	c = r;
	r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl), a),
			vec_perm(b, b, perm)),
		vec_xor(vec_sr(c, sr), c));
	array[i] = r;
    }
    /* main loop */
    for (j = 0; j < (blocks - 1) / DST_MAX_BLOCK; j++) {
	vec_dstst(&array[i], DST_TOUCH_BLOCK(DST_MAX_BLOCK), 0);
	for (k = 0; k < N * DST_MAX_BLOCK; k++) {
	    a = vec_ldl(0, &array[i + k - N]);
	    b = array[i + k + POS1 - N];
	    c = r;
	    r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl), a),
				vec_perm(b, b, perm)),
			vec_xor(vec_sr(c, sr), c));
	    array[i + k] = r;
	}
	i += N * DST_MAX_BLOCK;
    }
    /* remainder loop */
    vec_dstst(&array[i], DST_TOUCH_BLOCK(DST_MAX_BLOCK - j), 0);
    for (; i < N * (blocks - 1); i++) {
	a = vec_ldl(0, &array[i - N]);
	b = array[i + POS1 - N];
	c = r;
	r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl), a),
			    vec_perm(b, b, perm)),
		    vec_xor(vec_sr(c, sr), c));
	array[i] = r;
    }
    /* write back to sfmt */
    vec_dstst(sfmt, DST_TOUCH_BLOCK(1), 1);
    for (j = 0; i < N * blocks; i++, j++) {
	a = vec_ldl(0, &array[i - N]);
	b = array[i + POS1 - N];
	c = r;
	r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl), a),
			    vec_perm(b, b, perm)),
		    vec_xor(vec_sr(c, sr), c));
	array[i] = r;
	sfmt[j] = r;
    }
    vec_dss(0);
    vec_dss(1);
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
#if 1
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
