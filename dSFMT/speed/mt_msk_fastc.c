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

static uint32_t mt[N];	/* the array for the state vector  */
static int idx = N + 1;		/* idx==N+1 means mt[N] is not initialized */

//#define MAX_BLOCKS 10

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
#ifdef BIG_ENDIAN
    uint64_t r;
#endif
    uint32_t *ap;

    gen_rand_array((uint32_t *)array, size * 2);
    ap = (uint32_t *)array;
#ifdef BIG_ENDIAN
    for (i = 0; i < size * 2; i += 2) {
	r = (ap[i+1] & LOW_MASK32_1) | HIGH_CONST32;
	ap[i+1] = ap[i];
	ap[i] = r;
    }
#else
    for (i = 1; i < size * 2; i += 2) {
	ap[i] = (ap[i] & LOW_MASK32_1) | HIGH_CONST32;
    }
#endif
    for (i = 0; i < size; i++) {
	array[i] += 2.0L;
    }
}

#include "test_time.c"

