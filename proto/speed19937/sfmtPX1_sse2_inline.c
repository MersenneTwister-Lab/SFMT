/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdio.h>
#if defined(__GNUC__)
  #include <inttypes.h>
#endif
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
static uint32_t *sfmtp = (uint32_t *)sfmt;
#define SHUFF 0x39

#define POS1 4
#define SL1 20
#define SL2 1
#define SR1 1
#define SR2 1
#define MSK1 0xfffef7f9U
#define MSK2 0xffdff7fbU
#define MSK3 0xcbfff7feU
#define MSK4 0xedfefffdU

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
    fprintf(fp, "[POS1, SL1, SL2, SL3, SR1, MSK1, MSK2, MSK3, MSK4] = "
	    "[%u,%u,%u,%u,%u,%u,%u, %u, %u]\n", 
	    POS1, SL1, SL2, SR1, SR2, MSK1, MSK2, MSK3, MSK4);
    fflush(fp);
}

INLINE void print_state(FILE *fp) {
    int i;

    for (i = 0; i < N; i++) {
	fprintf(fp, "%08" PRIx32 " ", sfmtp[i]);
	if (i % 8 == 7) {
	    fprintf(fp, "\n");
	}
    }
}

INLINE static __m128i mm_recursion(__m128i *a, __m128i *b, 
				   __m128i c, __m128i d, __m128i mask) {
    __m128i v, x, y, z;
    
    x = _mm_xor_si128(x, *a);
    y = _mm_load_si128(b);
    z = _mm_srli_si128(c, SR2);
    v = _mm_slli_epi32(d, SL1);
    z = _mm_xor_si128(z, v);
    z = _mm_xor_si128(z, x);
    x = _mm_slli_si128(x, SL2);
    y = _mm_srli_epi32(y, SR1);
    y = _mm_and_si128(y, mask);
    z = _mm_xor_si128(z, x);
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
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1 - N], r1, r2, mask);
	_mm_store_si128(&sfmt[i], r);
	r1 = r2;
	r2 = r;
    }
}

INLINE static void gen_rand_array(__m128i array[], uint32_t blocks) {
    int i, j;
    __m128i r, r1, r2, mask;
    mask = _mm_set_epi32(MSK4, MSK3, MSK2, MSK1);

    r1 = _mm_load_si128(&sfmt[N - 2]);
    r2 = _mm_load_si128(&sfmt[N - 1]);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&sfmt[i], &sfmt[i + POS1], r1, r2, mask);
	_mm_store_si128(&array[i], r);
	r1 = r2;
	r2 = r;
    }
    for (; i < N; i++) {
	r = mm_recursion(&sfmt[i], &array[i + POS1 - N], r1, r2, mask);
	_mm_store_si128(&array[i], r);
	r1 = r2;
	r2 = r;
    }
    /* main loop */
    for (; i < N * (blocks - 1); i++) {
	r = mm_recursion(&array[i - N], &array[i + POS1 - N], r1, r2, mask);
	_mm_store_si128(&array[i], r);
	r1 = r2;
	r2 = r;
    }
    for (j = 0; i < N * blocks; i++, j++) {
	r = mm_recursion(&array[i - N], &array[i + POS1 - N], r1, r2, mask);
	_mm_store_si128(&array[i], r);
	_mm_store_si128(&sfmt[j], r);
	r1 = r2;
	r2 = r;
    }
}

INLINE uint32_t gen_rand(void)
{
    uint32_t r;

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
	//memcpy(array, sfmt, sizeof(sfmt));
	gen_rand_array((__m128i *)array, block_num);
	//memcpy(sfmt, &array[N * (block_num-1)], sizeof(sfmt));
    }
}

INLINE void init_gen_rand(uint32_t seed)
{
    int i;

    sfmtp[0] = seed;
    for (i = 1; i < N * 4; i++) {
	sfmtp[i] = 1812433253UL * (sfmtp[i - 1] ^ (sfmtp[i - 1] >> 30)) + i;
    }
    idx = N * 4;
}

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif
