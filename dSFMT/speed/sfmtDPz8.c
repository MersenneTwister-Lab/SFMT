/* Simple and Fast Twister */

#include <string.h>
#if defined(__GNUC__)
  #include <stdint.h>
#endif
#include <stdio.h>
#include <assert.h>
#include "random.h"
#include "paramsDPz2.h"

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 104
#define N ((MEXP - 128) / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N + 128)

#ifdef SSE2
union W128_T {
    __m128i si;
    __m128d sd;
    uint64_t a[2];
    double d[2];
};
#else
union W128_T {
    uint64_t a[2];
    double d[2];
};
#endif
typedef union W128_T w128_t;

union W64_T {
    double d;
    uint64_t u;
};
typedef union W64_T w64_t;

static w128_t sfmt[N + 1];
static int idx;

void fill_array_open_close(double array[], int size);
void fill_array_close_open(double array[], int size);
void fill_array_open_open(double array[], int size);
void fill_array_close1_open2(double array[], int size);
INLINE double genrand_open_close(void);
INLINE double genrand_close_open(void);
INLINE double genrand_open_open(void);
INLINE double genrand_close1_open2(void);

#if defined(__GNUC__)
#define ALWAYSINLINE __attribute__((always_inline)) 
#else
#define ALWAYSINLINE
#endif
INLINE static void do_recursion(w128_t *r, w128_t *a, w128_t *lung)
    ALWAYSINLINE;
#if 0
INLINE static void convert_co(w128_t array[], int size) ALWAYSINLINE;
#endif
INLINE static void convert_oc(w128_t array[], int size) ALWAYSINLINE;
INLINE static void convert_oo(w128_t array[], int size) ALWAYSINLINE;

#if (defined(__BIG_ENDIAN__) || defined(BIG_ENDIAN)) && !defined(__amd64)
inline static int idxof(int i) {
    return i ^ 1;
}
#else
inline static int idxof(int i) {
    return i;
}
#endif

#ifdef SSE2
static __m128i sse2_param_mask;
static __m128i sse2_low_mask;
static __m128i sse2_high_const;
static __m128i sse2_int_one;
static __m128d sse2_double_two;
static __m128d sse2_double_m_one;

static void setup_const(void) {
    static int first = true;
    if (!first) {
	return;
    }
    sse2_param_mask = _mm_set_epi32(SFMT_MSK32_3, SFMT_MSK32_4, SFMT_MSK32_1,
				    SFMT_MSK32_2);
    sse2_low_mask = _mm_set_epi32(SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2,
				  SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2);
    sse2_high_const = _mm_set_epi32(SFMT_HIGH_CONST32, 0,
				    SFMT_HIGH_CONST32, 0);
    sse2_int_one = _mm_set_epi32(0, 1, 0, 1);
    sse2_double_two = _mm_set_pd(2.0, 2.0);
    sse2_double_m_one = _mm_set_pd(-1.0, -1.0);
    first = false;
}
#endif

#ifdef SSE2
INLINE static void do_recursion(w128_t *r, w128_t *a, w128_t *u) {
    __m128i v, w, x, y, z;
    
    x = a->si;
    y = _mm_shuffle_epi32(u->si, SSE2_SHUFF);
    z = _mm_srli_epi64(x, 1);
    v = _mm_and_si128(x, sse2_param_mask);
    y = _mm_xor_si128(y, x);
    v = _mm_xor_si128(v, y);
    r->si = v;

    w = _mm_slli_epi64(u->si, 8);
    x = _mm_xor_si128(x, u->si);
    x = _mm_xor_si128(x, w);
    u->si = x;
}
#else
INLINE static void do_recursion(w128_t *r, w128_t *a, w128_t *lung) {
    uint64_t t0, t1;

    t0 = a->a[0];
    t1 = a->a[1];
    r->a[0] = lung->a[1] ^ (t0 >> 1) ^ (t0 & SFMT_MSK1);
    r->a[1] = lung->a[0] ^ (t1 >> 1) ^ (t1 & SFMT_MSK2);
    lung->a[0] = (lung->a[0] << 8) ^ lung->a[0] ^ t0;
    lung->a[1] = (lung->a[1] << 8) ^ lung->a[1] ^ t1;
}
#endif

#if 0
INLINE static void convert_co(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].d[0] = array[i].d[0] - 1.0;
	array[i].d[1] = array[i].d[1] - 1.0;
    }
}
#endif

#ifdef SSE2 
INLINE static void filter(w128_t *a) {
    __m128i x, y;

    x = a->si;
    y = _mm_srli_epi64(x, SFMT_SR3);
    y = _mm_and_si128(y, sse2_temp_mask1);
    x = _mm_xor_si128(x, y);

    y = _mm_slli_epi64(x, SFMT_SL3);
    y = _mm_and_si128(y, sse2_temp_mask2);
    x = _mm_xor_si128(x, y);

    y = _mm_slli_epi64(x, SFMT_SL4);
    y = _mm_and_si128(y, sse2_temp_mask3);
    x = _mm_xor_si128(x, y);

    y = _mm_srli_epi64(x, SFMT_SR4);
    x = _mm_xor_si128(x, y);

    x = _mm_and_si128(x, sse2_low_mask);
    x = _mm_or_si128(x, sse2_high_const);
    a->si = x;
}
#else
INLINE static void filter(w128_t *a) {
    a->a[0] ^= (a->a[0] >> 29) & 0x5555555555555555ULL;
    a->a[1] ^= (a->a[1] >> 29) & 0x5555555555555555ULL;
    a->a[0] ^= (a->a[0] << 17) & 0x71D67FFFEDA60000ULL;
    a->a[1] ^= (a->a[1] << 17) & 0x71D67FFFEDA60000ULL;
    a->a[0] ^= (a->a[0] << 37) & 0xFFF7EEE000000000ULL;
    a->a[1] ^= (a->a[1] << 37) & 0xFFF7EEE000000000ULL;
    a->a[0] ^= (a->a[0] >> 43);
    a->a[1] ^= (a->a[1] >> 43);
    a->a[0] &= SFMT_LOW_MASK;
    a->a[1] &= SFMT_LOW_MASK;
    a->a[0] |= SFMT_HIGH_CONST;
    a->a[1] |= SFMT_HIGH_CONST;
}
#endif

INLINE static double filter52(uint64_t a) {
    w64_t w;

    a ^= (a >> 29) & 0x5555555555555555ULL;
    a ^= (a << 17) & 0x71D67FFFEDA60000ULL;
    a ^= (a << 37) & 0xFFF7EEE000000000ULL;
    a ^= (a >> 43);
    a &= SFMT_LOW_MASK;
    a |= SFMT_HIGH_CONST;
    w.u = a;
    return w.d;
}

#ifdef SSE2
INLINE static void filter_co(w128_t *a) {
    filter(a);
    a->sd = _mm_add_pd(a->sd, sse2_double_m_one);
}
#else
INLINE static void filter_co(w128_t *a) {
    filter(a);
    a->d[0] = a->d[0] - 1.0;
    a->d[1] = a->d[1] - 1.0;
}
#endif

INLINE static void convert_oc(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].d[0] = 2.0 - array[i].d[0];
	array[i].d[1] = 2.0 - array[i].d[1];
    }
}

INLINE static void convert_oo(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].a[0] |= 1;
	array[i].a[1] |= 1;
	array[i].d[0] -= 1.0;
	array[i].d[1] -= 1.0;
    }
}

INLINE static void gen_rand_all(void) {
    int i;
    w128_t lung;

    lung = sfmt[N];
    for (i = 0; i < N; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &lung);
    }
    sfmt[N] = lung;
}

INLINE static void gen_rand_array12(w128_t array[], int size) {
    int i, j;
    w128_t lung;

    lung = sfmt[N];
    for (i = 0; i < N; i++) {
	do_recursion(&array[i], &sfmt[i], &lung);
    }
    for (; i < size - N; i++) {
	do_recursion(&array[i], &array[i - N], &lung);
	filter(&array[i - N]);
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j] = array[j + size - N];
    }
    for (; i < size; i++, j++) {
	do_recursion(&array[i], &array[i - N], &lung);
	sfmt[j] = array[i];
	filter(&array[i - N]);
    }
    for (i = size - N; i < size; i++) {
	filter(&array[i]);
    }
    sfmt[N] = lung;
}

INLINE static void gen_rand_arrayco(w128_t array[], int size) {
    int i, j;
    w128_t lung;

    lung = sfmt[N];
    for (i = 0; i < N; i++) {
	do_recursion(&array[i], &sfmt[i], &lung);
    }
    for (; i < size - N; i++) {
	do_recursion(&array[i], &array[i - N], &lung);
	filter_co(&array[i - N]);
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j] = array[j + size - N];
    }
    for (; i < size; i++, j++) {
	do_recursion(&array[i], &array[i - N], &lung);
	sfmt[j] = array[i];
	filter_co(&array[i - N]);
    }
    for (i = size - N; i < size; i++) {
	filter_co(&array[i]);
    }
    sfmt[N] = lung;
}

INLINE double genrand_close1_open2(void) {
    double r;
    uint64_t *psfmt = &(sfmt[0].a[0]);

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r = filter52(psfmt[idx++]);
    return r;
}

INLINE double genrand_open_open(void) {
    w64_t r;

    r.d = genrand_close1_open2();
    r.u |= 1;
    return r.d - 1.0;
}

void fill_array_open_close(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array12((w128_t *)array, size / 2);
    convert_oc((w128_t *)array, size / 2);
}

void fill_array_close_open(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_arrayco((w128_t *)array, size / 2);
}

void fill_array_open_open(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array12((w128_t *)array, size / 2);
    convert_oo((w128_t *)array, size / 2);
}

void fill_array_close1_open2(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array12((w128_t *)array, size / 2);
}

void init_gen_rand(uint64_t seed)
{
    int i;
    uint64_t *psfmt;

    psfmt = (uint64_t *)&sfmt[0];
    psfmt[0] = seed;
    for (i = 1; i < N * 2; i++) {
	psfmt[i] = 6364136223846793005ULL 
	    * (psfmt[i - 1] ^ (psfmt[i - 1] >> 62)) + i;
    }
    for (;i < (N + 1) * 2; i++) {
	psfmt[i] = 6364136223846793005ULL 
	    * (psfmt[i - 1] ^ (psfmt[i - 1] >> 62)) + i;
    }
    idx = N * 2;
#ifdef SSE2
    setup_const();
#endif
}

#include "test_time3.c"
