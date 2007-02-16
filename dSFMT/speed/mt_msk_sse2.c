#include <stdio.h>
#include <string.h>
#include <emmintrin.h>
//#include "mt19937ar.h"
#include <assert.h>
#include "random.h"
#include "speed.h"

/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL	/* constant vector a */
#define UPPER_MASK 0x80000000UL	/* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL	/* least significant r bits */
//#define DST_TOUCH_BLOCK(blk) (32 | (((blk) * 20) << 16) | (2 << 24))
//#define DST_MAX_BLOCK 12

/* the array for the state vector  */
static __m128i dmy[1];
static uint32_t mt[N + 4];
static int idx = N + 1;		/* idx==N+1 means mt[N] is not initialized */

#if 0
    union SIMD_T {
	__m128i sse2;
	uint32_t a[4];
    } simd;
#endif

//#define MAX_BLOCKS (DST_MAX_BLOCK + 2)

INLINE static void gen_rand_array(uint32_t array[], int size);
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
	fprintf(fp, "%08" PRIx32 " ", mt[i]);
	if (i % 8 == 7) {
	    fprintf(fp, "\n");
	}
    }
}

/* initializes mt[N] with a seed */
INLINE void init_gen_rand(uint64_t t)
{
    uint32_t s;
    s = (uint32_t)t;
    mt[0] = s & 0xffffffffUL;
    for (idx = 1; idx < N; idx++) {
	mt[idx] =
	    (1812433253UL * (mt[idx - 1] ^ (mt[idx - 1] >> 30)) + idx);
	/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
	/* In the previous versions, MSBs of the seed affect   */
	/* only MSBs of the array mt[].                        */
	/* 2002/01/09 modified by Makoto Matsumoto             */
	mt[idx] &= 0xffffffffUL;
	/* for >32 bit machines */
    }
}

INLINE static __m128i mm_recursion(uint32_t *x, uint32_t *y, uint32_t *z,
			 __m128i u_mask, __m128i l_mask, 
			 __m128i mat_a, __m128i one) {
    __m128i a0, a1, b, r;
    a0 = _mm_load_si128((__m128i *)x);
    a1 = _mm_loadu_si128((__m128i *)y);
    b = _mm_loadu_si128((__m128i *)z);
    a0 = _mm_and_si128(a0, u_mask);
    a1 = _mm_and_si128(a1, l_mask);
    r = _mm_or_si128(a0, a1);
    a1 = _mm_and_si128(a1, one);
    a1 = _mm_cmpeq_epi32(a1, one);
    a1 = _mm_and_si128(a1, mat_a);
    a0 = _mm_srli_epi32(r, 1);
    r = _mm_xor_si128(b, a0);
    r = _mm_xor_si128(r, a1);
    return r;
}

INLINE static void gen_rand_all(void)
{
    //uint32_t y;
    __m128i r, u_mask, l_mask, mat_a, one;
    //static uint32_t mag01[2] = { 0x0UL, MATRIX_A };
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    int i;
    u_mask = _mm_set_epi32(UPPER_MASK, UPPER_MASK, UPPER_MASK, UPPER_MASK);
    l_mask = _mm_set_epi32(LOWER_MASK, LOWER_MASK, LOWER_MASK, LOWER_MASK);
    mat_a = _mm_set_epi32(MATRIX_A, MATRIX_A, MATRIX_A, MATRIX_A);
    one = _mm_set_epi32(1, 1, 1, 1);

    for (i = 0; i < N - M - 4; i += 4) {
	r = mm_recursion(&mt[i], &mt[i + 1], &mt[i + M], u_mask, l_mask,
			 mat_a, one);
	_mm_store_si128((__m128i *)&mt[i], r);
    }
    mt[N] = mt[0];
    r = mm_recursion(&mt[i], &mt[i + 1], &mt[i + M], u_mask, l_mask,
		     mat_a, one);
    _mm_store_si128((__m128i *)&mt[i], r);
    i += 4;
    for (; i < N; i += 4) {
	r = mm_recursion(&mt[i], &mt[i + 1], &mt[i + M - N], u_mask, l_mask,
			 mat_a, one);
	_mm_store_si128((__m128i *)&mt[i], r);
    }
    idx = 0;

}

INLINE static void gen_rand_array(uint32_t array[], int size)
{
    __m128i r, u_mask, l_mask, mat_a, one;
    int i, j;

    u_mask = _mm_set_epi32(UPPER_MASK, UPPER_MASK, UPPER_MASK, UPPER_MASK);
    l_mask = _mm_set_epi32(LOWER_MASK, LOWER_MASK, LOWER_MASK, LOWER_MASK);
    mat_a = _mm_set_epi32(MATRIX_A, MATRIX_A, MATRIX_A, MATRIX_A);
    one = _mm_set_epi32(1, 1, 1, 1);

    //memcpy(array, mt, sizeof(mt));
    for (i = 0; i < N - M - 4; i += 4) {
	r = mm_recursion(&mt[i], &mt[i + 1], &mt[i + M], u_mask,
			 l_mask, mat_a, one);
	_mm_store_si128((__m128i *)&array[i], r);
    }
    mt[N] = array[0];
    r = mm_recursion(&mt[i], &mt[i + 1], &mt[i + M], u_mask,
		     l_mask, mat_a, one);
    _mm_store_si128((__m128i *)&array[i], r);
    i += 4;
    for (; i < N; i += 4) {
	r = mm_recursion(&mt[i], &mt[i + 1], &array[i + M - N], u_mask,
			 l_mask, mat_a, one);
	_mm_store_si128((__m128i *)&array[i], r);
    }
    for (; i < size - N; i += 4) {
	r = mm_recursion(&array[i - N], &array[i + 1 - N], &array[i + M - N],
			 u_mask, l_mask, mat_a, one);
	_mm_store_si128((__m128i *)&array[i], r);
    }
    for (j = 0; j < 2 * N - size; j += 4) {
	r = _mm_load_si128((__m128i *)&array[j + size - N]);
	_mm_store_si128((__m128i *)&mt[j], r);
    }
    for (; i < size; i += 4, j += 4) {
	r = mm_recursion(&array[i - N], &array[i + 1 - N], &array[i + M - N],
			 u_mask, l_mask, mat_a, one);
	_mm_store_si128((__m128i *)&array[i], r);
	_mm_store_si128((__m128i *)&mt[j], r);
    }
    //memcpy(mt, &array[N * (blocks - 1)], sizeof(mt));
 }

// tempering
INLINE static void tempering(uint32_t array[], int size)
{
    __m128i a0, a1, b0, b1, and1, and2;
    int i;

    and1 = _mm_set_epi32(0x9d2c5680U, 0x9d2c5680U, 
			 0x9d2c5680U, 0x9d2c5680U);
    and2 = _mm_set_epi32(0xefc60000U, 0xefc60000U, 
			 0xefc60000U, 0xefc60000U);

    for (i = 0; i < size; i += 8) {
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

#define LOW_MASK  0x000FFFFFFFFFFFFFULL
#define HIGH_CONST 0xBFF0000000000000ULL
#define LOW_MASK32_1 0x000fffffU
#define LOW_MASK32_2 0xffffffffU
#define HIGH_CONST32 0xbff00000U

INLINE double gen_rand(void)
{
    uint64_t r;
    double *dp;

    if (idx >= N) {
	gen_rand_all();
	idx = 0;
    }
    r = mt[idx] | ((uint64_t)mt[idx+1] << 32);
    idx += 2;
    r &= LOW_MASK;
    r |= HIGH_CONST;
    dp = (double *)&r;
    *dp += 2.0L;
    return *dp;
}

INLINE void fill_array(double array[], int size)
{
    int i;
    __m128i low_mask, high_const;
    __m128d double_two, *dp;
    __m128i r, *ap;

    low_mask = _mm_set_epi32(LOW_MASK32_1, LOW_MASK32_2,
				  LOW_MASK32_1, LOW_MASK32_2);
    high_const = _mm_set_epi32(HIGH_CONST32, 0, HIGH_CONST32, 0);
    double_two = _mm_set_pd(2.0L, 2.0L);
    //assert(size >= N * 4);
    //assert(size % 4 == 0);
    //assert((int)array % 16 == 0);

    gen_rand_array((uint32_t *)array, size * 2);
    ap = (__m128i *)array;
    for (i = 0; i < size / 2; i++) {
	r = _mm_and_si128(ap[i], low_mask);
	ap[i] = _mm_or_si128(r, high_const);
    }
    dp = (__m128d *)array;
    for (i = 0; i < size / 2; i++) {
	dp[i] = _mm_add_pd(dp[i], double_two);
    }
}

#include "test_time.c"

