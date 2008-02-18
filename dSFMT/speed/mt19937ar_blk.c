#include <stdio.h>
//#include "mt19937ar.h"
#include "random-inline.h"
#include <string.h>
#include <assert.h>

/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL	/* constant vector a */
#define UPPER_MASK 0x80000000UL	/* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL	/* least significant r bits */

#if defined(SSE2)
union W128_T {
    __m128i si;
    __m128d sd;
    uint32_t a[4];
    double d[2];
};
#elif defined(ALTIVEC)
union W128_T {
    vector unsigned int v;
    uint32_t a[4];
    double d[2];
};
#else
union W128_T {
    uint32_t a[4];
    double d[2];
};
#endif
typedef union W128_T w128_t;

union W64_T {
    double d;
    uint32_t a[2];
};
typedef union W64_T w64_t;

static uint32_t mt[N];	/* the array for the state vector  */
static int mti = N + 1;		/* mti==N+1 means mt[N] is not initialized */

//#define MAX_BLOCKS 10

INLINE static void gen_rand_arrayco(w128_t array[], int size);
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

INLINE static double temper(uint32_t y, uint32_t z) {
	y ^= (y >> 11);
	z ^= (z >> 11);
	y ^= (y << 7) & 0x9d2c5680UL;
	z ^= (z << 7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	z ^= (z << 15) & 0xefc60000UL;
	y ^= (y >> 18);
	z ^= (z >> 18);
	y = y >> 5;
	z = z >> 6;
	return (y * 67108864.0 + z) * (1.0/9007199254740992.0);
}

INLINE static void gen_rand_arrayco(w128_t array[], int size)
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
    for (; kk < size - N; kk += 2) {
	y = (array[kk - N] & UPPER_MASK) | (array[kk + 1 - N] & LOWER_MASK);
	array[kk] = array[kk + M - N] ^ (y >> 1) ^ mag01[y & 0x1UL];
	y = (array[kk + 1 - N] & UPPER_MASK) | (array[kk + 2 - N] & LOWER_MASK);
	array[kk] = array[kk + 1 + M - N] ^ (y >> 1) ^ mag01[y & 0x1UL];
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

/* generates a random number on [0,0xffffffff]-interval */
INLINE uint32_t gen_rand(void)
{
    unsigned long y;

    if (mti >= N) {		/* generate N words at one time */
	gen_rand_all();
	mti = 0;
    }

    y = mt[mti++];

    /* Tempering */
    //y = temper(y);
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

INLINE void fill_array(double array[], int size)
{

    assert(size >= 2 * N);
    assert(mti == N);

    gen_rand_array(array, size);
}

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif
