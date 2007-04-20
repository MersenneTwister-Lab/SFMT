#include <stdio.h>
//#include "mt19937ar.h"
#include <string.h>
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

static uint32_t mt[N];	/* the array for the state vector  */
static int idx = N + 1;		/* idx==N+1 means mt[N] is not initialized */
static uint64_t *mt64 = (uint64_t *)mt;

union W128_T {
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

INLINE static void gen_rand_array(uint32_t array[], int size);
INLINE static void gen_rand_all(void);

#if defined(__GNUC__) && (!defined(DEBUG))
#define ALWAYSINLINE __attribute__((always_inline))
#else
#define ALWAYSINLINE
#endif
INLINE static void convert_12(w128_t array[], int size) ALWAYSINLINE;
INLINE static void convert_co(w128_t array[], int size) ALWAYSINLINE;
INLINE static void convert_oc(w128_t array[], int size) ALWAYSINLINE;
INLINE static void convert_oo(w128_t array[], int size) ALWAYSINLINE;

#if defined(__BIG_ENDIAN__)
INLINE static void convert_12(w128_t array[], int size) {
    uint32_t r;
    int i;
    for (i = 0; i < size; i++) {
	r = (array[i].a[1] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[1] = array[i].a[0];
	array[i].a[0] = r;
	r = (array[i].a[3] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[3] = array[i].a[2];
	array[i].a[2] = r;
    }
}
#else
INLINE static void convert_12(w128_t array[], int size) {
    int i;
    for (i = 0; i < size; i++) {
	array[i].u[0] = (array[i].u[0] & LOW_MASK) | HIGH_CONST;
	array[i].u[1] = (array[i].u[1] & LOW_MASK) | HIGH_CONST;
    }
}
#endif

#if defined(__BIG_ENDIAN__)
INLINE static void convert_co(w128_t array[], int size) {
    uint32_t r;
    int i;

    for (i = 0; i < size; i++) {
	r = (array[i].a[1] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[1] = array[i].a[0];
	array[i].a[0] = r;
	r = (array[i].a[3] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[3] = array[i].a[2];
	array[i].a[2] = r;
	array[i].d[0] -= 1.0;
	array[i].d[1] -= 1.0;
    }
}
#else
INLINE static void convert_co(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].u[0] = (array[i].u[0] & LOW_MASK) | HIGH_CONST;
	array[i].u[1] = (array[i].u[1] & LOW_MASK) | HIGH_CONST;
	array[i].d[0] -= 1.0;
	array[i].d[1] -= 1.0;
    }
}
#endif

#if defined(__BIG_ENDIAN__)
INLINE static void convert_oc(w128_t array[], int size) {
    uint32_t r;
    int i;

    for (i = 0; i < size; i++) {
	r = (array[i].a[1] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[1] = array[i].a[0];
	array[i].a[0] = r;
	r = (array[i].a[3] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[3] = array[i].a[2];
	array[i].a[2] = r;
	array[i].d[0] = 2.0 - array[i].d[0];
	array[i].d[1] = 2.0 - array[i].d[1];
    }
}
#else
INLINE static void convert_oc(w128_t array[], int size) {
    uint32_t r;
    int i;

    for (i = 0; i < size; i++) {
	r = (array[i].a[1] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[1] = array[i].a[0];
	array[i].a[0] = r;
	r = (array[i].a[3] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[3] = array[i].a[2];
	array[i].a[2] = r;
	array[i].d[0] = 2.0 - array[i].d[0];
	array[i].d[1] = 2.0 - array[i].d[1];
    }
}
#endif

#if defined(__BIG_ENDIAN__)
INLINE static void convert_oo(w128_t array[], int size) {
    uint32_t r;
    int i;
    for (i = 0; i < size; i++) {
	r = (array[i].a[1] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[1] = array[i].a[0] | 1;
	array[i].a[0] = r;
	r = (array[i].a[3] & LOW_MASK32_1) | HIGH_CONST32;
	array[i].a[3] = array[i].a[2] | 1;
	array[i].a[2] = r;
	array[i].d[0] -= 1.0;
	array[i].d[1] -= 1.0;
    }
}
#else
INLINE static void convert_oo(w128_t array[], int size) {
    int i;
    for (i = 0; i < size; i++) {
	array[i].u[0] = (array[i].u[0] & LOW_MASK) | HIGH_CONST | 1;
	array[i].u[1] = (array[i].u[1] & LOW_MASK) | HIGH_CONST | 1;
	array[i].d[0] -= 1.0;
	array[i].d[1] -= 1.0;
    }
}
#endif

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
}

INLINE static void gen_rand_all(void)
{
    unsigned long y;
    static uint32_t mag01[2] = { 0x0UL, MATRIX_A };
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    int kk;

    for (kk = 0; kk < N - M; kk++) {
	y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
	mt[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    for (; kk < N - 1; kk++) {
	y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
	mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
    mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];
}

INLINE static uint32_t temper(uint32_t y) {
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	y ^= (y >> 18);
	return y;
}

INLINE static void gen_rand_array(uint32_t array[], int size)
{
    unsigned long y;
    static unsigned long mag01[2] = { 0x0UL, MATRIX_A };
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    int kk, j;

    for (kk = 0; kk < N - M; kk++) {
	y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
	array[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    for (; kk < N - 1; kk++) {
	y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
	array[kk] = array[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    y = (mt[N - 1] & UPPER_MASK) | (array[0] & LOWER_MASK);
    array[N - 1] = array[M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];
    kk++;
    for (; kk < size - N; kk++) {
	y = (array[kk - N] & UPPER_MASK) | (array[kk + 1 - N] & LOWER_MASK);
	array[kk] = array[kk + M - N] ^ (y >> 1) ^ mag01[y & 0x1UL];
	array[kk - N] = temper(array[kk - N]);
    }
    for (j = 0; j < 2 * N - size; j++) {
	mt[j] = array[j + size - N];
    }
    for (; kk < size; kk++, j++) {
	y = (array[kk - N] & UPPER_MASK) | (array[kk + 1 - N] & LOWER_MASK);
	array[kk] = array[kk + M - N] ^ (y >> 1) ^ mag01[y & 0x1UL];
	mt[j] = array[kk];
	array[kk - N] = temper(array[kk - N]);
    }
    for (j = size - N; j < size; j++) {
	array[j] = temper(array[j]);
    }
}

INLINE double gen_rand(void)
{
    w64_t r;

    if (idx >= N / 2) {
	gen_rand_all();
	idx = 0;
    }
    r.u = temper(mt[idx]);
    idx++;
    r.u &= LOW_MASK;
    r.u |= HIGH_CONST;
    return r.d;
}

#if defined(__BIG_ENDIAN__)
INLINE double genrand_close1_open2(void)
{
    w64_t r;
    uint32_t t;

    if (idx >= N / 2) {
	gen_rand_all();
	idx = 0;
    }
    r.u = mt64[idx++];
    r.a[0] = temper(r.a[0]);
    r.a[1] = temper(r.a[1]);
    t = (r.a[1] & LOW_MASK32_1) | HIGH_CONST32;
    r.a[1] = r.a[0];
    r.a[0] = t;
    t = (r.a[3] & LOW_MASK32_1) | HIGH_CONST32;
    r.a[3] = r.a[2];
    r.a[2] = t;
    return r.d;
}
#else
INLINE double genrand_close1_open2(void)
{
    w64_t r;

    if (idx >= N / 2) {
	gen_rand_all();
	idx = 0;
    }
    r.u = mt64[idx++];
    r.a[0] = temper(r.a[0]);
    r.a[1] = temper(r.a[1]);
    r.u &= LOW_MASK;
    r.u |= HIGH_CONST;
    return r.d;
}
#endif

#if defined(__BIG_ENDIAN__)
INLINE double genrand_open_open(void)
{
    w64_t r;
    uint32_t t;

    if (idx >= N / 2) {
	gen_rand_all();
	idx = 0;
    }
    r.u = mt64[idx++];
    r.a[0] = temper(r.a[0]);
    r.a[1] = temper(r.a[1]);
    t = (r.a[1] & LOW_MASK32_1) | HIGH_CONST32;
    r.a[1] = r.a[0] | 1;
    r.a[0] = t;
    t = (r.a[3] & LOW_MASK32_1) | HIGH_CONST32;
    r.a[3] = r.a[2] | 1;
    r.a[2] = t;
    return r.d - 1.0;
}
#else
INLINE double genrand_open_open(void)
{
    w64_t r;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r.u = mt64[idx++];
    r.a[0] = temper(r.a[0]);
    r.a[1] = temper(r.a[1]);
    r.u &= LOW_MASK;
    r.u |= (HIGH_CONST | 1);
    return r.d - 1.0;
}
#endif

INLINE void fill_array_close1_open2(double array[], int size)
{
    assert(size >= N / 2);
    assert(size % 2 == 0);

    gen_rand_array((uint32_t *)array, size * 2);
    convert_12((w128_t *)array, size / 2);
}

INLINE void fill_array_open_close(double array[], int size)
{
    assert(size >= N / 2);
    assert(size % 2 == 0);

    gen_rand_array((uint32_t *)array, size * 2);
    convert_oc((w128_t *)array, size / 2);
}

INLINE void fill_array_close_open(double array[], int size)
{
    assert(size >= N / 2);
    assert(size % 2 == 0);

    gen_rand_array((uint32_t *)array, size * 2);
    convert_co((w128_t *)array, size / 2);
}

INLINE void fill_array_open_open(double array[], int size)
{
    assert(size >= N / 2);
    assert(size % 2 == 0);

    gen_rand_array((uint32_t *)array, size * 2);
    convert_oo((w128_t *)array, size / 2);
}

#include "test_time3.c"

