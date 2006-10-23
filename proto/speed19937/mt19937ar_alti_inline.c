#include <stdio.h>
#include <string.h>
#include <ppc_intrinsics.h>
#include <assert.h>
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

INLINE static __attribute__((always_inline))
    vector unsigned int vec_recursion(vector unsigned int a,
				      vector unsigned int b,
				      vector unsigned int one,
				      vector unsigned int zero,
				      vector unsigned int mat_a) {
    vector unsigned int r, m;
    
    m = vec_and(a, one);
    r = vec_sr(a, one);
    m = vec_cmpeq(m, zero);
    r = vec_xor(b, r);
    m = vec_sel(mat_a, zero, m);
    r = vec_xor(r, m);
		
    return r;
}

INLINE static __attribute__((always_inline)) 
    void gen_rand_all(void)
{
    //uint32_t y;
    vector unsigned int a0, a1, a, b0, b1, b, r;
    vector unsigned int u_mask = (vector unsigned int)(UPPER_MASK);
    vector unsigned int l_mask = (vector unsigned int)(LOWER_MASK);
    vector unsigned int one = (vector unsigned int)(1);
    vector unsigned int zero = (vector unsigned int)(0);
    vector unsigned char perm1 = (vector unsigned char)
	(4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19);
    vector unsigned int mat_a = (vector unsigned int)(MATRIX_A);
    //static uint32_t mag01[2] = { 0x0UL, MATRIX_A };
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    int i;

    a0 = vec_ld(0, &mt[0]);
    b0 = vec_ld(0, &mt[M]);
    for (i = 0; i < N - M - 4; i += 4) {
	a1 = vec_ld(0, &mt[i + 4]);
	a = vec_perm(a0, a1, perm1);
	b1 = vec_ld(0, &mt[i + M + 4]);
	b = vec_perm(b0, b1, perm1);
	r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
	r = vec_recursion(r, b, one, zero, mat_a);
	vec_st(r, 0, &mt[i]);
	a0 = a1;
	b0 = b1;
    }
    a1 = vec_ld(0, &mt[i + 4]);
    a = vec_perm(a0, a1, perm1);
    b1 = vec_ld(0, &mt[0]);
    b = vec_perm(b0, b1, perm1);
    r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
    r = vec_recursion(r, b, one, zero, mat_a);
    vec_st(r, 0, &mt[i]);
    a0 = a1;
    b0 = b1;
    i += 4;
    for (; i < N - 4; i += 4) {
	a1 = vec_ld(0, &mt[i + 4]);
	a = vec_perm(a0, a1, perm1);
	b1 = vec_ld(0, &mt[i + M - N + 4]);
	b = vec_perm(b0, b1, perm1);
	r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
	r = vec_recursion(r, b, one, zero, mat_a);
	vec_st(r, 0, &mt[i]);
	a0 = a1;
	b0 = b1;
    }
    a1 = vec_ld(0, &mt[0]);
    a = vec_perm(a0, a1, perm1);
    b1 = vec_ld(0, &mt[i + M - N + 4]);
    b = vec_perm(b0, b1, perm1);
    r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
    r = vec_recursion(r, b, one, zero, mat_a);
    vec_st(r, 0, &mt[i]);

    mti = 0;
}

INLINE static __attribute__((always_inline)) 
    vector unsigned int temper(vector unsigned int *a,
			       vector unsigned int s11,
			       vector unsigned int s7,
			       vector unsigned int s15,
			       vector unsigned int s18,
			       vector unsigned int and1,
			       vector unsigned int and2) {
    vector unsigned r;

    r = vec_ld(0, a);
    r = vec_xor(r, vec_sr(r, s11));
    r = vec_xor(r, vec_and(vec_sl(r, s7), and1));
    r = vec_xor(r, vec_and(vec_sl(r, s15), and2));
    r = vec_xor(r, vec_sr(r, s18));
    return r;
}

INLINE static void gen_rand_array(uint32_t array[], int size)
{
    //unsigned long y;
    vector unsigned int a0, a1, a, b0, b1, b, r;
    vector unsigned int u_mask = (vector unsigned int)(UPPER_MASK);
    vector unsigned int l_mask = (vector unsigned int)(LOWER_MASK);
    vector unsigned int one = (vector unsigned int)(1);
    vector unsigned int zero = (vector unsigned int)(0);
    vector unsigned char perm1 = (vector unsigned char)
	(4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19);
    vector unsigned int mat_a = (vector unsigned int)(MATRIX_A);
    //static unsigned long mag01[2] = { 0x0UL, MATRIX_A };
    vector unsigned int s11 = (vector unsigned int)(11);
    vector unsigned int s7 = (vector unsigned int)(7);
    vector unsigned int s15 = (vector unsigned int)(15);
    vector unsigned int s18 = (vector unsigned int)(18);
    vector unsigned int and1 = (vector unsigned int)(0x9d2c5680U);
    vector unsigned int and2 = (vector unsigned int)(0xefc60000U);

    int i;

    //vec_dst(mt, DST_TOUCH_BLOCK(1), 0);
    memcpy(array, mt, sizeof(mt));
    a0 = vec_ld(0, &array[0]);
    b0 = vec_ld(0, &array[M]);
    for (i = 0; i < N - M - 4; i += 4) {
	a1 = vec_ld(0, &array[i + 4]);
	a = vec_perm(a0, a1, perm1);
	b1 = vec_ld(0, &array[i + M + 4]);
	b = vec_perm(b0, b1, perm1);
	r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
	r = vec_recursion(r, b, one, zero, mat_a);
	vec_st(r, 0, &array[i]);
	a0 = a1;
	b0 = b1;
    }
    a1 = vec_ld(0, &array[i + 4]);
    a = vec_perm(a0, a1, perm1);
    b1 = vec_ld(0, &array[0]);
    b = vec_perm(b0, b1, perm1);
    r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
    r = vec_recursion(r, b, one, zero, mat_a);
    vec_st(r, 0, &array[i]);
    a0 = a1;
    b0 = b1;
    i += 4;
    for (; i < N - 4; i += 4) {
	a1 = vec_ld(0, &array[i + 4]);
	a = vec_perm(a0, a1, perm1);
	b1 = vec_ld(0, &array[i + M - N + 4]);
	b = vec_perm(b0, b1, perm1);
	r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
	r = vec_recursion(r, b, one, zero, mat_a);
	vec_st(r, 0, &array[i]);
	a0 = a1;
	b0 = b1;
    }
    a1 = vec_ld(0, &array[0]);
    a = vec_perm(a0, a1, perm1);
    b1 = vec_ld(0, &array[i + M - N + 4]);
    b = vec_perm(b0, b1, perm1);
    r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
    r = vec_recursion(r, b, one, zero, mat_a);
    vec_st(r, 0, &array[i]);
    a0 = a1;
    b0 = b1;
    i += 4;
    for (; i < size; i += 4) {
	// tempering
	r = temper((vector unsigned int *)&array[i - N],
		   s11, s7, s15, s18, and1, and2);
	vec_st(r, 0, &array[i - N]);
	// end of tempering
	//__dcbtst(&array[i], 64);
	a1 = vec_ld(0, &array[i - N + 4]);
	a = vec_perm(a0, a1, perm1);
	b1 = vec_ld(0, &array[i + M - N + 4]);
	b = vec_perm(b0, b1, perm1);
	r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
	r = vec_recursion(r, b, one, zero, mat_a);
	vec_st(r, 0, &array[i]);
	a0 = a1;
	b0 = b1;
    }
    memcpy(mt, &array[size - N], sizeof(mt));
    for (; i < size + N; i += 4) {
	r = temper((vector unsigned int *)&array[i - N],
		   s11, s7, s15, s18, and1, and2);
	vec_st(r, 0, &array[i - N]);
    }
}

/* generates a random number on [0,0xffffffff]-interval */
INLINE __attribute__((always_inline)) 
    uint32_t gen_rand(void)
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

INLINE __attribute__((always_inline)) 
    void fill_array(uint32_t array[], int size)
{
    assert(size >= 2 * N);
    assert((int)array % 16 == 0);

    gen_rand_array(array, size);
}

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif
