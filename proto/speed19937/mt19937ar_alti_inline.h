#include <stdio.h>
//#include "mt19937ar.h"
#include "random-inline.h"
/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL	/* constant vector a */
#define UPPER_MASK 0x80000000UL	/* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL	/* least significant r bits */

/* the array for the state vector  */
static unsigned int mt[N + 4] __attribute__ ((aligned (16)));
static int mti = N + 1;		/* mti==N+1 means mt[N] is not initialized */

//#define MAX_BLOCKS 10

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
    uint32_t y;
    vector unsigned int a, b, c, r, v, w;
    vector unsigned int u_mask = (vector unsigned int)(UPPER_MASK);
    vector unsigned int l_mask = (vector unsigned int)(LOWER_MASK);
    vector unsigned int one = (vector unsigned int)(1);
    vector unsigned int zero = (vector unsigned int)(0);
    vector unsigned char perm1 = (vector unsigned char)
	(4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19);
    vector unsigned int mat_a = (vector unsigned int)(MATRIX_A);
    static uint32_t mag01[2] = { 0x0UL, MATRIX_A };
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    int i, kk;

    a0 = vec_ld(0, &mt[0]);
    b0 = vec_ld(0, &mt[M]);
    for (i = 0; i < N - M - 4; i += 4) {
	a1 = vec_ld(0, &mt[i + 4]);
	a = vec_perm(a0, a1, perm1);
	b1 = vec_ld(0, &mt[i + M + 4]);
	b = vec_perm(b0, b1, perm1);
	r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
	r = vec_xor(vec_xor(b, vec_sl(r, one)),
		    vec_sel(mat_a, zero, vec_cmpeq(vec_and(r, one), zero)));
	vec_st(r, 0, &mt[i]);
	a0 = a1;
	b0 = b1;
    }
    a1 = vec_ld(0, &mt[i + 4]);
    a = vec_perm(a0, a1, perm1);
    //b1 = vec_ld(0, &mt[i + M + 4]);
    //b = vec_perm(b0, b1, perm1);
    r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
    r = vec_xor(vec_xor(b, vec_sl(r, one)),
		vec_sel(mat_a, zero, vec_cmpeq(vec_and(r, one), zero)));
    vec_st(r, 0, &mt[i]);
    a0 = a1;
    b0 = b1;
    i += 4;
    for (; kk < N - 1; kk++) {
	y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
	mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
    mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];

    mti = 0;

}

INLINE static void gen_rand_array(uint32_t array[], uint32_t blocks)
{
    unsigned long y;
    static unsigned long mag01[2] = { 0x0UL, MATRIX_A };
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    int kk;

    for (kk = 0; kk < N - M; kk++) {
	y = (array[kk] & UPPER_MASK) | (array[kk + 1] & LOWER_MASK);
	array[kk] = array[kk + M] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    for (; kk < N - 1; kk++) {
	y = (array[kk] & UPPER_MASK) | (array[kk + 1] & LOWER_MASK);
	array[kk] = array[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    y = (array[N - 1] & UPPER_MASK) | (array[0] & LOWER_MASK);
    array[N - 1] = array[M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];
    kk++;
    for (; kk < N * blocks - M; kk++) {
	y = (array[kk - N] & UPPER_MASK) | (array[kk + 1 - N] & LOWER_MASK);
	array[kk] = array[kk + M - N] ^ (y >> 1) ^ mag01[y & 0x1UL];
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
	memcpy(array, mt, sizeof(mt));
	gen_rand_array(array, MAX_BLOCKS);
	memcpy(mt, &array[N * (MAX_BLOCKS - 1)], sizeof(mt));
	for (i = 0; i < N * MAX_BLOCKS; i++) {
	    y = array[i];
	    y ^= (y >> 11);
	    y ^= (y << 7) & 0x9d2c5680UL;
	    y ^= (y << 15) & 0xefc60000UL;
	    y ^= (y >> 18);
	    array[i] = y;
	}
	array += N * MAX_BLOCKS;
	block_num -= MAX_BLOCKS;
    }
#endif
    if (block_num == 0) {
	return;
    } else if (block_num == 1) {
	gen_rand_all();
	memcpy(array, mt, sizeof(mt));
    } else {
	memcpy(array, mt, sizeof(mt));
	gen_rand_array(array, block_num);
	memcpy(mt, &array[N * (block_num-1)], sizeof(mt));
    }
    for (i = 0; i < N * block_num; i++) {
	y = array[i];
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	y ^= (y >> 18);
	array[i] = y;
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
