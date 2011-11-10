#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#define DSFMT_DO_NOT_USE_OLD_NAMES
#include "dSFMT.h"

#define NUM_RANDS 50000
#define TIC_MAG 1
#define TIC_COUNT 2000

w128_t dummy[NUM_RANDS / 2 + 1];

typedef double (*genrand_t)(void);
typedef double (*st_genrand_t)(dsfmt_t *dsfmt);
typedef void (*fill_array_t)(double array[], int size);
typedef void (*st_fill_array_t)(dsfmt_t *dsfmt, double array[], int size);

#ifdef __GNUC__
static void test_co(void) __attribute__((noinline));
static void test_oc(void) __attribute__((noinline));
static void test_oo(void) __attribute__((noinline));
static void test_12(void) __attribute__((noinline));
static void test_seq_co(void) __attribute__((noinline));
static void test_seq_oc(void) __attribute__((noinline));
static void test_seq_oo(void) __attribute__((noinline));
static void test_seq_12(void) __attribute__((noinline));
#else
static void test_co(void);
static void test_oc(void);
static void test_oo(void);
static void test_12(void);
static void test_seq_co(void);
static void test_seq_oc(void);
static void test_seq_oo(void);
static void test_seq_12(void);
#endif
static void check(char *start_mess, genrand_t genrand, fill_array_t fill_array,
		  st_genrand_t st_genrand, st_fill_array_t st_fill_array,
		  uint32_t seed, int n);
static void check_ar(char *start_mess, genrand_t genrand, 
		     fill_array_t fill_array,
		     st_genrand_t st_genrand,
		     st_fill_array_t st_fill_array,
		     int n);

/* not inline wrapper functions for check() */
static double s_genrand_close_open(void) {
    return dsfmt_gv_genrand_close_open();
}
static double s_genrand_open_close(void) {
    return dsfmt_gv_genrand_open_close();
}
static double s_genrand_open_open(void) {
    return dsfmt_gv_genrand_open_open();
}
static double s_genrand_close1_open2(void) {
    return dsfmt_gv_genrand_close1_open2();
}
static double sst_genrand_close_open(dsfmt_t *dsfmt) {
    return dsfmt_genrand_close_open(dsfmt);
}
static double sst_genrand_open_close(dsfmt_t *dsfmt) {
    return dsfmt_genrand_open_close(dsfmt);
}
static double sst_genrand_open_open(dsfmt_t *dsfmt) {
    return dsfmt_genrand_open_open(dsfmt);
}
static double sst_genrand_close1_open2(dsfmt_t *dsfmt) {
    return dsfmt_genrand_close1_open2(dsfmt);
}
static void s_fill_array_close_open(double array[], int size) {
    dsfmt_gv_fill_array_close_open(array, size);
}
static void s_fill_array_open_close(double array[], int size) {
    dsfmt_gv_fill_array_open_close(array, size);
}
static void s_fill_array_open_open(double array[], int size) {
    dsfmt_gv_fill_array_open_open(array, size);
}
static void s_fill_array_close1_open2(double array[], int size) {
    dsfmt_gv_fill_array_close1_open2(array, size);
}
static void sst_fill_array_close_open(dsfmt_t *dsfmt, double array[],
				      int size) {
    dsfmt_fill_array_close_open(dsfmt, array, size);
}
static void sst_fill_array_open_close(dsfmt_t *dsfmt, double array[],
				      int size) {
    dsfmt_fill_array_open_close(dsfmt, array, size);
}
static void sst_fill_array_open_open(dsfmt_t *dsfmt, double array[],
				      int size) {
    dsfmt_fill_array_open_open(dsfmt, array, size);
}
static void sst_fill_array_close1_open2(dsfmt_t *dsfmt, double array[],
				      int size) {
    dsfmt_fill_array_close1_open2(dsfmt, array, size);
}

static void check(char *range_str, genrand_t genrand, fill_array_t fill_array,
		  st_genrand_t st_genrand, st_fill_array_t st_fill_array,
		  uint32_t seed, int print_size) {
    int i;
    union W64_T {
	uint64_t u;
	double d;
    };
    w128_t little[DSFMT_N+1];
    union W64_T *array = (union W64_T *)dummy;
    union W64_T *plittle = (union W64_T *)little;
    union W64_T r, r_st;
    int lsize = DSFMT_N * 2 + 2;
    dsfmt_t dsfmt;

    printf("generated randoms %s\n", range_str);
    dsfmt_gv_init_gen_rand(seed);
    fill_array(&plittle[0].d, lsize);
    fill_array(&array[0].d, 5000);
    dsfmt_gv_init_gen_rand(seed);
    dsfmt_init_gen_rand(&dsfmt, seed);
    for (i = 0; i < lsize; i++) {
	r.d = genrand();
	r_st.d = st_genrand(&dsfmt);
	if (r.u != r_st.u || r.u != plittle[i].u) {
	    printf("\n%s mismatch i = %d: r = %1.15f(%08"PRIx64"), "
		   "st = %1.15f(%08"PRIx64")"
		   "array = %1.15f(%08"PRIx64")\n",
		   range_str, i, r.d, r.u,
		   r_st.d, r_st.u,
		   plittle[i].d, plittle[i].u);
	    exit(1);
	}
	if (i < print_size) {
	    printf("%1.15f ", plittle[i].d);
	    if (i % 4 == 3) {
		printf("\n");
	    }
	}
    }
    for (i = 0; i < 5000; i++) {
	r.d = genrand();
	if (r.u != array[i].u) {
	    printf("\n%s mismatch i = %d: r = %1.15f(%08"PRIx64"), "
		   "array = %1.15f(%08"PRIx64")\n",
		   range_str, i + lsize, r.d, r.u,
		   array[i].d, array[i].u);
	    exit(1);
	}
	if (i + lsize < print_size) {
	    printf("%1.15f ", array[i].d);
	    if ((i + lsize) % 4 == 3) {
		printf("\n");
	    }
	}
    }

    dsfmt_init_gen_rand(&dsfmt, seed);
    st_fill_array(&dsfmt, &plittle[0].d, lsize);
    st_fill_array(&dsfmt, &array[0].d, 5000);
    dsfmt_init_gen_rand(&dsfmt, seed);
    for (i = 0; i < lsize; i++) {
	r_st.d = st_genrand(&dsfmt);
	if (r_st.u != plittle[i].u) {
	    printf("\n%s mismatch i = %d: st = %1.15f(%08"PRIx64"), "
		   "array = %1.15f(%08"PRIx64")\n",
		   range_str, i, r_st.d, r_st.u,
		   plittle[i].d, plittle[i].u);
	    exit(1);
	}
    }
    for (i = 0; i < 5000; i++) {
	r_st.d = st_genrand(&dsfmt);
	if (r_st.u != array[i].u) {
	    printf("\n%s mismatch i = %d: r = %1.15f(%08"PRIx64"), "
		   "array = %1.15f(%08"PRIx64")\n",
		   range_str, i + lsize, r_st.d, r_st.u,
		   array[i].d, array[i].u);
	    exit(1);
	}
    }
}

static void check_ar(char *range_str, genrand_t genrand,
		     fill_array_t fill_array,
		     st_genrand_t st_genrand,
		     st_fill_array_t st_fill_array,
		     int print_size) {
    int i;
    union W64_T {
	uint64_t u;
	double d;
    };
    w128_t little[DSFMT_N+1];
    union W64_T *array = (union W64_T *)dummy;
    union W64_T *plittle = (union W64_T *)little;
    union W64_T r, r_st;
    int lsize = DSFMT_N * 2 + 2;
    dsfmt_t dsfmt;
    uint32_t ar[4] = {1, 2, 3, 4};

    printf("generated randoms %s\n", range_str);
    dsfmt_gv_init_by_array(ar, 4);
    fill_array(&plittle[0].d, lsize);
    fill_array(&array[0].d, 5000);
    dsfmt_gv_init_by_array(ar, 4);
    dsfmt_init_by_array(&dsfmt, ar, 4);
    for (i = 0; i < lsize; i++) {
	r.d = genrand();
	r_st.d = st_genrand(&dsfmt);
	if (r.u != r_st.u || r.u != plittle[i].u) {
	    printf("\n%s mismatch i = %d: r = %1.15f(%08"PRIx64"), "
		   "st = %1.15f(%08"PRIx64")"
		   "array = %1.15f(%08"PRIx64")\n",
		   range_str, i, r.d, r.u,
		   r_st.d, r_st.u,
		   plittle[i].d, plittle[i].u);
	    exit(1);
	}
	if (i < print_size) {
	    printf("%1.15f ", plittle[i].d);
	    if (i % 4 == 3) {
		printf("\n");
	    }
	}
    }
    for (i = 0; i < 5000; i++) {
	r.d = genrand();
	if (r.u != array[i].u) {
	    printf("\n%s mismatch i = %d: r = %1.15f(%08"PRIx64"), "
		   "array = %1.15f(%08"PRIx64")\n",
		   range_str, i + lsize, r.d, r.u,
		   array[i].d, array[i].u);
	    exit(1);
	}
	if (i + lsize < print_size) {
	    printf("%1.15f ", array[i].d);
	    if ((i + lsize) % 4 == 3) {
		printf("\n");
	    }
	}
    }

    dsfmt_init_by_array(&dsfmt, ar, 4);
    st_fill_array(&dsfmt, &plittle[0].d, lsize);
    st_fill_array(&dsfmt, &array[0].d, 5000);
    dsfmt_init_by_array(&dsfmt, ar, 4);
    for (i = 0; i < lsize; i++) {
	r_st.d = st_genrand(&dsfmt);
	if (r_st.u != plittle[i].u) {
	    printf("\n%s mismatch i = %d: st = %1.15f(%08"PRIx64"), "
		   "array = %1.15f(%08"PRIx64")\n",
		   range_str, i, r_st.d, r_st.u,
		   plittle[i].d, plittle[i].u);
	    exit(1);
	}
    }
    for (i = 0; i < 5000; i++) {
	r_st.d = st_genrand(&dsfmt);
	if (r_st.u != array[i].u) {
	    printf("\n%s mismatch i = %d: r = %1.15f(%08"PRIx64"), "
		   "array = %1.15f(%08"PRIx64")\n",
		   range_str, i + lsize, r_st.d, r_st.u,
		   array[i].d, array[i].u);
	    exit(1);
	}
    }
}

static void test_co(void) {
    uint32_t i, j;
    uint64_t clo;
    uint64_t sum;
    double *array = (double *)dummy;
    dsfmt_t dsfmt;

#if 0
    dsfmt_gv_init_gen_rand(1234);
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    dsfmt_gv_fill_array_close_open(array, NUM_RANDS);
	}
	clo = clock() - clo;
	sum += clo;
    }
    printf("GL BLOCK [0, 1) AVE:%4"PRIu64"ms.\n",
	   (sum * 100) / CLOCKS_PER_SEC);
#endif
    dsfmt_init_gen_rand(&dsfmt, 1234);
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    dsfmt_fill_array_close_open(&dsfmt, array, NUM_RANDS);
	}
	clo = clock() - clo;
	sum += clo;
    }
    printf("ST BLOCK [0, 1) AVE:%4"PRIu64"ms.\n",
	   (sum * 100) / CLOCKS_PER_SEC);
}

static void test_oc(void) {
    uint32_t i, j;
    uint64_t clo;
    uint64_t sum;
    double *array = (double *)dummy;
    dsfmt_t dsfmt;
#if 0
    dsfmt_gv_init_gen_rand(1234);
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    dsfmt_gv_fill_array_open_close(array, NUM_RANDS);
	}
	clo = clock() - clo;
	sum += clo;
    }
    printf("GL BLOCK (0, 1] AVE:%4"PRIu64"ms.\n",
	   (sum * 100) / CLOCKS_PER_SEC);
#endif
    dsfmt_init_gen_rand(&dsfmt, 1234);
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    dsfmt_fill_array_open_close(&dsfmt, array, NUM_RANDS);
	}
	clo = clock() - clo;
	sum += clo;
    }
    printf("ST BLOCK (0, 1] AVE:%4"PRIu64"ms.\n",
	   (sum * 100) / CLOCKS_PER_SEC);
}

static void test_oo(void) {
    uint32_t i, j;
    uint64_t clo;
    uint64_t sum;
    double *array = (double *)dummy;
    dsfmt_t dsfmt;
#if 0
    dsfmt_gv_init_gen_rand(1234);
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    dsfmt_gv_fill_array_open_open(array, NUM_RANDS);
	}
	clo = clock() - clo;
	sum += clo;
    }
    printf("GL BLOCK (0, 1) AVE:%4"PRIu64"ms.\n",
	   (sum * 100) / CLOCKS_PER_SEC);
#endif
    dsfmt_init_gen_rand(&dsfmt, 1234);
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    dsfmt_fill_array_open_open(&dsfmt, array, NUM_RANDS);
	}
	clo = clock() - clo;
	sum += clo;
    }
    printf("ST BLOCK (0, 1) AVE:%4"PRIu64"ms.\n",
	   (sum * 100) / CLOCKS_PER_SEC);
}

static void test_12(void) {
    uint32_t i, j;
    uint64_t clo;
    uint64_t sum;
    double *array = (double *)dummy;
    dsfmt_t dsfmt;
#if 0
    dsfmt_gv_init_gen_rand(1234);
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    dsfmt_gv_fill_array_close1_open2(array, NUM_RANDS);
	}
	clo = clock() - clo;
	sum += clo;
    }
    printf("GL BLOCK [1, 2) AVE:%4"PRIu64"ms.\n",
	   (sum * 100) / CLOCKS_PER_SEC);
#endif
    dsfmt_init_gen_rand(&dsfmt, 1234);
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    dsfmt_fill_array_close1_open2(&dsfmt, array, NUM_RANDS);
	}
	clo = clock() - clo;
	sum += clo;
    }
    printf("ST BLOCK [1, 2) AVE:%4"PRIu64"ms.\n",
	   (sum * 100) / CLOCKS_PER_SEC);
}

static void test_seq_co(void) {
    uint32_t i, j, k;
    uint64_t clo;
    uint64_t sum;
    double *array = (double *)dummy;
    double r;
    double total = 0;
    dsfmt_t dsfmt;
#if 0
    dsfmt_gv_init_gen_rand(1234);
    sum = 0;
    r = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		r += dsfmt_gv_genrand_close_open();
	    }
	}
	clo = clock() - clo;
	sum += clo;
    }
    total = r;
    printf("GL SEQ [0, 1) 1 AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);

    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = dsfmt_gv_genrand_close_open();
	    }
	}
	clo = clock() - clo;
	sum += clo;
    }
    for (k = 0; k < NUM_RANDS; k++) {
	total += array[k];
    }
    printf("GL SEQ [0, 1) 2 AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);
#endif
    dsfmt_init_gen_rand(&dsfmt, 1234);
    sum = 0;
    r = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		r += dsfmt_genrand_close_open(&dsfmt);
	    }
	}
	clo = clock() - clo;
	sum += clo;
    }
    total = r;
    printf("ST SEQ [0, 1) 1 AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);

    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = dsfmt_genrand_close_open(&dsfmt);
	    }
	}
	clo = clock() - clo;
	sum += clo;
    }
    for (k = 0; k < NUM_RANDS; k++) {
	total += array[k];
    }
    printf("ST SEQ [0, 1) 2 AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);

    printf("total = %f\n", total);
}

static void test_seq_oc(void) {
    uint32_t i, j, k;
    uint64_t clo;
    uint64_t sum;
    double *array = (double *)dummy;
    double r;
    double total = 0;
    dsfmt_t dsfmt;
#if 0
    dsfmt_gv_init_gen_rand(1234);
    sum = 0;
    r = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		r += dsfmt_gv_genrand_open_close();
	    }
	}
	clo = clock() - clo;
	sum += clo;
    }
    total = r;
    printf("GL SEQ (0, 1] 1 AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = dsfmt_gv_genrand_open_close();
	    }
	}
	clo = clock() - clo;
	sum += clo;
    }
    for (k = 0; k < NUM_RANDS; k++) {
	total += array[k];
    }
    printf("GL SEQ (0, 1] 2 AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);
#endif
    dsfmt_init_gen_rand(&dsfmt, 1234);
    sum = 0;
    r = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		r += dsfmt_genrand_open_close(&dsfmt);
	    }
	}
	clo = clock() - clo;
	sum += clo;
    }
    total += r;
    printf("ST SEQ (0, 1] 1 AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = dsfmt_genrand_open_close(&dsfmt);
	    }
	}
	clo = clock() - clo;
	sum += clo;
    }
    for (k = 0; k < NUM_RANDS; k++) {
	total += array[k];
    }
    printf("ST SEQ (0, 1] 2 AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %f\n", total);
}

static void test_seq_oo(void) {
    uint32_t i, j, k;
    uint64_t clo;
    uint64_t sum;
    double *array = (double *)dummy;
    double r;
    double total = 0;
    dsfmt_t dsfmt;
#if 0
    dsfmt_gv_init_gen_rand(1234);
    sum = 0;
    r = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		r += dsfmt_gv_genrand_open_open();
	    }
	}
	clo = clock() - clo;
	sum += clo;
    }
    total = r;
    printf("GL SEQ (0, 1) 1 AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = dsfmt_gv_genrand_open_open();
	    }
	}
	clo = clock() - clo;
	sum += clo;
    }
    for (k = 0; k < NUM_RANDS; k++) {
	total += array[k];
    }
    printf("GL SEQ (0, 1) 2 AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);
#endif
    dsfmt_init_gen_rand(&dsfmt, 1234);
    sum = 0;
    r = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		r += dsfmt_genrand_open_open(&dsfmt);
	    }
	}
	clo = clock() - clo;
	sum += clo;
    }
    total += r;
    printf("ST SEQ (0, 1) 1 AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = dsfmt_genrand_open_open(&dsfmt);
	    }
	}
	clo = clock() - clo;
	sum += clo;
    }
    for (k = 0; k < NUM_RANDS; k++) {
	total += array[k];
    }
    printf("ST SEQ (0, 1) 2 AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %f\n", total);
}

static void test_seq_12(void) {
    uint32_t i, j, k;
    uint64_t clo;
    uint64_t sum;
    double *array = (double *)dummy;
    double r;
    double total = 0;
    dsfmt_t dsfmt;
#if 0
    dsfmt_gv_init_gen_rand(1234);
    sum = 0;
    r = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		r += dsfmt_gv_genrand_close1_open2();
	    }
	}
	clo = clock() - clo;
	sum += clo;
    }
    total = r;
    printf("GL SEQ [1, 2) 1 AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = dsfmt_gv_genrand_close1_open2();
	    }
	}
	clo = clock() - clo;
	sum += clo;
    }
    for (k = 0; k < NUM_RANDS; k++) {
	total += array[k];
    }
    printf("GL SEQ [1, 2) 2 AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);
#endif
    dsfmt_init_gen_rand(&dsfmt, 1234);
    sum = 0;
    r = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		r += dsfmt_genrand_close1_open2(&dsfmt);
	    }
	}
	clo = clock() - clo;
	sum += clo;
    }
    total += r;
    printf("ST SEQ [1, 2) 1 AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = dsfmt_genrand_close1_open2(&dsfmt);
	    }
	}
	clo = clock() - clo;
	sum += clo;
    }
    for (k = 0; k < NUM_RANDS; k++) {
	total += array[k];
    }
    printf("ST SEQ [1, 2) 2 AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %f\n", total);
}

int main(int argc, char *argv[]) {
    int i;

    if ((argc >= 2) && (strncmp(argv[1],"-s",2) == 0)) {
	printf("consumed time for generating %d randoms.\n",
	       NUM_RANDS * TIC_COUNT);
	test_co();
	test_oc();
	test_oo();
	test_12();
	test_seq_co();
	test_seq_oc();
	test_seq_oo();
	test_seq_12();
    } else {
	printf("%s\n", dsfmt_get_idstring());
	printf("init_gen_rand(0) ");
	check("[1, 2)", s_genrand_close1_open2, s_fill_array_close1_open2,
	      sst_genrand_close1_open2, sst_fill_array_close1_open2, 0, 1000);
	for (i = 0; i < 20; i++) {
	    printf("init_gen_rand(%d) ", i);
	    switch (i % 4) {
	    case 0:
		check("[0, 1)", s_genrand_close_open, s_fill_array_close_open,
		      sst_genrand_close_open, sst_fill_array_close_open, i, 12);
		break;
	    case 1:
		check("(0, 1]", s_genrand_open_close, s_fill_array_open_close,
		      sst_genrand_open_close, sst_fill_array_open_close, i, 12);
		break;
	    case 2:
		check("(0, 1)", s_genrand_open_open, s_fill_array_open_open,
		      sst_genrand_open_open, sst_fill_array_open_open, i, 12);
		break;
	    case 3:
	    default:
		check("[1, 2)", s_genrand_close1_open2, 
		      s_fill_array_close1_open2,
		      sst_genrand_close1_open2, 
		      sst_fill_array_close1_open2, i, 12);
	    }
	}
	printf("init_by_array {1, 2, 3, 4} ");
	check_ar("[1, 2)", s_genrand_close1_open2, 
		 s_fill_array_close1_open2,
		 sst_genrand_close1_open2, 
		 sst_fill_array_close1_open2,
		 1000);
    }
    return 0;
}
