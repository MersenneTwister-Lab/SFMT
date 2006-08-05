#include <stdio.h>
#include <string.h>
#include <ppc_intrinsics.h>
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
static unsigned int mt[N + 4] __attribute__ ((aligned (16)));
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
    vector unsigned int a0, a1, a, b0, b1, b, r;
    vector unsigned int u_mask = (vector unsigned int)(UPPER_MASK);
    vector unsigned int l_mask = (vector unsigned int)(LOWER_MASK);
    vector unsigned int one = (vector unsigned int)(1);
    vector unsigned int zero = (vector unsigned int)(0);
    vector unsigned char perm1 = (vector unsigned char)
	(4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19);
    vector unsigned char perm;
    vector unsigned int mat_a = (vector unsigned int)(MATRIX_A);
    //static uint32_t mag01[2] = { 0x0UL, MATRIX_A };
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    int i;

    //__dcbt(&mt[0], 32);
    //__dcbt(&mt[M], 32);
    a0 = vec_ld(0, &mt[0]);
    b0 = vec_ld(0, &mt[M]);
    for (i = 0; i < N - M - 4; i += 4) {
	//__dcbt(&mt[i + 4], 32);
	//__dcbt(&mt[i + M + 4], 32);
	//__dcbtst(&mt[i], 32);
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
    //__dcbt(&mt[i + 4], 32);
    //__dcbtst(&mt[i], 32);
    perm = (vector unsigned char)
	(4, 5, 6, 7, 8, 9, 10, 11, 16, 17, 18, 19, 20, 21, 22, 23);
    a1 = vec_ld(0, &mt[i + 4]);
    a = vec_perm(a0, a1, perm1);
    b1 = vec_ld(0, &mt[0]);
    b = vec_perm(b0, b1, perm);
    r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
    r = vec_xor(vec_xor(b, vec_sl(r, one)),
		vec_sel(mat_a, zero, vec_cmpeq(vec_and(r, one), zero)));
    vec_st(r, 0, &mt[i]);
    perm = (vector unsigned char)
	(8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23);
    a0 = a1;
    b0 = b1;
    i += 4;
    for (; i < N - 4; i += 4) {
	//__dcbt(&mt[i + 4], 32);
	//__dcbtst(&mt[i], 32);
	a1 = vec_ld(0, &mt[i + 4]);
	a = vec_perm(a0, a1, perm1);
	b1 = vec_ld(0, &mt[i + M - N + 4]);
	b = vec_perm(b0, b1, perm);
	r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
	r = vec_xor(vec_xor(b, vec_sl(r, one)),
		    vec_sel(mat_a, zero, vec_cmpeq(vec_and(r, one), zero)));
	vec_st(r, 0, &mt[i]);
	a0 = a1;
	b0 = b1;
    }
    a1 = vec_ld(0, &mt[0]);
    a = vec_perm(a0, a1, perm1);
    b1 = vec_ld(0, &mt[i + M - N + 4]);
    b = vec_perm(b0, b1, perm);
    r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
    r = vec_xor(vec_xor(b, vec_sl(r, one)),
		vec_sel(mat_a, zero, vec_cmpeq(vec_and(r, one), zero)));
    vec_st(r, 0, &mt[i]);

    mti = 0;

}

INLINE static void gen_rand_array(uint32_t array[], uint32_t blocks)
{
    //unsigned long y;
    vector unsigned int a0, a1, a, b0, b1, b, r;
    vector unsigned int u_mask = (vector unsigned int)(UPPER_MASK);
    vector unsigned int l_mask = (vector unsigned int)(LOWER_MASK);
    vector unsigned int one = (vector unsigned int)(1);
    vector unsigned int zero = (vector unsigned int)(0);
    vector unsigned char perm1 = (vector unsigned char)
	(4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19);
    vector unsigned char perm;
    vector unsigned int mat_a = (vector unsigned int)(MATRIX_A);
    //static unsigned long mag01[2] = { 0x0UL, MATRIX_A };
    vector unsigned int s11 = (vector unsigned int)(11);
    vector unsigned int s7 = (vector unsigned int)(7);
    vector unsigned int s15 = (vector unsigned int)(15);
    vector unsigned int s18 = (vector unsigned int)(18);
    vector unsigned int and1 = (vector unsigned int)(0x9d2c5680U);
    vector unsigned int and2 = (vector unsigned int)(0xefc60000U);

    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    int i, j;

    //vec_dst(mt, DST_TOUCH_BLOCK(1), 0);
    memcpy(array, mt, sizeof(mt));
    //__dcbt(&array[0], 32);
    //__dcbt(&array[M], 32);
    a0 = vec_ld(0, &array[0]);
    b0 = vec_ld(0, &array[M]);
    for (i = 0; i < N - M - 4; i += 4) {
	//__dcbt(&array[i + 4], 32);
	//__dcbt(&array[i + M + 4], 32);
	a1 = vec_ld(0, &array[i + 4]);
	//__dcbtst(&array[i], 32);
	a = vec_perm(a0, a1, perm1);
	b1 = vec_ld(0, &array[i + M + 4]);
	b = vec_perm(b0, b1, perm1);
	r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
	r = vec_xor(vec_xor(b, vec_sl(r, one)),
		    vec_sel(mat_a, zero, vec_cmpeq(vec_and(r, one), zero)));
	vec_st(r, 0, &array[i]);
	a0 = a1;
	b0 = b1;
    }
    //__dcbt(&array[i + 4], 32);
    perm = (vector unsigned char)
	(4, 5, 6, 7, 8, 9, 10, 11, 16, 17, 18, 19, 20, 21, 22, 23);
    a1 = vec_ld(0, &array[i + 4]);
    //__dcbtst(&array[i], 32);
    a = vec_perm(a0, a1, perm1);
    b1 = vec_ld(0, &array[0]);
    b = vec_perm(b0, b1, perm);
    r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
    r = vec_xor(vec_xor(b, vec_sl(r, one)),
		vec_sel(mat_a, zero, vec_cmpeq(vec_and(r, one), zero)));
    vec_st(r, 0, &array[i]);
    perm = (vector unsigned char)
	(8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23);
    a0 = a1;
    b0 = b1;
    i += 4;
    for (; i < N - 4; i += 4) {
	//__dcbt(&array[i + 4], 32);
	a1 = vec_ld(0, &array[i + 4]);
	//__dcbtst(&array[i], 32);
	a = vec_perm(a0, a1, perm1);
	b1 = vec_ld(0, &array[i + M - N + 4]);
	b = vec_perm(b0, b1, perm);
	r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
	r = vec_xor(vec_xor(b, vec_sl(r, one)),
		    vec_sel(mat_a, zero, vec_cmpeq(vec_and(r, one), zero)));
	vec_st(r, 0, &array[i]);
	a0 = a1;
	b0 = b1;
    }
    //__dcbtst(&array[i], 32);
    a1 = vec_ld(0, &array[0]);
    a = vec_perm(a0, a1, perm1);
    b1 = vec_ld(0, &array[i + M - N + 4]);
    b = vec_perm(b0, b1, perm);
    r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
    r = vec_xor(vec_xor(b, vec_sl(r, one)),
		vec_sel(mat_a, zero, vec_cmpeq(vec_and(r, one), zero)));
    vec_st(r, 0, &array[i]);
    a0 = a1;
    b0 = b1;
    i += 4;
    for (; i < N * blocks; i += 4) {
	__dcbtst(&array[i], 64);
	//__dcbtst(&array[i - N], 32);
	a1 = vec_ld(0, &array[i - N + 4]);
	a = vec_perm(a0, a1, perm1);
	// tempering
	a0 = a;
	a0 = vec_xor(a0, vec_sr(a0, s11));
	a0 = vec_xor(a0, vec_and(vec_sl(a0, s7), and1));
	a0 = vec_xor(a0, vec_and(vec_sl(a0, s15), and2));
	a0 = vec_xor(a0, vec_sr(a0, s18));
	vec_st(a0, 0, &array[i - N]);
	//__dcbf(&array[i - N], 0);
	// end of tempering
	b1 = vec_ld(0, &array[i + M - N + 4]);
	b = vec_perm(b0, b1, perm);
	r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
	r = vec_xor(vec_xor(b, vec_sl(r, one)),
		    vec_sel(mat_a, zero, vec_cmpeq(vec_and(r, one), zero)));
	vec_st(r, 0, &array[i]);
	a0 = a1;
	b0 = b1;
    }
    memcpy(mt, &array[N * (blocks - 1)], sizeof(mt));
    for (j = (N - 1) * blocks; j < N * blocks; j += 4) {
	// tempering
	r = vec_ld(0, &array[j]);
	r = vec_xor(r, vec_sr(r, s11));
	r = vec_xor(r, vec_and(vec_sl(r, s7), and1));
	r = vec_xor(r, vec_and(vec_sl(r, s15), and2));
	r = vec_xor(r, vec_sr(r, s18));
	vec_st(r, 0, &array[j]);
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

#if 1
    while (block_num > MAX_BLOCKS) {
	gen_rand_array(array, MAX_BLOCKS);
	array += N * MAX_BLOCKS;
	block_num -= MAX_BLOCKS;
    }
#endif
    if (block_num == 0) {
	return;
    } else if (block_num == 1) {
	gen_rand_all();
	memcpy(array, mt, sizeof(mt));
	for (i = 0; i < N * block_num; i++) {
	    y = array[i];
	    y ^= (y >> 11);
	    y ^= (y << 7) & 0x9d2c5680UL;
	    y ^= (y << 15) & 0xefc60000UL;
	    y ^= (y >> 18);
	    array[i] = y;
	}
    } else {
	gen_rand_array(array, block_num);
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

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif
