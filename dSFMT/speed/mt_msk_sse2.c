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
#define LOW_MASK  0x000FFFFFFFFFFFFFULL
#define HIGH_CONST 0x3FF0000000000000ULL
#define LOW_MASK32_1 0x000fffffU
#define LOW_MASK32_2 0xffffffffU
#define HIGH_CONST32 0x3ff00000U

/* the array for the state vector  */
static __m128i dmy[1];
static uint32_t mt[N + 4];
static int idx = N + 1;		/* idx==N+1 means mt[N] is not initialized */

union W128_T {
    __m128i si;
    __m128d sd;
    double d[2];
    uint64_t u[2];
    uint32_t a[4];
};
typedef union W128_T w128_t;

union W64_T {
    double d;
    uint64_t u;
    uint32_t a[2];
};
typedef union W64_T w64_t;
static w64_t *dmt = (w64_t *)&mt[0];

INLINE static void gen_rand_array(uint32_t array[], int size);
INLINE static void gen_rand_all(void);
void fill_array_open_close(double array[], int size);
void fill_array_close_open(double array[], int size);
void fill_array_open_open(double array[], int size);
void fill_array_close1_open2(double array[], int size);
INLINE double genrand_close1_open2(void);

#if defined(__GNUC__) && (!defined(DEBUG))
#define ALWAYSINLINE __attribute__((always_inline))
#else
#define ALWAYSINLINE
#endif
INLINE static void convert_co(w128_t array[], int size) ALWAYSINLINE;
INLINE static void convert_oc(w128_t array[], int size) ALWAYSINLINE;
INLINE static void convert_oo(w128_t array[], int size) ALWAYSINLINE;
INLINE static uint32_t temper(uint32_t y) ALWAYSINLINE;

static __m128i sse2_low_mask;
static __m128i sse2_high_const;
static __m128i sse2_int_one;
static __m128d sse2_double_two;
static __m128d sse2_double_m_one;

INLINE static uint32_t temper(uint32_t y) {
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	y ^= (y >> 18);
	return y;
}

static void setup_const(void) {
    static int first = true;
    if (!first) {
	return;
    }
    sse2_low_mask = _mm_set_epi32(LOW_MASK32_1, LOW_MASK32_2,
				  LOW_MASK32_1, LOW_MASK32_2);
    sse2_high_const = _mm_set_epi32(HIGH_CONST32, 0, HIGH_CONST32, 0);
    sse2_int_one = _mm_set_epi32(0, 1, 0, 1);
    sse2_double_two = _mm_set_pd(2.0, 2.0);
    sse2_double_m_one = _mm_set_pd(-1.0, -1.0);
    first = false;
}

// tempering
INLINE static void temper_mask(uint32_t array[], int size)
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
	a0 = _mm_and_si128(a0, sse2_low_mask);
	b0 = _mm_and_si128(b0, sse2_low_mask);
	a0 = _mm_or_si128(a0, sse2_high_const);
	b0 = _mm_or_si128(b0, sse2_high_const);
	_mm_store_si128((__m128i *)&array[i], a0);
	_mm_store_si128((__m128i *)&array[i + 4], b0);
    }
}

INLINE static void convert_co(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].sd = _mm_add_pd(array[i].sd, sse2_double_m_one);
    }
}

INLINE static void convert_oc(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].sd = _mm_sub_pd(sse2_double_two, array[i].sd);
    }
}

INLINE static void convert_oo(w128_t array[], int size) {
    int i;
    w128_t r;

    for (i = 0; i < size; i++) {
	r.si = _mm_or_si128(array[i].si, sse2_int_one);
	array[i].sd = _mm_add_pd(r.sd, sse2_double_m_one);
    }
}

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

#if 0
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
#endif
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
    setup_const();
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


INLINE double gen_rand(void)
{
    w64_t r;

    if (idx >= N / 2) {
	gen_rand_all();
	idx = 0;
    }
    r.u = dmt[idx++].u;
    r.a[0] = temper(r.a[0]);
    r.a[1] = temper(r.a[1]);
    r.u = (r.u & LOW_MASK) | HIGH_CONST;
    return r.d;
}


INLINE double genrand_close1_open2(void)
{
    w64_t r;

    if (idx >= N / 2) {
	gen_rand_all();
	idx = 0;
    }
    r.u = dmt[idx++].u;
    r.a[0] = temper(r.a[0]);
    r.a[1] = temper(r.a[1]);
    r.u = (r.u & LOW_MASK) | HIGH_CONST;
    return r.d;
}

INLINE double genrand_open_open(void)
{
    w64_t r;

    if (idx >= N / 2) {
	gen_rand_all();
	idx = 0;
    }
    r.u = dmt[idx++].u;
    r.a[0] = temper(r.a[0]);
    r.a[1] = temper(r.a[1]);
    r.u = (r.u & LOW_MASK) | (HIGH_CONST | 1);
    return r.d - 1.0;
}

INLINE void fill_array_close1_open2(double array[], int size)
{
    assert(size >= N / 2);
    assert(size % 2 == 0);
    assert((int)array % 16 == 0);

    gen_rand_array((uint32_t *)array, size * 2);
    temper_mask((uint32_t *)array, size * 2);
}

INLINE void fill_array_close_open(double array[], int size)
{
    assert(size >= N / 2);
    assert(size % 2 == 0);
    assert((int)array % 16 == 0);

    gen_rand_array((uint32_t *)array, size * 2);
    temper_mask((uint32_t *)array, size * 2);
    convert_co((w128_t *)array, size / 2);
}

INLINE void fill_array_open_close(double array[], int size)
{
    assert(size >= N / 2);
    assert(size % 2 == 0);
    assert((int)array % 16 == 0);

    gen_rand_array((uint32_t *)array, size * 2);
    temper_mask((uint32_t *)array, size * 2);
    convert_oc((w128_t *)array, size / 2);
}

INLINE void fill_array_open_open(double array[], int size)
{
    assert(size >= N / 2);
    assert(size % 2 == 0);
    assert((int)array % 16 == 0);

    gen_rand_array((uint32_t *)array, size * 2);
    temper_mask((uint32_t *)array, size * 2);
    convert_oo((w128_t *)array, size / 2);
}


#include "test_time3.c"

