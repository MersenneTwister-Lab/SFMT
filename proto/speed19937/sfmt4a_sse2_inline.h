/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdio.h>
#include "random-inline.h"
#include <emmintrin.h>

#define MEXP 19937

#define WORDSIZE 128
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

INLINE static void gen_rand_array(__m128i array[], uint32_t blocks);
INLINE static void gen_rand_all(void);

static __m128i sfmt[N];
static unsigned int idx;

#define POS1 71
#define SL1 11
#define SL2 7
#define SR1 17

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

INLINE void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    fprintf(fp, "SL2 = %u\n", SL2);
    fprintf(fp, "SR1 = %u\n", SR1);
    fflush(fp);
}

INLINE void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SR1] = [%u,%u,%u,%u]\n", 
	    POS1, SL1, SL2, SR1);
    fflush(fp);
}

INLINE void print_state(FILE *fp) {
    int i;
    uint32_t *psfmt;

    psfmt = (uint32_t *)sfmt;
    for (i = 0; i < N; i++) {
	fprintf(fp, "%08x ", psfmt[i]);
	if (i % 8 == 7) {
	    fprintf(fp, "\n");
	}
    }
}

INLINE void gen_rand_all(void) {
    int i;
    __m128i a, b, c, v, w;

    a = _mm_load_si128(&sfmt[0]);
    b = _mm_load_si128(&sfmt[POS1]);
    c = _mm_load_si128(&sfmt[N - 1]);
    v = _mm_slli_si128(b, 4);
    w = _mm_srli_epi32(b, SR1);
    b = _mm_xor_si128(v, w);
    v = _mm_srli_si128(c, 4);
    w = _mm_slli_epi32(c, SL2);
    c = _mm_xor_si128(v, w);
    v = _mm_slli_epi32(a, SL1);
    a = _mm_xor_si128(a, v);
    w = _mm_xor_si128(b, c);
    w = _mm_xor_si128(w, a);
    _mm_store_si128(&sfmt[0], w);
    for (i = 1; i < N - POS1; i++) {
	a = _mm_load_si128(&sfmt[i]);
	b = _mm_load_si128(&sfmt[i + POS1]);
	c = w;
	v = _mm_slli_si128(b, 4);
	w = _mm_srli_epi32(b, SR1);
	b = _mm_xor_si128(v, w);
	v = _mm_srli_si128(c, 4);
	w = _mm_slli_epi32(c, SL2);
	c = _mm_xor_si128(v, w);
	v = _mm_slli_epi32(a, SL1);
	a = _mm_xor_si128(a, v);
	w = _mm_xor_si128(b, c);
	w = _mm_xor_si128(w, a);
	_mm_store_si128(&sfmt[i], w);
    }
    for (; i < N; i++) {
	a = _mm_load_si128(&sfmt[i]);
	b = _mm_load_si128(&sfmt[i + POS1 - N]);
	c = w;
	v = _mm_slli_si128(b, 4);
	w = _mm_srli_epi32(b, SR1);
	b = _mm_xor_si128(v, w);
	v = _mm_srli_si128(c, 4);
	w = _mm_slli_epi32(c, SL2);
	c = _mm_xor_si128(v, w);
	v = _mm_slli_epi32(a, SL1);
	a = _mm_xor_si128(a, v);
	w = _mm_xor_si128(b, c);
	w = _mm_xor_si128(w, a);
	_mm_store_si128(&sfmt[i], w);
    }
}

INLINE static void gen_rand_array(__m128i array[], uint32_t blocks) {
    int i;
    __m128i a, b, c, v, w;

    a = _mm_load_si128(&array[0]);
    b = _mm_load_si128(&array[POS1]);
    c = _mm_load_si128(&array[N - 1]);
    v = _mm_slli_si128(b, 4);
    w = _mm_srli_epi32(b, SR1);
    b = _mm_xor_si128(v, w);
    v = _mm_srli_si128(c, 4);
    w = _mm_slli_epi32(c, SL2);
    c = _mm_xor_si128(v, w);
    v = _mm_slli_epi32(a, SL1);
    a = _mm_xor_si128(a, v);
    w = _mm_xor_si128(b, c);
    w = _mm_xor_si128(w, a);
    _mm_store_si128(&array[0], w);
    for (i = 1; i < N - POS1; i++) {
	a = _mm_load_si128(&array[i]);
	b = _mm_load_si128(&array[i + POS1]);
	c = w;
	v = _mm_slli_si128(b, 4);
	w = _mm_srli_epi32(b, SR1);
	b = _mm_xor_si128(v, w);
	v = _mm_srli_si128(c, 4);
	w = _mm_slli_epi32(c, SL2);
	c = _mm_xor_si128(v, w);
	v = _mm_slli_epi32(a, SL1);
	a = _mm_xor_si128(a, v);
	w = _mm_xor_si128(b, c);
	w = _mm_xor_si128(w, a);
	_mm_store_si128(&array[i], w);
    }
    for (; i < N * blocks; i++) {
	a = _mm_load_si128(&array[i]);
	b = _mm_load_si128(&array[i + POS1 - N]);
	c = w;
	v = _mm_slli_si128(b, 4);
	w = _mm_srli_epi32(b, SR1);
	b = _mm_xor_si128(v, w);
	v = _mm_srli_si128(c, 4);
	w = _mm_slli_epi32(c, SL2);
	c = _mm_xor_si128(v, w);
	v = _mm_slli_epi32(a, SL1);
	a = _mm_xor_si128(a, v);
	w = _mm_xor_si128(b, c);
	w = _mm_xor_si128(w, a);
	_mm_store_si128(&array[i], w);
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
    if (block_num == 0) {
	return;
    } else if (block_num == 1) {
	gen_rand_all();
	memcpy(array, sfmt, sizeof(sfmt));
    } else {
	memcpy(array, sfmt, sizeof(sfmt));
	gen_rand_array((__m128i *)array, block_num);
	memcpy(sfmt, &array[N * (block_num-1)], sizeof(sfmt));
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
