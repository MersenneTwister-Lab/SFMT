/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdio.h>
#include "random-inline.h"
#include <emmintrin.h>

#define MEXP 19937

#define WORDSIZE 128
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

//#define MAX_BLOCKS 10

INLINE static void gen_rand_array(__m128i array[], uint32_t blocks);
INLINE static void gen_rand_all(void);

static __m128i sfmt[N];
static unsigned int idx;
#define SHUFF 0x39

#define POS1 70
#define SL1 20
#define SL2 1
#define SL3 23
#define SR1 7
#define MSK1 0xffbfffb7UL
#define MSK2 0xfbffff72UL
#define MSK3 0xfeff5fffUL
#define MSK4 0xffbff7ffUL

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
    fprintf(fp, "SR1 = %u\n", SR1);
    fprintf(fp, "MSK1 = %lu\n", MSK1);
    fprintf(fp, "MSK2 = %lu\n", MSK2);
    fprintf(fp, "MSK3 = %lu\n", MSK3);
    fprintf(fp, "MSK4 = %lu\n", MSK4);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SL3, SR1, MSK1, MSK2, MSK3, MSK4] = "
	    "[%u,%u,%u,%lu,%lu,%lu,%lu, %lu, %lu]\n", 
	    POS1, SL1, SL2, SL3, SR1, MSK1, MSK2, MSK3, MSK4);
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

INLINE static __m128i mm_recursion(__m128i *a, __m128i *b, 
				   __m128i c, __m128i d, __m128i mask) {
    __m128i v, x, y, z;
    
    x = _mm_load_si128(a);
    y = _mm_load_si128(b);
    z = _mm_xor_si128(c, d);
    v = _mm_srli_epi32(c, SR1);
    z = _mm_xor_si128(z, v);
    v = _mm_slli_epi32(d, SL3);
    z = _mm_xor_si128(z, v);
    z = _mm_xor_si128(z, x);
    x = _mm_slli_si128(x, SL2);
    v = _mm_shuffle_epi32(y, SHUFF);
    v = _mm_and_si128(v, mask);
    y = _mm_slli_epi32(y, SL1);
    z = _mm_xor_si128(z, x);
    z = _mm_xor_si128(z, v);
    z = _mm_xor_si128(z, y);
    return z;
}

INLINE void gen_rand_all(void) {
    int i;
    __m128i r, r1, r2, mask;
    mask = _mm_set_epi32(MSK4, MSK3, MSK2, MSK1);

    r1 = _mm_load_si128(&sfmt[N - 2]);
    r2 = _mm_load_si128(&sfmt[N - 1]);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1], r1, r2, mask);
	_mm_store_si128(&sfmt[i], r);
	r1 = r2;
	r2 = r;
    }
    for (; i < N; i++) {
	r = mm_recursion(&sfmt[0], &sfmt[i + POS1 - N], r1, r2, mask);
	_mm_store_si128(&sfmt[i], r);
	r1 = r2;
	r2 = r;
    }
}

INLINE static void gen_rand_array(__m128i array[], uint32_t blocks) {
    int i;
    __m128i r, r1, r2, mask;
    mask = _mm_set_epi32(MSK4, MSK3, MSK2, MSK1);

    r1 = _mm_load_si128(&array[N - 2]);
    r2 = _mm_load_si128(&array[N - 1]);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&array[i], &array[i + POS1], r1, r2, mask);
	_mm_store_si128(&array[i], r);
	r1 = r2;
	r2 = r;
    }
    for (; i < N; i++) {
	r = mm_recursion(&array[i], &array[i + POS1 - N], r1, r2, mask);
	_mm_store_si128(&array[i], r);
	r1 = r2;
	r2 = r;
    }
    for (; i < N * blocks; i++) {
	r = mm_recursion(&array[i - N], &array[i + POS1 - N], r1, r2, mask);
	_mm_store_si128(&array[i], r);
	r1 = r2;
	r2 = r;
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
	memcpy(array, sfmt, sizeof(sfmt));
	gen_rand_array((__m128i *)array, MAX_BLOCKS);
	memcpy(sfmt, &array[N * (MAX_BLOCKS - 1)], sizeof(sfmt));
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

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif
