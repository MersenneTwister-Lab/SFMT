#include <stdio.h>
#include <emmintrin.h>
//#include "mt19937ar.h"
#include "random-inline.h"
/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfU	/* constant vector a */
#define UPPER_MASK 0x80000000UL	/* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL	/* least significant r bits */
#define DST_TOUCH_BLOCK(blk) (32 | (((blk) * 20) << 16) | (2 << 24))
#define DST_MAX_BLOCK 12

/* the array for the state vector  */
__m128i dmy[1];
unsigned int mt[N + 4];
static int mti = N + 1;		/* mti==N+1 means mt[N] is not initialized */

#define MAX_BLOCKS (DST_MAX_BLOCK + 2)

INLINE static void gen_rand_array(uint32_t array[], uint32_t blocks);
INLINE static void gen_rand_all(void);

INLINE unsigned int get_rnd_maxdegree(void)
{
    return 19937;
}

INLINE unsigned int get_rnd_mexp(void)
{
    return 19937;
}

INLINE unsigned int get_onetime_rnds(void) 
{
    return N;
}

INLINE void print_param(FILE *fp) {
    ;
}

INLINE void print_state(FILE *fp) {
    int i;

    for (i = 0; i < N; i++) {
	fprintf(fp, "%08x ", mt[i]);
	if (i % 8 == 7) {
	    fprintf(fp, "\n");
	}
    }
}

/* initializes mt[N] with a seed */
INLINE void init_gen_rand(uint32_t s)
{
    mt[0] = s & 0xffffffffUL;
    for (mti = 1; mti < N; mti++) {
	mt[mti] =
	    (1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
	/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
	/* In the previous versions, MSBs of the seed affect   */
	/* only MSBs of the array mt[].                        */
	/* 2002/01/09 modified by Makoto Matsumoto             */
	mt[mti] &= 0xffffffffUL;
	/* for >32 bit machines */
    }
}

INLINE static void gen_rand_all(void)
{
    //uint32_t y;
    __m128i a0, a1, b, r, u_mask, l_mask, mat_a, one;
    //static uint32_t mag01[2] = { 0x0UL, MATRIX_A };
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    int i;
    u_mask = _mm_set_epi32(UPPER_MASK, UPPER_MASK, UPPER_MASK, UPPER_MASK);
    l_mask = _mm_set_epi32(LOWER_MASK, LOWER_MASK, LOWER_MASK, LOWER_MASK);
    mat_a = _mm_set_epi32(MATRIX_A, MATRIX_A, MATRIX_A, MATRIX_A);
    one = _mm_set_epi32(1, 1, 1, 1);

    for (i = 0; i < N - M - 4; i += 4) {
	a0 = _mm_load_si128((__m128i *)&mt[i]);
	a1 = _mm_loadu_si128((__m128i *)&mt[i + 1]);
	b = _mm_loadu_si128((__m128i *)&mt[i + M]);
	a0 = _mm_and_si128(a0, u_mask);
	a1 = _mm_and_si128(a1, l_mask);
	r = _mm_or_si128(a0, a1);
	a0 = _mm_and_si128(a0, one);
	a0 = _mm_cmpeq_epi32(a0, one);
	a0 = _mm_and_si128(a0, mat_a);
	a1 = _mm_slli_epi32(r, 1);
	r = _mm_xor_si128(b, a1);
	r = _mm_xor_si128(r, a0);
	_mm_store_si128((__m128i *)&mt[i], r);
    }
    a0 = _mm_load_si128((__m128i *)&mt[i]);
    a1 = _mm_loadu_si128((__m128i *)&mt[i + 1]);
    b = _mm_loadu_si128((__m128i *)&mt[i + M]);
    r = _mm_cvtsi32_si128(mt[0]);
    b = _mm_slli_si128(b, 4);
    r = _mm_slli_si128(r, 12);
    b = _mm_srli_si128(b, 4);
    b = _mm_or_si128(b, r);
    a0 = _mm_and_si128(a0, u_mask);
    a1 = _mm_and_si128(a1, l_mask);
    r = _mm_or_si128(a0, a1);
    a0 = _mm_and_si128(a0, one);
    a0 = _mm_cmpeq_epi32(a0, one);
    a0 = _mm_and_si128(a0, mat_a);
    a1 = _mm_slli_epi32(r, 1);
    r = _mm_xor_si128(b, a1);
    r = _mm_xor_si128(r, a0);
    _mm_store_si128((__m128i *)&mt[i], r);
    i += 4;
    for (; i < N - 4; i += 4) {
	a0 = _mm_load_si128((__m128i *)&mt[i]);
	a1 = _mm_loadu_si128((__m128i *)&mt[i + 1]);
	b = _mm_loadu_si128((__m128i *)&mt[i + M - N]);
	a0 = _mm_and_si128(a0, u_mask);
	a1 = _mm_and_si128(a1, l_mask);
	r = _mm_or_si128(a0, a1);
	a0 = _mm_and_si128(a0, one);
	a0 = _mm_cmpeq_epi32(a0, one);
	a0 = _mm_and_si128(a0, mat_a);
	a1 = _mm_slli_epi32(r, 1);
	r = _mm_xor_si128(b, a1);
	r = _mm_xor_si128(r, a0);
	_mm_store_si128((__m128i *)&mt[i], r);
    }
    a0 = _mm_load_si128((__m128i *)&mt[i]);
    a1 = _mm_loadu_si128((__m128i *)&mt[i + 1]);
    r = _mm_cvtsi32_si128(mt[0]);
    a1 = _mm_slli_si128(a1, 4);
    r = _mm_slli_si128(r, 12);
    a1 = _mm_srli_si128(a1, 4);
    a1 = _mm_or_si128(a1, r);
    b = _mm_loadu_si128((__m128i *)&mt[i + M - N]);
    a0 = _mm_and_si128(a0, u_mask);
    a1 = _mm_and_si128(a1, l_mask);
    r = _mm_or_si128(a0, a1);
    a0 = _mm_and_si128(a0, one);
    a0 = _mm_cmpeq_epi32(a0, one);
    a0 = _mm_and_si128(a0, mat_a);
    a1 = _mm_slli_epi32(r, 1);
    r = _mm_xor_si128(b, a1);
    r = _mm_xor_si128(r, a0);
    _mm_store_si128((__m128i *)&mt[i], r);

    mti = 0;

}

INLINE static void gen_rand_array(uint32_t array[], uint32_t blocks)
{
    __m128i a0, a1, b, r, u_mask, l_mask, mat_a, one;
    int i;

    u_mask = _mm_set_epi32(UPPER_MASK, UPPER_MASK, UPPER_MASK, UPPER_MASK);
    l_mask = _mm_set_epi32(LOWER_MASK, LOWER_MASK, LOWER_MASK, LOWER_MASK);
    mat_a = _mm_set_epi32(MATRIX_A, MATRIX_A, MATRIX_A, MATRIX_A);
    one = _mm_set_epi32(1, 1, 1, 1);

    memcpy(array, mt, sizeof(mt));
    for (i = 0; i < N - M - 4; i += 4) {
	a0 = _mm_load_si128((__m128i *)&array[i]);
	a1 = _mm_loadu_si128((__m128i *)&array[i + 1]);
	b = _mm_loadu_si128((__m128i *)&array[i + M]);
	a0 = _mm_and_si128(a0, u_mask);
	a1 = _mm_and_si128(a1, l_mask);
	r = _mm_or_si128(a0, a1);
	a0 = _mm_and_si128(a0, one);
	a0 = _mm_cmpeq_epi32(a0, one);
	a0 = _mm_and_si128(a0, mat_a);
	a1 = _mm_slli_epi32(r, 1);
	r = _mm_xor_si128(b, a1);
	r = _mm_xor_si128(r, a0);
	_mm_store_si128((__m128i *)&array[i], r);
    }
    a0 = _mm_load_si128((__m128i *)&array[i]);
    a1 = _mm_loadu_si128((__m128i *)&array[i + 1]);
    b = _mm_loadu_si128((__m128i *)&array[i + M]);
    r = _mm_cvtsi32_si128(array[0]);
    b = _mm_slli_si128(b, 4);
    r = _mm_slli_si128(r, 12);
    b = _mm_srli_si128(b, 4);
    b = _mm_or_si128(b, r);
    a0 = _mm_and_si128(a0, u_mask);
    a1 = _mm_and_si128(a1, l_mask);
    r = _mm_or_si128(a0, a1);
    a0 = _mm_and_si128(a0, one);
    a0 = _mm_cmpeq_epi32(a0, one);
    a0 = _mm_and_si128(a0, mat_a);
    a1 = _mm_slli_epi32(r, 1);
    r = _mm_xor_si128(b, a1);
    r = _mm_xor_si128(r, a0);
    _mm_store_si128((__m128i *)&array[i], r);
    i += 4;
    for (; i < N - 4; i += 4) {
	a0 = _mm_load_si128((__m128i *)&array[i]);
	a1 = _mm_loadu_si128((__m128i *)&array[i + 1]);
	b = _mm_loadu_si128((__m128i *)&array[i + M - N]);
	a0 = _mm_and_si128(a0, u_mask);
	a1 = _mm_and_si128(a1, l_mask);
	r = _mm_or_si128(a0, a1);
	a0 = _mm_and_si128(a0, one);
	a0 = _mm_cmpeq_epi32(a0, one);
	a0 = _mm_and_si128(a0, mat_a);
	a1 = _mm_slli_epi32(r, 1);
	r = _mm_xor_si128(b, a1);
	r = _mm_xor_si128(r, a0);
	_mm_store_si128((__m128i *)&array[i], r);
    }
    a0 = _mm_load_si128((__m128i *)&mt[i]);
    a1 = _mm_loadu_si128((__m128i *)&mt[i + 1]);
    r = _mm_cvtsi32_si128(array[0]);
    a1 = _mm_slli_si128(a1, 4);
    r = _mm_slli_si128(r, 12);
    a1 = _mm_srli_si128(a1, 4);
    a1 = _mm_or_si128(a1, r);
    b = _mm_loadu_si128((__m128i *)&mt[i + M - N]);
    a0 = _mm_and_si128(a0, u_mask);
    a1 = _mm_and_si128(a1, l_mask);
    r = _mm_or_si128(a0, a1);
    a0 = _mm_and_si128(a0, one);
    a0 = _mm_cmpeq_epi32(a0, one);
    a0 = _mm_and_si128(a0, mat_a);
    a1 = _mm_slli_epi32(r, 1);
    r = _mm_xor_si128(b, a1);
    r = _mm_xor_si128(r, a0);
    _mm_store_si128((__m128i *)&mt[i], r);
    i += 4;
    for (; i < N * blocks; i += 4) {
	a0 = _mm_load_si128((__m128i *)&array[i]);
	a1 = _mm_loadu_si128((__m128i *)&array[i + 1 - N]);
	b = _mm_loadu_si128((__m128i *)&array[i + M - N]);
	a0 = _mm_and_si128(a0, u_mask);
	a1 = _mm_and_si128(a1, l_mask);
	r = _mm_or_si128(a0, a1);
	a0 = _mm_and_si128(a0, one);
	a0 = _mm_cmpeq_epi32(a0, one);
	a0 = _mm_and_si128(a0, mat_a);
	a1 = _mm_slli_epi32(r, 1);
	r = _mm_xor_si128(b, a1);
	r = _mm_xor_si128(r, a0);
	_mm_store_si128((__m128i *)&array[i], r);
    }
    memcpy(mt, &array[N * (blocks - 1)], sizeof(mt));
 }

// tempering
INLINE static void tempering(uint32_t array[], uint32_t blocks)
{
    __m128i a0, a1, b0, b1, and1, and2;
    int i;

    and1 = _mm_set_epi32(0x9d2c5680UL, 0x9d2c5680UL, 
			 0x9d2c5680UL, 0x9d2c5680UL);
    and2 = _mm_set_epi32(0xefc60000UL, 0xefc60000UL, 
			 0xefc60000UL, 0xefc60000UL);

    for (i = 0; i < N * blocks; i += 8) {
	a0 = _mm_load_si128((__m128i *)&array[i]);
	b0 = _mm_load_si128((__m128i *)&array[i + 4]);
	a1 = _mm_srli_epi32(a0, 11);
	b1 = _mm_srli_epi32(b0, 11);
	a0 = _mm_xor_si128(a0, a1);
	b0 = _mm_xor_si128(b0, b1);
	a1 = _mm_slli_epi32(a0, 7);
	b1 = _mm_slli_epi32(b0, 7);
	a1 = _mm_and_si128(a1, and1);
	b1 = _mm_and_si128(b1, and1);
	a0 = _mm_xor_si128(a0, a1);
	b0 = _mm_xor_si128(b0, b1);
	a1 = _mm_slli_epi32(a0, 15);
	b1 = _mm_slli_epi32(b0, 15);
	a1 = _mm_and_si128(a1, and2);
	b1 = _mm_and_si128(b1, and2);
	a0 = _mm_xor_si128(a0, a1);
	b0 = _mm_xor_si128(b0, b1);
	a1 = _mm_srli_epi32(a0, 18);
	b1 = _mm_srli_epi32(b0, 18);
	a0 = _mm_xor_si128(a0, a1);
	b0 = _mm_xor_si128(b0, b1);
	_mm_store_si128((__m128i *)&array[i], a0);
	_mm_store_si128((__m128i *)&array[i + 4], b0);
    }
}

/* generates a random number on [0,0xffffffff]-interval */
INLINE uint32_t gen_rand(void)
{
    unsigned long y;

    if (mti >= N) {		/* generate N words at one time */
	gen_rand_all();
    }

    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

INLINE void fill_array_block(uint32_t array[], uint32_t block_num)
{
    int i;
    uint32_t y;

#if 0
    while (block_num > MAX_BLOCKS) {
	gen_rand_array(array, MAX_BLOCKS);
	tempering(array, MAX_BLOCKS);
	array += N * MAX_BLOCKS;
	block_num -= MAX_BLOCKS;
    }
#endif
    if (block_num == 0) {
	return;
    } else if (block_num == 1) {
	gen_rand_all();
	memcpy(array, mt, sizeof(mt));
	tempering(array, 1);
    } else {
	gen_rand_array(array, block_num);
	tempering(array, block_num);
    }
}

INLINE void fill_array(uint32_t array[], uint32_t size) 
{
    if (size < N - mti) {
	memcpy(array, mt, size * sizeof(uint32_t));
	mti += size;
	return;
    }
    if (mti < N) {
	memcpy(array, mt, (N - mti) * sizeof(uint32_t));
	array += N - mti;
	size -= N - mti;
    }
    while (size >= N) {
	gen_rand_all();
	memcpy(array, mt, sizeof(mt));
	array += N;
	size -= N;
    }
    if (size > 0) {
	gen_rand_all();
	memcpy(array, mt, size * sizeof(uint32_t));
	mti = size;
    } else {
	mti = N;
    }
}
