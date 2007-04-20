#include <stdio.h>
#include <string.h>
#include <ppc_intrinsics.h>
#include <assert.h>
//#include "mt19937ar.h"
#include "random.h"
#include "speed.h"
/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfU	/* constant vector a */
#define UPPER_MASK 0x80000000UL	/* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL	/* least significant r bits */

/* the array for the state vector  */
static uint32_t mt[N + 4] __attribute__ ((aligned (16)));
static int idx = N + 1;		/* idx==N+1 means mt[N] is not initialized */
static uint64_t *mt64 = (uint64_t *)mt;

union W64_T {
    double d;
    uint64_t u;
    uint32_t a[2];
};
typedef union W64_T w64_t;

union W128_T {
    vector unsigned int v;
    double d[2];
    uint64_t u[2];
};
typedef union W128_T w128_t;

INLINE uint32_t gen_rand_int32(void);

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
INLINE static uint32_t temper(uint32_t x) ALWAYSINLINE;

INLINE static void convert_12(w128_t array[], int size) {
    int i;
    vector unsigned int low_mask = (vector unsigned int)
	(0x000fffff, 0xffffffff, 0x000fffff, 0xffffffff);
    vector unsigned int high_const = (vector unsigned int)
	(0x3ff00000, 0, 0x3ff00000, 0);
    vector unsigned int r;

    for (i = 0; i < size; i++) {
	r = vec_and(array[i].v, low_mask);
	array[i].v = vec_or(r, high_const);
    }
}

INLINE static void convert_co(w128_t array[], int size) {
    int i;
    vector unsigned int low_mask = (vector unsigned int)
	(0x000fffff, 0xffffffff, 0x000fffff, 0xffffffff);
    vector unsigned int high_const = (vector unsigned int)
	(0x3ff00000, 0, 0x3ff00000, 0);
    vector unsigned int r;

    for (i = 0; i < size; i++) {
	r = vec_and(array[i].v, low_mask);
	array[i].v = vec_or(r, high_const);
	array[i].d[0] -= 1.0;
	array[i].d[1] -= 1.0;
    }
}
INLINE static void convert_oc(w128_t array[], int size) {
    int i;
    vector unsigned int low_mask = (vector unsigned int)
	(0x000fffff, 0xffffffff, 0x000fffff, 0xffffffff);
    vector unsigned int high_const = (vector unsigned int)
	(0x3ff00000, 0, 0x3ff00000, 0);
    vector unsigned int r;

    for (i = 0; i < size; i++) {
	r = vec_and(array[i].v, low_mask);
	array[i].v = vec_or(r, high_const);
	array[i].d[0] = 2.0 - array[i].d[0];
	array[i].d[1] = 2.0 - array[i].d[1];
    }
}

INLINE static void convert_oo(w128_t array[], int size) {
    int i;
    vector unsigned int low_mask = (vector unsigned int)
	(0x000fffff, 0xffffffff, 0x000fffff, 0xffffffff);
    vector unsigned int high_const_one = (vector unsigned int)
	(0x3ff00000, 1, 0x3ff00000, 1);
    vector unsigned int r;

    for (i = 0; i < size; i++) {
	r = vec_and(array[i].v, low_mask);
	array[i].v = vec_or(r, high_const_one);
	array[i].d[0] -= 1.0;
	array[i].d[1] -= 1.0;
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
	fprintf(fp, "%08x ", mt[i]);
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

INLINE __attribute__((always_inline)) 
    uint32_t temper(uint32_t y) {
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
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

    idx = 0;
}

INLINE static __attribute__((always_inline)) 
    vector unsigned int vec_temper(vector unsigned int *a,
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
	r = vec_temper((vector unsigned int *)&array[i - N],
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
	r = vec_temper((vector unsigned int *)&array[i - N],
		   s11, s7, s15, s18, and1, and2);
	vec_st(r, 0, &array[i - N]);
    }
}

/* generates a random number on [0,0xffffffff]-interval */
INLINE __attribute__((always_inline)) 
    uint32_t gen_rand_int32(void)
{
    unsigned long y;

    if (idx >= N) {		/* generate N words at one time */
	gen_rand_all();
    }

    y = mt[idx++];

    return temper(y);
}

#define LOW_MASK  0x000FFFFFFFFFFFFFULL
#define HIGH_CONST 0x3FF0000000000000ULL
#define LOW_MASK32_1 0x000fffffU
#define LOW_MASK32_2 0xffffffffU
#define HIGH_CONST32 0x3ff00000U

/* generates a random number on [0,1]-real-interval */
INLINE  __attribute__((always_inline))
    double gen_rand(void)
{
    w64_t r;

    if (idx >= N / 2) {
	gen_rand_all();
	idx = 0;
    }
    r.u = mt64[idx++];
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
    r.u = mt64[idx++];
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
    r.u = mt64[idx++];
    r.a[0] = temper(r.a[0]);
    r.a[1] = temper(r.a[1]);
    r.u = (r.u & LOW_MASK) | (HIGH_CONST | 1);
    return r.d - 1.0;
}

INLINE void fill_array_close1_open2(double array[], int size)
{
    assert(size >= N * 2);
    assert(size % 2 == 0);

    gen_rand_array((uint32_t *)array, size * 2);
    convert_12((w128_t *)array, size / 2);
}

INLINE void fill_array_open_close(double array[], int size)
{
    assert(size >= N * 2);
    assert(size % 2 == 0);

    gen_rand_array((uint32_t *)array, size * 2);
    convert_oc((w128_t *)array, size / 2);
}

INLINE void fill_array_close_open(double array[], int size)
{
    assert(size >= N * 2);
    assert(size % 2 == 0);

    gen_rand_array((uint32_t *)array, size * 2);
    convert_co((w128_t *)array, size / 2);
}

INLINE void fill_array_open_open(double array[], int size)
{
    assert(size >= N * 2);
    assert(size % 2 == 0);

    gen_rand_array((uint32_t *)array, size * 2);
    convert_oo((w128_t *)array, size / 2);
}


#include "test_time3.c"

