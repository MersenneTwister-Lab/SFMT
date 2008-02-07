/* Simple and Fast Twister */

#include <string.h>
#if defined(__GNUC__)
  #include <stdint.h>
#endif
#include <stdio.h>
#include <assert.h>
#include "random.h"
#include "paramsDPza.h"

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 104
#define N ((MEXP - 128) / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N + 128)

#if defined(SSE2)
union W128_T {
    __m128i si;
    __m128d sd;
    uint64_t a[2];
    double d[2];
};
#elif defined(ALTIVEC)
union W128_T {
    vector unsigned int v;
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
INLINE static void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *lung)
    ALWAYSINLINE;
#if 0
INLINE static void convert_co(w128_t array[], int size) ALWAYSINLINE;
#endif
INLINE static void convert_co128(w128_t *r);
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

#if defined(SSE2)
INLINE static void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *u) {
    __m128i v, w, x, y, z;
    
    x = a->si;
    z = _mm_slli_epi64(x, SFMT_SL1);
    y = _mm_shuffle_epi32(u->si, SSE2_SHUFF);
    z = _mm_xor_si128(z, b->si);
    y = _mm_xor_si128(y, z);

    v = _mm_srli_epi64(y, SFMT_SR1);
    w = _mm_and_si128(y, sse2_param_mask);
    v = _mm_xor_si128(v, x);
    v = _mm_xor_si128(v, w);
    r->si = v;
    u->si = y;
}
#elif defined(ALTIVEC)
INLINE static void do_recursion(w128_t *r, w128_t *a, w128_t * b,
				w128_t *lung) {
    const vector unsigned char sl1 = ALTI_SL1;
    const vector unsigned char sl1_perm = ALTI_SL1_PERM;
    const vector unsigned int sl1_msk = ALTI_SL1_MSK;
    const vector unsigned char sr1 = ALTI_SR1;
    const vector unsigned char sr1_perm = ALTI_SR1_PERM;
    const vector unsigned int sr1_msk = ALTI_SR1_MSK;
    const vector unsigned char perm = ALTI_PERM;
    const vector unsigned int msk1 = ALTI_MSK1;
    vector unsigned int w, x, y, z;

    z = a->v;
    w = lung->v;
    x = vec_perm(w, (vector unsigned int)perm, perm);
    y = vec_perm(z, sl1_perm, sl1_perm);
    y = vec_sll(y, sl1);
    y = vec_and(y, sl1_msk);
    w = vec_xor(x, b->v);
    w = vec_xor(w, y);
    x = vec_perm(w, (vector unsigned int)sr1_perm, sr1_perm);
    x = vec_srl(x, sr1);
    x = vec_and(x, sr1_msk);
    y = vec_and(w, msk1);
    z = vec_xor(z, y);
    r->v = vec_xor(z, x);
}
#else
INLINE static void do_recursion(w128_t *r, w128_t *a, w128_t * b,
				w128_t *lung) {
    uint64_t t0, t1, L0, L1;

    t0 = a->a[0];
    t1 = a->a[1];
    L0 = lung->a[0];
    L1 = lung->a[1];
    lung->a[0] = (t0 << SFMT_SL1) ^ (L1 >> 32) ^ (L1 << 32) ^ b->a[0];
    lung->a[1] = (t1 << SFMT_SL1) ^ (L0 >> 32) ^ (L0 << 32) ^ b->a[1];
    r->a[0] = (lung->a[0] >> 12) ^ (lung->a[0] & SFMT_MSK1) ^ t0;
    r->a[1] = (lung->a[1] >> 12) ^ (lung->a[1] & SFMT_MSK2) ^ t1;
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

#if defined(SSE2)
INLINE static void convert_co128(w128_t *r) {
    r->sd = _mm_add_pd(r->sd, sse2_double_m_one);
}
#else
INLINE static void convert_co128(w128_t *r) {
    r->d[0] = r->d[0] - 1.0;
    r->d[1] = r->d[1] - 1.0;
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
    for (i = 0; i < N - SFMT_POS1; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + SFMT_POS1], &lung);
    }
    for (; i < N; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + SFMT_POS1 - N], &lung);
    }
    sfmt[N] = lung;
}

INLINE static void gen_rand_array12(w128_t array[], int size) {
    int i, j;
    w128_t lung;

    lung = sfmt[N];
    for (i = 0; i < N - SFMT_POS1; i++) {
	do_recursion(&array[i], &sfmt[i], &sfmt[i + SFMT_POS1], &lung);
    }
    for (; i < N; i++) {
	do_recursion(&array[i], &sfmt[i], &array[i + SFMT_POS1 - N], &lung);
    }
    for (; i < size - N; i++) {
	do_recursion(&array[i], &array[i - N], &array[i + SFMT_POS1 - N],
		     &lung);
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j] = array[j + size - N];
    }
    for (; i < size; i++, j++) {
	do_recursion(&array[i], &array[i - N], &array[i + SFMT_POS1 - N],
		     &lung);
	sfmt[j] = array[i];
    }
    sfmt[N] = lung;
}

INLINE static void gen_rand_arrayco(w128_t array[], int size) {
    int i, j;
    w128_t lung;

    lung = sfmt[N];
    for (i = 0; i < N - SFMT_POS1; i++) {
	do_recursion(&array[i], &sfmt[i], &sfmt[i + SFMT_POS1], &lung);
    }
    for (; i < N; i++) {
	do_recursion(&array[i], &sfmt[i], &array[i + SFMT_POS1 - N], &lung);
    }
    for (; i < size - N; i++) {
	do_recursion(&array[i], &array[i - N], &array[i + SFMT_POS1 - N],
		     &lung);
	convert_co128(&array[i - N]);
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j] = array[j + size - N];
    }
    for (; i < size; i++, j++) {
	do_recursion(&array[i], &array[i - N], &array[i + SFMT_POS1 - N],
		     &lung);
	sfmt[j] = array[i];
	convert_co128(&array[i - N]);
    }
    for (i = size - N; i < size; i++) {
	convert_co128(&array[i]);
    }
    sfmt[N] = lung;
}

INLINE double genrand_close1_open2(void) {
    double r;
    double *dsfmt = &(sfmt[0].d[0]);

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r = dsfmt[idx++];
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
    psfmt[0] = (seed & SFMT_LOW_MASK) | SFMT_HIGH_CONST;
    for (i = 1; i < N * 2; i++) {
	psfmt[i] = 6364136223846793005ULL 
	    * (psfmt[i - 1] ^ (psfmt[i - 1] >> 62)) + i;
	psfmt[i] = (psfmt[i] & SFMT_LOW_MASK) | SFMT_HIGH_CONST;
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
