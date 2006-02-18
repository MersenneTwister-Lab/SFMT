/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdio.h>
#include "random-inline.h"

#define MEXP 19937

#define WORDSIZE 128
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

static vector unsigned int sfmt[N];
static unsigned int idx;

#define POS1 71
#define SL1 23
#define SL2 20
#define SL3 11
#define SL4 12
#define SL5 20
#define SL6 13
#define SL7 28
#define SL8 31
#define SR1 1
#define SR2 1
#define SR3 1
#define SR4 14

#define MAX_BLOCKS 10

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
    fprintf(fp, "SL5 = %u\n", SL5);
    fprintf(fp, "SL6 = %u\n", SL6);
    fprintf(fp, "SL7 = %u\n", SL7);
    fprintf(fp, "SL8 = %u\n", SL8);
    fprintf(fp, "SR1 = %u\n", SR1);
    fprintf(fp, "SR2 = %u\n", SR2);
    fprintf(fp, "SR3 = %u\n", SR3);
    fprintf(fp, "SR4 = %u\n", SR4);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SL3, SL4, SL5, SL6, SL7, SL8,"
	    " SR1, SR2, SR3, SR4] = "
	    "[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u]\n", 
	    POS1, SL1, SL2, SL3, SL4, SL5, SL6, SL7, SL8, 
	    SR1, SR2, SR3, SR4);
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
    vector unsigned int sl1 = (vector unsigned int)(SL1, SL2, SL3, SL4);
    vector unsigned int sl2 = (vector unsigned int)(SL5, SL6, SL7, SL8);
    vector unsigned int sr = (vector unsigned int)(SR1, SR2, SR3, SR4);

    a = sfmt[0];
    b = sfmt[POS1];
    c = sfmt[N - 1];
    r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl1),	a),
			vec_xor(
			    vec_sr(b, sr),
			    vec_slo(b, (vector unsigned char)(4 << 3)))),
		vec_xor(vec_sl(c, sl2),
			vec_sro(c, (vector unsigned char)(4 << 3))));
    sfmt[0] = r;
    for (i = 1; i < N - POS1; i++) {
	a = sfmt[i];
	b = sfmt[i + POS1];
	c = r;
	r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl1), a),
			    vec_xor(
				vec_sr(b, sr),
				vec_slo(b, (vector unsigned char)(4 << 3)))),
		    vec_xor(vec_sl(c, sl2),
			    vec_sro(c, (vector unsigned char)(4 << 3))));
	sfmt[i] = r;
    }
    for (; i < N; i++) {
	a = sfmt[i];
	b = sfmt[i + POS1 - N];
	c = r;
	r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl1), a),
			    vec_xor(
				vec_sr(b, sr),
				vec_slo(b, (vector unsigned char)(4 << 3)))),
		    vec_xor(vec_sl(c, sl2),
			    vec_sro(c, (vector unsigned char)(4 << 3))));
	sfmt[i] = r;
    }
}

INLINE static void gen_rand_array(vector unsigned int array[], uint32_t blocks)
{
    int i;
    vector unsigned int a, b, c, r;
    vector unsigned int sl1 = (vector unsigned int)(SL1, SL2, SL3, SL4);
    vector unsigned int sl2 = (vector unsigned int)(SL5, SL6, SL7, SL8);
    vector unsigned int sr = (vector unsigned int)(SR1, SR2, SR3, SR4);

    a = array[0];
    b = array[POS1];
    c = array[N - 1];
    r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl1),	a),
			vec_xor(
			    vec_sr(b, sr),
			    vec_slo(b, (vector unsigned char)(4 << 3)))),
		vec_xor(vec_sl(c, sl2),
			vec_sro(c, (vector unsigned char)(4 << 3))));
    array[0] = r;
    for (i = 1; i < N - POS1; i++) {
	a = array[i];
	b = array[i + POS1];
	c = r;
	r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl1),	a),
			    vec_xor(
				vec_sr(b, sr),
				vec_slo(b, (vector unsigned char)(4 << 3)))),
		    vec_xor(vec_sl(c, sl2),
			    vec_sro(c, (vector unsigned char)(4 << 3))));
	array[i] = r;
    }
    for (; i < N; i++) {
	a = array[i];
	b = array[i + POS1 - N];
	c = r;
	r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl1),	a),
			    vec_xor(
				vec_sr(b, sr),
				vec_slo(b, (vector unsigned char)(4 << 3)))),
		    vec_xor(vec_sl(c, sl2),
			    vec_sro(c, (vector unsigned char)(4 << 3))));
	array[i] = r;
    }
    for (; i < N * blocks; i++) {
	a = array[i - N];
	b = array[i + POS1 - N];
	c = r;
	r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl1),	a),
			    vec_xor(
				vec_sr(b, sr),
				vec_slo(b, (vector unsigned char)(4 << 3)))),
		    vec_xor(vec_sl(c, sl2),
			    vec_sro(c, (vector unsigned char)(4 << 3))));
	array[i] = r;
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
    while (block_num > MAX_BLOCKS) {
	memcpy(array, sfmt, sizeof(sfmt));
	gen_rand_array((vector unsigned int *)array, MAX_BLOCKS);
	memcpy(sfmt, &array[N * (MAX_BLOCKS - 1)], sizeof(sfmt));
	array += N * MAX_BLOCKS;
	block_num -= MAX_BLOCKS;
    }
    if (block_num == 0) {
	return;
    } else if (block_num == 1) {
	gen_rand_all();
	memcpy(array, sfmt, sizeof(sfmt));
    } else {
	memcpy(array, sfmt, sizeof(sfmt));
	gen_rand_array((vector unsigned int *)array, block_num);
	memcpy(sfmt, &array[N * (block_num-1)], sizeof(sfmt));
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
