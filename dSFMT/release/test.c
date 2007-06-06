#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include "dSFMT.c"

#define NUM_RANDS 50000
#define TIC_MAG 1
#define TIC_COUNT 2000

w128_t dummy[NUM_RANDS / 2 + 1];

#ifdef __GNUC__
void check_co(void) __attribute__((noinline));
void check_oc(void) __attribute__((noinline));
void check_oo(void) __attribute__((noinline));
void check_12(void) __attribute__((noinline));
void test_co(void) __attribute__((noinline));
void test_oc(void) __attribute__((noinline));
void test_oo(void) __attribute__((noinline));
void test_12(void) __attribute__((noinline));
void test_seq_co(void) __attribute__((noinline));
void test_seq_oc(void) __attribute__((noinline));
void test_seq_oo(void) __attribute__((noinline));
void test_seq_12(void) __attribute__((noinline));
#else
void check_co(void);
void check_oc(void);
void check_oo(void);
void check_12(void);
void test_co(void);
void test_oc(void);
void test_oo(void);
void test_12(void);
void test_seq_co(void);
void test_seq_oc(void);
void test_seq_oo(void);
void test_seq_12(void);
#endif

void check_co(void) {
    int i;
    union W64_T {
	uint64_t u;
	double d;
    };
    w128_t little[SFMT_N+1];
    union W64_T *array = (union W64_T *)dummy;
    union W64_T *plittle = (union W64_T *)little;
    union W64_T r;
    int lsize = SFMT_N * 2 + 2;

    printf("generated randoms [0,1)\n");
    init_gen_rand(1234);
    fill_array_close_open(&plittle[0].d, lsize);
    fill_array_close_open(&array[0].d, 5000);
    init_gen_rand(1234);
    for (i = 0; i < lsize; i++) {
	r.d = genrand_close_open();
	if (r.d != plittle[i].d) {
	    printf("\n[0,1) mismatch i = %d: r = %1.20lf(%016"PRIx64"), "
		   "plittle = %1.20lf(%016"PRIx64")\n", i, r.d, r.u,
		   plittle[i].d, plittle[i].u);
	    exit(1);
	}
	if (i < 1000) {
	    printf("%1.20lf ", plittle[i].d);
	    if (i % 3 == 2) {
		printf("\n");
	    }
	}
    }
    for (i = 0; i < 5000; i++) {
	r.d = genrand_close_open();
	if (r.d != array[i].d) {
	    printf("\n[0,1) mismatch i = %d: r = %1.20lf(%016"PRIx64"), "
		   "array = %1.20lf(%016"PRIx64")\n", i + lsize, r.d, r.u,
		   array[i].d, array[i].u);
	    exit(1);
	}
	if (i + lsize < 1000) {
	    printf("%1.20lf ", array[i].d);
	    if ((i + lsize) % 3 == 2) {
		printf("\n");
	    }
	}
    }
    printf("\n");
}

void check_oc(void) {
    int i;
    union W64_T {
	uint64_t u;
	double d;
    };
    union W64_T *array = (union W64_T *)dummy;
    union W64_T r;

    printf("generated randoms (0, 1]\n");
    init_gen_rand(1234);
    fill_array_open_close(&array[0].d, 5000);
    init_gen_rand(1234);
    for (i = 0; i < 5000; i++) {
	r.d = genrand_open_close();
	if (r.d != array[i].d) {
	    printf("\n(0,1] mismatch i = %d: r = %1.20lf(%016"PRIx64"), "
		   "array = %1.20lf(%016"PRIx64")\n", i, r.d, r.u,
		   array[i].d, array[i].u);
	    exit(1);
	}
	if (i < 1000) {
	    printf("%1.20lf ", array[i].d);
	    if (i % 3 == 2) {
		printf("\n");
	    }
	}
    }
    printf("\n");
}

void check_oo(void) {
    int i;
    union W64_T {
	uint64_t u;
	double d;
    };
    union W64_T *array = (union W64_T *)dummy;
    union W64_T r;

    printf("generated randoms (0,1)\n");
    init_gen_rand(1234);
    fill_array_open_open(&array[0].d, 5000);
    init_gen_rand(1234);
    for (i = 0; i < 5000; i++) {
	r.d = genrand_open_open();
	if (r.d != array[i].d) {
	    printf("\n(0,1) mismatch i = %d: r = %1.20lf(%016"PRIx64"), "
		   "array = %1.20lf(%016"PRIx64")\n", i, r.d, r.u,
		   array[i].d, array[i].u);
	    exit(1);
	}
	if (i < 1000) {
	    printf("%1.20lf ", array[i].d);
	    if (i % 3 == 2) {
		printf("\n");
	    }
	}
    }
    printf("\n");
}

void check_12(void) {
    int i;
    union W64_T {
	uint64_t u;
	double d;
    };
    w128_t little[SFMT_N+1];
    union W64_T *array = (union W64_T *)dummy;
    union W64_T *plittle = (union W64_T *)little;
    union W64_T r;
    int lsize = SFMT_N * 2 + 2;

    printf("generated randoms [1, 2)\n");
    init_gen_rand(1234);
    fill_array_close1_open2(&plittle[0].d, lsize);
    fill_array_close1_open2(&array[0].d, 5000);
    init_gen_rand(1234);
    for (i = 0; i < lsize; i++) {
	r.d = genrand_close1_open2();
	if (r.d != plittle[i].d) {
	    printf("\n[1, 2) mismatch i = %d: r = %1.20lf(%016"PRIx64"), "
		   "plittle = %1.20lf(%016"PRIx64")\n", i, r.d, r.u,
		   plittle[i].d, plittle[i].u);
	    exit(1);
	}
	if (i < 1000) {
	    printf("%1.20lf ", plittle[i].d);
	    if (i % 3 == 2) {
		printf("\n");
	    }
	}
    }
    for (i = 0; i < 5000; i++) {
	r.d = genrand_close1_open2();
	if (r.d != array[i].d) {
	    printf("\n[1, 2) mismatch i = %d: r = %1.20lf(%016"PRIx64"), "
		   "array = %1.20lf(%016"PRIx64")\n", i + lsize, r.d, r.u,
		   array[i].d, array[i].u);
	    exit(1);
	}
	if (i + lsize < 1000) {
	    printf("%1.20lf ", array[i].d);
	    if ((i + lsize) % 3 == 2) {
		printf("\n");
	    }
	}
    }
    printf("\n");
}

void test_co(void) {
    uint32_t i, j;
    uint64_t clo;
    uint64_t sum;
    uint64_t min;
    double *array = (double *)dummy;

    init_gen_rand(1234);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    fill_array_close_open(array, NUM_RANDS);
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("BLOCK [0, 1) AVE:%4"PRIu64"ms.\n",  (sum * 100) / CLOCKS_PER_SEC);
}

void test_oc(void) {
    uint32_t i, j;
    uint64_t clo;
    uint64_t sum;
    uint64_t min;
    double *array = (double *)dummy;

    init_gen_rand(1234);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    fill_array_open_close(array, NUM_RANDS);
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("BLOCK (0, 1] AVE:%4"PRIu64"ms.\n",  (sum * 100) / CLOCKS_PER_SEC);
}

void test_oo(void) {
    uint32_t i, j;
    uint64_t clo;
    uint64_t sum;
    uint64_t min;
    double *array = (double *)dummy;

    init_gen_rand(1234);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    fill_array_open_open(array, NUM_RANDS);
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("BLOCK (0, 1) AVE:%4"PRIu64"ms.\n",  (sum * 100) / CLOCKS_PER_SEC);
}

void test_12(void) {
    uint32_t i, j;
    uint64_t clo;
    uint64_t sum;
    uint64_t min;
    double *array = (double *)dummy;

    init_gen_rand(1234);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    fill_array_close1_open2(array, NUM_RANDS);
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("BLOCK [1, 2) AVE:%4"PRIu64"ms.\n",  (sum * 100) / CLOCKS_PER_SEC);
}

void test_seq_co(void) {
    uint32_t i, j, k;
    uint64_t clo;
    uint64_t sum;
    uint64_t min;
    double *array = (double *)dummy;
    double r;
    double total = 0;

    min = LONG_MAX;
    sum = 0;
    r = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		r += genrand_close_open();
	    }
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    total = r;
    printf("SEQ [0, 1) 1 AVE:%4"PRIu64"ms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = genrand_close_open();
	    }
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    total = 0;
    for (k = 0; k < NUM_RANDS; k++) {
	total += array[k];
    }
    printf("SEQ [0, 1) 2 AVE:%4"PRIu64"ms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = 1.0;
	    }
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    total = 0;
    for (k = 0; k < NUM_RANDS; k++) {
	total += array[k];
    }
    printf("ADD   AVE:%4"PRIu64"ms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
}

void test_seq_oc(void) {
    uint32_t i, j, k;
    uint64_t clo;
    uint64_t sum;
    uint64_t min;
    double *array = (double *)dummy;
    double r;
    double total = 0;

    min = LONG_MAX;
    sum = 0;
    r = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		r += genrand_open_close();
	    }
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    total = r;
    printf("SEQ (0, 1] 1 AVE:%4"PRIu64"ms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = genrand_open_close();
	    }
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    total = 0;
    for (k = 0; k < NUM_RANDS; k++) {
	total += array[k];
    }
    printf("SEQ (0, 1] 2 AVE:%4"PRIu64"ms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = 1.0;
	    }
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    total = 0;
    for (k = 0; k < NUM_RANDS; k++) {
	total += array[k];
    }
    printf("ADD   AVE:%4"PRIu64"ms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
}

void test_seq_oo(void) {
    uint32_t i, j, k;
    uint64_t clo;
    uint64_t sum;
    uint64_t min;
    double *array = (double *)dummy;
    double r;
    double total = 0;

    min = LONG_MAX;
    sum = 0;
    r = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		r += genrand_open_open();
	    }
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    total = r;
    printf("SEQ (0, 1) 1 AVE:%4"PRIu64"ms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = genrand_open_open();
	    }
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    total = 0;
    for (k = 0; k < NUM_RANDS; k++) {
	total += array[k];
    }
    printf("SEQ (0, 1) 2 AVE:%4"PRIu64"ms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = 1.0;
	    }
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    total = 0;
    for (k = 0; k < NUM_RANDS; k++) {
	total += array[k];
    }
    printf("ADD   AVE:%4"PRIu64"ms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
}

void test_seq_12(void) {
    uint32_t i, j, k;
    uint64_t clo;
    uint64_t sum;
    uint64_t min;
    double *array = (double *)dummy;
    double r;
    double total = 0;

    min = LONG_MAX;
    sum = 0;
    r = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		r += genrand_close1_open2();
	    }
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    total = r;
    printf("SEQ [1, 2) 1 AVE:%4"PRIu64"ms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = genrand_close1_open2();
	    }
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    total = 0;
    for (k = 0; k < NUM_RANDS; k++) {
	total += array[k];
    }
    printf("SEQ [1, 2) 2 AVE:%4"PRIu64"ms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = 1.0;
	    }
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    total = 0;
    for (k = 0; k < NUM_RANDS; k++) {
	total += array[k];
    }
    printf("ADD   AVE:%4"PRIu64"ms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
}

int main(int argc, char *argv[]) {
    if ((argc >= 2) && (strncmp(argv[1],"-v",2) == 0)) {
	printf("%s\n", get_idstring());
	check_12();
    } else if ((argc >= 2) && (strncmp(argv[1],"-s",2) == 0)) {
	printf("consumed time for generating %u randoms.\n",
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
	check_co();
	check_oc();
	check_oo();
	check_12();
    }
    return 0;
}
