/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdio.h>
#include "random-inline.h"

#define MEXP 19937

#define WORDSIZE 32
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

static uint32_t sfmt[N + 4] __attribute__ ((aligned (16)));
static unsigned int idx;

#define POS1 11
#define SL1 5
#define SL2 11
#define SL3 7
#define SR1 13
#define SR2 4

#define MAX_BLOCKS 10

INLINE static void gen_rand_array(uint32_t array[], uint32_t blocks);
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
    return N;
}

void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    fprintf(fp, "SL2 = %u\n", SL2);
    fprintf(fp, "SR1 = %u\n", SR1);
    fflush(fp);
}

#if 0
void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SL3, SL4, SR1, SR2, SR3, SR4] = "
	    "[%u,%u,%u,%u,%u,%u,%u,%u,%u]\n", 
	    POS1, SL1, SL2, SL3, SL4, SR1, SR2, SR3, SR4);
    fflush(fp);
}
#endif

INLINE void print_state(FILE *fp) {
    int i;
    for (i = 0; i < N; i++) {
	fprintf(fp, "%08x ", sfmt[i]);
	if (i % 8 == 7) {
	    fprintf(fp, "\n");
	}
    }
}

INLINE void gen_rand_all(void) {
    int i;
    vector unsigned int a, b, c, r, x, y, z, w;
    vector unsigned int sl1 = (vector unsigned int)(SL1, SL1, SL1, SL1);
    vector unsigned int sl2 = (vector unsigned int)(SL2, SL2, SL2, SL2);
    vector unsigned int sl3 = (vector unsigned int)(SL3, SL3, SL3, SL3);
    vector unsigned int sr1 = (vector unsigned int)(SR1, SR1, SR1, SR1);
    vector unsigned char perm = (vector unsigned char)
	(12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27);

    x = vec_ld(POS1, sfmt);
    y = vec_ld(POS1 + 4, sfmt);
    b = vec_perm(x, y, perm);
    z = vec_ld(0, &sfmt[N - 4]);
    w = vec_ld(0, &sfmt[N]);
    c = vec_perm(z, w, perm);
    a = vec_ld(0, sfmt);
    r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl1), a),
			vec_xor(vec_sl(b, sl2), b)),
		vec_xor(vec_sr(c, sr1), vec_sl(c, sl3)));
    vec_st(r, 0, sfmt);
    for (i = 4; i < N - POS1 - 4; i += 4) {
	x = vec_ld(POS1, sfmt + i);
	y = vec_ld(POS1 + 4, sfmt + i);
	b = vec_perm(x, y, perm);
	a = vec_ld(0, sfmt + i);
	c = r;
	r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl1), a),
			    vec_xor(vec_sl(b, sl2), b)),
		    vec_xor(vec_sr(c, sr1), vec_sl(c, sl3)));
	vec_st(r, 0, sfmt + i);
    }
    x = vec_ld(POS1, sfmt + i);
    y = vec_ld(0, sfmt);
    b = vec_perm(x, y, perm);
    a = vec_ld(0, sfmt + i);
    c = r;
    r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl1), a),
			vec_xor(vec_sl(b, sl2), b)),
		vec_xor(vec_sr(c, sr1), vec_sl(c, sl3)));
    vec_st(r, 0, sfmt + i);
    i += 4;
    for (; i < N - 4; i += 4) {
	x = vec_ld(POS1, sfmt + i - N);
	y = vec_ld(POS1 + 4, sfmt + i - N);
	b = vec_perm(x, y, perm);
	a = vec_ld(0, sfmt + i);
	c = r;
	r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl1), a),
			    vec_xor(vec_sl(b, sl2), b)),
		    vec_xor(vec_sr(c, sr1), vec_sl(c, sl3)));
	vec_st(r, 0, sfmt + i);
    }
    for (; i < N; i++) {
	sfmt[i] = sfmt[i] ^ (sfmt[i] << SL1)
	    ^ sfmt[i + N - POS1] ^ (sfmt[i + N - POS1] << SL2)
	    ^ (sfmt[i - 4] << SL3) ^ (sfmt[i - 4] >> SR1);
    }
}

INLINE static void gen_rand_array(uint32_t array[], uint32_t blocks)
{
    int i;
    vector unsigned int a, b, c, r, x, y;
    vector unsigned int sl1 = (vector unsigned int)(SL1, SL1, SL1, SL1);
    vector unsigned int sl2 = (vector unsigned int)(SL2, SL2, SL2, SL2);
    vector unsigned int sl3 = (vector unsigned int)(SL3, SL3, SL3, SL3);
    vector unsigned int sr1 = (vector unsigned int)(SR1, SR1, SR1, SR1);
    vector unsigned char perm = (vector unsigned char)
	(12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27);

    for (i = 0; i < 4; i++) {
	array[i] = array[i] ^ (array[i] << SL1)
	    ^ array[i + POS1] ^ (array[i + POS1] << SL2)
	    ^ (array[i + N - 4] << SL3) ^ (array[i + N - 4] >> SR1);
    }
    for (; ((long)&array[i] & 0x03UL) != 0; i++) {
	array[i] = array[i] ^ (array[i] << SL1)
	    ^ array[i + POS1] ^ (array[i + POS1] << SL2)
	    ^ (array[i - 4] << SL3) ^ (array[i - 4] >> SR1);
    }
    r = vec_ld(0, &array[i - 4]);
    for (i = 1; i < N - POS1 - 4; i += 4) {
	a = vec_ld(0, &array[i]);
	x = vec_ld(0, &array[i + POS1]);
	y = vec_ld(0, &array[i + POS1 + 4]);
	b = vec_perm(x, y, perm);
	c = r;
	r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl1), a),
			    vec_xor(vec_sl(b, sl2), b)),
		    vec_xor(vec_sr(c, sr1), vec_sl(c, sl3)));
	vec_st(r, 0, &array[i]);
    }
    a = vec_ld(0, &array[i]);
    x = vec_ld(0, &array[i + POS1]);
    y = vec_ld(0, &array[0]);
    b = vec_perm(x, y, perm);
    c = r;
    r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl1), a),
			vec_xor(vec_sl(b, sl2), b)),
		vec_xor(vec_sr(c, sr1), vec_sl(c, sl3)));
    vec_st(r, 0, &array[i]);
    i += 4;
    for (; i < N - 4; i += 4) {
	a = vec_ld(0, &array[i]);
	x = vec_ld(0, &array[i + POS1 - N]);
	y = vec_ld(0, &array[i + POS1 - N + 4]);
	b = vec_perm(x, y, perm);
	c = r;
	r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl1), a),
			    vec_xor(vec_sl(b, sl2), b)),
		vec_xor(vec_sr(c, sr1), vec_sl(c, sl3)));
	vec_st(r, 0, &array[i]);
    }
    for (; i < N; i++) {
	array[i] = array[i] ^ (array[i] << SL1)
	    ^ array[i + POS1 - N] ^ (array[i + POS1 - N] << SL2)
	    ^ (array[i - 4] << SL3) ^ (array[i - 4] >> SR1);
    }
    for (; ((long)&array[i] & 0x03UL) != 0; i++) {
	array[i] = array[i - N] ^ (array[i - N] << SL1)
	    ^ array[i + POS1 - N] ^ (array[i + POS1 - N] << SL2)
	    ^ (array[i - 4] << SL3) ^ (array[i - 4] >> SR1);
    }
    r = vec_ld(0, &array[i - 4]);
    for (; i < N * blocks - 4; i += 4) {
	a = vec_ld(0, &array[i - N]);
	x = vec_ld(0, &array[i + POS1 - N]);
	y = vec_ld(0, &array[i + POS1 - N + 4]);
	b = vec_perm(x, y, perm);
	c = r;
	r = vec_xor(vec_xor(vec_xor(vec_sl(a, sl1), a),
			    vec_xor(vec_sl(b, sl2), b)),
		    vec_xor(vec_sr(c, sr1), vec_sl(c, sl3)));
	vec_st(r, 0, &array[i]);
    }
    for (; i < N * blocks; i++) {
	array[i] = array[i] ^ (array[i] << SL1)
	    ^ array[i + POS1 - N] ^ (array[i + POS1 - N] << SL2)
	    ^ (array[i - 4] << SL3) ^ (array[i - 4] >> SR1);
    }
}

INLINE uint32_t gen_rand(void)
{
    uint32_t r;

    if (idx >= N) {
	gen_rand_all();
	idx = 0;
    }
    r = sfmt[idx++];
    return r;
}

INLINE void fill_array_block(uint32_t array[], uint32_t block_num)
{
    while (block_num > MAX_BLOCKS) {
	memcpy(array, sfmt, sizeof(sfmt));
	gen_rand_array(array, MAX_BLOCKS);
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
	gen_rand_array(array, block_num);
	memcpy(sfmt, &array[N * (block_num-1)], sizeof(sfmt));
    }
}

INLINE void fill_array(uint32_t array[], uint32_t size) 
{
    if (size < N - idx) {
	memcpy(array, sfmt, size * sizeof(uint32_t));
	idx += size;
	return;
    }
    if (idx < N) {
	memcpy(array, sfmt, ((N * 4) - idx) * sizeof(uint32_t));
	array += N - idx;
	size -= N - idx;
    }
    while (size >= N) {
	gen_rand_all();
	memcpy(array, sfmt, sizeof(sfmt));
	array += N;
	size -= N;
    }
    if (size > 0) {
	gen_rand_all();
	memcpy(array, sfmt, size * sizeof(uint32_t));
	idx = size;
    } else {
	idx = N;
    }
}

INLINE void init_gen_rand(uint32_t seed)
{
    int i;

    for (i = 1; i < N * 4; i++) {
	sfmt[i] = seed;
	seed = 1812433253UL * (seed ^ (seed >> 30)) + i;
    }
    idx = N;
}
