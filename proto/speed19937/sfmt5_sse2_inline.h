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
static const __m128i sl1 = {SL1, SL2, SL3, SL4};
static const __m128i sl2 = {SL5, SL6, SL7, SL8};
static const __m128i sr = {SR1, SR2, SR3, SR4};

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
    __m128i a, b, c, r;

    a = _mm_load_si128(&sfmt[0]);
    b = _mm_load_si128(&sfmt[POS1]);
    c = _mm_load_si128(&sfmt[N - 1]);
    r = _mm_xor_si128(_mm_xor_si128(
				    _mm_xor_si128(
						  _mm_slli_epi32(a, sl1),
						  a),
				    _mm_xor_si128(
						  _mm_srli_epi32(b, sr),
						  _mm_srli_si128(b, 4))
				    ),
		      _mm_xor_si128(
				    _mm_slli_epi32(c, sl2),
				    _mm_slli_si128(c, 4))
		      );
    _mm_store_si128(&sfmt[0], r);
    for (i = 1; i < N - POS1; i++) {
	a = _mm_load_si128(&sfmt[i]);
	b = _mm_load_si128(&sfmt[i + POS1]);
	c = r;
	r = _mm_xor_si128(_mm_xor_si128(
					_mm_xor_si128(
						      _mm_slli_epi32(a, sl1),
						      a),
					_mm_xor_si128(
						      _mm_srli_epi32(b, sr),
						      _mm_srli_si128(b, 4))
					),
			  _mm_xor_si128(
					_mm_slli_epi32(c, sl2),
					_mm_slli_si128(c, 4))
			  );
	_mm_store_si128(&sfmt[i], r);
    }
    for (; i < N; i++) {
	a = _mm_load_si128(&sfmt[i]);
	b = _mm_load_si128(&sfmt[i + POS1 - N]);
	c = r;
	r = _mm_xor_si128(_mm_xor_si128(
					_mm_xor_si128(
						      _mm_slli_epi32(a, sl1),
						      a),
					_mm_xor_si128(
						      _mm_srli_epi32(b, sr),
						      _mm_srli_si128(b, 4))
					),
			  _mm_xor_si128(
					_mm_slli_epi32(c, sl2),
					_mm_slli_si128(c, 4))
			  );
	_mm_store_si128(&sfmt[i], r);
    }
}

INLINE static void gen_rand_array(__m128i array[], uint32_t blocks) {
    int i;
    __m128i a, b, c, r;

    a = _mm_load_si128(&array[0]);
    b = _mm_load_si128(&array[POS1]);
    c = _mm_load_si128(&array[N - 1]);
    r = _mm_xor_si128(_mm_xor_si128(
				    _mm_xor_si128(
						  _mm_slli_epi32(a, sl1),
						  a),
				    _mm_xor_si128(
						  _mm_srli_epi32(b, sr),
						  _mm_srli_si128(b, 4))
				    ),
		      _mm_xor_si128(
				    _mm_slli_epi32(c, sl2),
				    _mm_slli_si128(c, 4))
		      );
    _mm_store_si128(&array[0], r);
    for (i = 1; i < N - POS1; i++) {
	a = _mm_load_si128(&array[i]);
	b = _mm_load_si128(&array[i + POS1]);
	c = r;
	r = _mm_xor_si128(_mm_xor_si128(
					_mm_xor_si128(
						      _mm_slli_epi32(a, sl1),
						      a),
					_mm_xor_si128(
						      _mm_srli_epi32(b, sr),
						      _mm_srli_si128(b, 4))
					),
			  _mm_xor_si128(
					_mm_slli_epi32(c, sl2),
					_mm_slli_si128(c, 4))
			  );
	_mm_store_si128(&array[i], r);
    }
    for (; i < N; i++) {
	a = _mm_load_si128(&array[i]);
	b = _mm_load_si128(&array[i + POS1 - N]);
	c = r;
	r = _mm_xor_si128(_mm_xor_si128(
					_mm_xor_si128(
						      _mm_slli_epi32(a, sl1),
						      a),
					_mm_xor_si128(
						      _mm_srli_epi32(b, sr),
						      _mm_srli_si128(b, 4))
					),
			  _mm_xor_si128(
					_mm_slli_epi32(c, sl2),
					_mm_slli_si128(c, 4))
			  );
	_mm_store_si128(&array[i], r);
    }
    for (; i < N * blocks; i++) {
	a = _mm_load_si128(&array[i - N]);
	b = _mm_load_si128(&array[i + POS1 - N]);
	c = r;
	r = _mm_xor_si128(_mm_xor_si128(
					_mm_xor_si128(
						      _mm_slli_epi32(a, sl1),
						      a),
					_mm_xor_si128(
						      _mm_srli_epi32(b, sr),
						      _mm_srli_si128(b, 4))
					),
			  _mm_xor_si128(
					_mm_slli_epi32(c, sl2),
					_mm_slli_si128(c, 4))
			  );
	_mm_store_si128(&array[i], r);
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
