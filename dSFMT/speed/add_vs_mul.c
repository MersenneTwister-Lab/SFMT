#include "c99.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "mt19937ar.h"

#define NUM 10000

void test_add(void) {
    uint32_t i, j, k;
    uint64_t clo;
    double array1[NUM];
    double array2[NUM];
    double array3[NUM];
    double dsum = 0;

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	array1[j] = genrand_res53();
	array2[j] = genrand_res53();
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM; j++) {
	    array3[j] = array1[j] + array2[j];
	}
    }
    clo = clock() - clo;
    printf("ADD [0,1) %"PRIu64" AVE:%4"PRIu64"ms.\n",  (uint64_t)NUM * NUM,
	   (clo * 100) / CLOCKS_PER_SEC);
}

void test_add2(void) {
    uint32_t i, j, k;
    uint64_t clo;
    double array1[NUM];
    double array2[NUM];
    double array3[NUM];
    double dsum = 0;

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	array1[j] = genrand_res53() + 1.0;
	array2[j] = genrand_res53() + 1.0;
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM; j++) {
	    array3[j] = array1[j] + array2[j];
	}
    }
    clo = clock() - clo;
    printf("ADD [1,2) %"PRIu64" AVE:%4"PRIu64"ms.\n",  (uint64_t)NUM * NUM,
	   (clo * 100) / CLOCKS_PER_SEC);
}

void test_mul(void) {
    uint32_t i, j, k;
    uint64_t clo;
    double array1[NUM];
    double array2[NUM];
    double array3[NUM];
    double dsum = 0;

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	array1[j] = genrand_res53();
	array2[j] = genrand_res53();
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM; j++) {
	    array3[j] = array1[j] * array2[j];
	}
    }
    clo = clock() - clo;
    printf("MUL [0,1) %"PRIu64" AVE:%4"PRIu64"ms.\n",  (uint64_t)NUM * NUM,
	   (clo * 100) / CLOCKS_PER_SEC);
}

void test_mul2(void) {
    uint32_t i, j, k;
    uint64_t clo;
    double array1[NUM];
    double array2[NUM];
    double array3[NUM];
    double dsum = 0;

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	array1[j] = genrand_res53() + 1.0;
	array2[j] = genrand_res53() + 1.0;
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM; j++) {
	    array3[j] = array1[j] * array2[j];
	}
    }
    clo = clock() - clo;
    printf("MUL [1,2) %"PRIu64" AVE:%4"PRIu64"ms.\n",  (uint64_t)NUM * NUM,
	   (clo * 100) / CLOCKS_PER_SEC);
}

void test_mul3(void) {
    uint32_t i, j, k;
    uint64_t clo;
    uint64_t array1[NUM];
    double array2[NUM];
    double array3[NUM];
    double dsum = 0;

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	array1[j] = ((uint64_t)genrand_int32() << 32) | genrand_int32();
	array2[j] = genrand_res53();
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM; j++) {
	    array3[j] = array1[j] * array2[j];
	}
    }
    clo = clock() - clo;
    printf("MUL uint64_t * [0,1) %"PRIu64" AVE:%4"PRIu64"ms.\n",
	   (uint64_t)NUM * NUM, (clo * 100) / CLOCKS_PER_SEC);
}

void test_conv(void) {
    uint32_t i, j, k;
    uint64_t clo;
    uint64_t array1[NUM];
    double array2[NUM];
    double dsum = 0;

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	array1[j] = ((uint64_t)genrand_int32() << 32) | genrand_int32();
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM; j++) {
	    array2[j] = (double)array1[j];
	}
    }
    clo = clock() - clo;
    printf("conv uint64_t -> double %"PRIu64" AVE:%4"PRIu64"ms.\n",
	   (uint64_t)NUM * NUM, (clo * 100) / CLOCKS_PER_SEC);
}

void test_conv2(void) {
    uint32_t i, j, k;
    uint64_t clo;
    uint32_t array1[NUM];
    double array2[NUM];
    double two32 = pow(2.0, 32.0);

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	array1[j] = genrand_int32();
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM; j++) {
	    array2[j] = (double)array1[j];
	}
    }
    clo = clock() - clo;
    printf("conv uint32_t -> double %"PRIu64" AVE:%4"PRIu64"ms.\n",
	   (uint64_t)NUM * NUM, (clo * 100) / CLOCKS_PER_SEC);
}

void test_conv3(void) {
    uint32_t i, j, k;
    uint64_t clo;
    uint32_t array1[NUM];
    uint32_t array2[NUM];
    double array3[NUM];
    double two32 = pow(2.0, 32.0);

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	array1[j] = genrand_int32();
	array2[j] = genrand_int32();
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM; j++) {
	    array3[j] = (double)array1[j] * two32 + (double)array2[j];
	}
    }
    clo = clock() - clo;
    printf("conv uint32_t, uin32_t -> double %"PRIu64" AVE:%4"PRIu64"ms.\n",
	   (uint64_t)NUM * NUM, (clo * 100) / CLOCKS_PER_SEC);
}

#ifdef SSE2
void sse_add(void) {
    uint32_t i, j, k;
    uint64_t clo;
    __m128d array1[NUM / 2];
    __m128d array2[NUM / 2];
    __m128d array3[NUM / 2];
    double *dp1 = (double *)array1;
    double *dp2 = (double *)array2;
    double *dp3 = (double *)array3;

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	dp1[j] = genrand_res53();
	dp2[j] = genrand_res53();
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM / 2; j++) {
	    array3[j] = _mm_add_pd(array1[j], array2[j]);
	}
    }
    clo = clock() - clo;
    printf("SSE2 ADD [0,1) %"PRIu64" AVE:%4"PRIu64"ms.\n",  (uint64_t)NUM * NUM,
	   (clo * 100) / CLOCKS_PER_SEC);
}

void sse_add2(void) {
    uint32_t i, j, k;
    uint64_t clo;
    __m128d array1[NUM / 2];
    __m128d array2[NUM / 2];
    __m128d array3[NUM / 2];
    double *dp1 = (double *)array1;
    double *dp2 = (double *)array2;
    double *dp3 = (double *)array3;

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	dp1[j] = genrand_res53() + 1.0;
	dp2[j] = genrand_res53() + 1.0;
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM / 2; j++) {
	    array3[j] = _mm_add_pd(array1[j], array2[j]);
	}
    }
    clo = clock() - clo;
    printf("SSE2 ADD [1,2) %"PRIu64" AVE:%4"PRIu64"ms.\n",  (uint64_t)NUM * NUM,
	   (clo * 100) / CLOCKS_PER_SEC);
}

void sse_mul(void) {
    uint32_t i, j, k;
    uint64_t clo;
    __m128d array1[NUM / 2];
    __m128d array2[NUM / 2];
    __m128d array3[NUM / 2];
    double *dp1 = (double *)array1;
    double *dp2 = (double *)array2;
    double *dp3 = (double *)array3;

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	dp1[j] = genrand_res53();
	dp2[j] = genrand_res53();
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM / 2; j++) {
	    array3[j] = _mm_mul_pd(array1[j], array2[j]);
	}
    }
    clo = clock() - clo;
    printf("SSE2 MUL [0,1) %"PRIu64" AVE:%4"PRIu64"ms.\n",  (uint64_t)NUM * NUM,
	   (clo * 100) / CLOCKS_PER_SEC);
}

void sse_mul2(void) {
    uint32_t i, j, k;
    uint64_t clo;
    __m128d array1[NUM / 2];
    __m128d array2[NUM / 2];
    __m128d array3[NUM / 2];
    double *dp1 = (double *)array1;
    double *dp2 = (double *)array2;
    double *dp3 = (double *)array3;

    init_genrand(1234);
    for (j = 0; j < NUM; j++) {
	dp1[j] = genrand_res53() + 1.0;
	dp2[j] = genrand_res53() + 1.0;
    }
    clo = clock();
    for (k = 0; k < NUM; k++) {
	for (j = 0; j < NUM / 2; j++) {
	    array3[j] = _mm_mul_pd(array1[j], array2[j]);
	}
    }
    clo = clock() - clo;
    printf("SSE2 MUL [1,2) %"PRIu64" AVE:%4"PRIu64"ms.\n",  (uint64_t)NUM * NUM,
	   (clo * 100) / CLOCKS_PER_SEC);
}

#endif /* SSE2 */

int main(void) {
    test_add();
    test_add2();
    test_mul();
    test_mul2();
    test_mul3();
    test_conv();
    test_conv2();
    test_conv3();
#ifdef SSE2
    sse_add();
    sse_add2();
    sse_mul();
    sse_mul2();
#endif
    return 0;
}
