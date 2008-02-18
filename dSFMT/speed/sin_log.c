#include "c99.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "mt19937ar.h"

#define NUM 10000

void test_sin(void) {
    uint32_t i, j, k;
    uint64_t clo;
    double array1[NUM];
    double array2[NUM];
    double dsum = 0;

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	array1[j] = genrand_res53() * 2 * M_PI;
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM; j++) {
	    array2[j] = sin(array1[j]);
	}
    }
    clo = clock() - clo;
    printf("sin [0,2pi) %"PRIu64" AVE:%4"PRIu64"ms.\n",  (uint64_t)NUM * NUM,
	   (clo * 100) / CLOCKS_PER_SEC);
}

void test_sin2(void) {
    uint32_t i, j, k;
    uint64_t clo;
    double array1[NUM];
    double array2[NUM];
    double dsum = 0;

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	array1[j] = (genrand_res53() + 1.0) * 2 * M_PI;
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM; j++) {
	    array2[j] = sin(array1[j]);
	}
    }
    clo = clock() - clo;
    printf("sin [2pi,4pi) %"PRIu64" AVE:%4"PRIu64"ms.\n",  (uint64_t)NUM * NUM,
	   (clo * 100) / CLOCKS_PER_SEC);
}

void test_sin3(void) {
    uint32_t i, j, k;
    uint64_t clo;
    double array1[NUM];
    double array2[NUM];
    double dsum = 0;

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	array1[j] = genrand_res53() * M_PI / 2;
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM; j++) {
	    array2[j] = sin(array1[j]);
	}
    }
    clo = clock() - clo;
    printf("sin [0, 0.5pi) %"PRIu64" AVE:%4"PRIu64"ms.\n",  (uint64_t)NUM * NUM,
	   (clo * 100) / CLOCKS_PER_SEC);
}

void test_sin4(void) {
    uint32_t i, j, k;
    uint64_t clo;
    double array1[NUM];
    double array2[NUM];
    double dsum = 0;

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	array1[j] = genrand_res53() * M_PI / 4;
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM; j++) {
	    array2[j] = sin(array1[j]);
	}
    }
    clo = clock() - clo;
    printf("sin [0, 0.25pi) %"PRIu64" AVE:%4"PRIu64"ms.\n",  (uint64_t)NUM * NUM,
	   (clo * 100) / CLOCKS_PER_SEC);
}

void test_sin5(void) {
    uint32_t i, j, k;
    uint64_t clo;
    double array1[NUM];
    double array2[NUM];
    double dsum = 0;

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	array1[j] = genrand_res53() * M_PI / 8;
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM; j++) {
	    array2[j] = sin(array1[j]);
	}
    }
    clo = clock() - clo;
    printf("sin [0, 1/8pi) %"PRIu64" AVE:%4"PRIu64"ms.\n",  (uint64_t)NUM * NUM,
	   (clo * 100) / CLOCKS_PER_SEC);
}

void test_log(void) {
    uint32_t i, j, k;
    uint64_t clo;
    double array1[NUM];
    double array2[NUM];
    double dsum = 0;

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	array1[j] = genrand_res53();
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM; j++) {
	    array2[j] = log(array1[j]);
	}
    }
    clo = clock() - clo;
    printf("log [0,1) %"PRIu64" AVE:%4"PRIu64"ms.\n",  (uint64_t)NUM * NUM,
	   (clo * 100) / CLOCKS_PER_SEC);
}

int main(void) {
    test_sin();
    test_sin2();
    test_sin3();
    test_sin4();
    test_sin5();
    test_log();
#ifdef SSE2
#endif
    return 0;
}
