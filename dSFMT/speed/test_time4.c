#include "c99.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>

#define NUM_RANDS 50000
#define TIC_MAG 1
#define TIC_COUNT 2000

#ifdef __ppc__
w128_t dummy[NUM_RANDS / 2 + 1];
#else
w128_t dummy[NUM_RANDS / 2 + 1];
#endif
#ifdef __GNUC__
void check_d32(void) __attribute__((noinline));
void check_d64(void) __attribute__((noinline));
void test_d32(void) __attribute__((noinline));
void test_d64(void) __attribute__((noinline));
void test_seq_d32(void) __attribute__((noinline));
void test_seq_d64(void) __attribute__((noinline));
inline static double to_d32(uint32_t v) __attribute__((always_inline));
inline static double genrand_d32(void) __attribute__((always_inline));
inline static double to_d64(uint32_t x, uint32_t y) 
    __attribute__((always_inline));
inline static double genrand_d64(void) __attribute__((always_inline));
#else
void check_d32(void);
void check_d64(void);
void test_d32(void);
void test_d64(void);
void test_seq_d32(void);
void test_seq_d64(void);
inline static double to_d32(uint32_t v);
inline static double genrand_d32(uint32_t v);
inline static double to_d64(uint32_t x, uint32_t y);
inline static double genrand_d64(uint32_t v);
#endif

/** generates a random number on [0,1)-real-interval */
inline static double to_d32(uint32_t v) {
    return v * (1.0/4294967296.0); 
    /* divided by 2^32 */
}

/** generates a random number on [0,1)-real-interval */
inline static double genrand_d32(void) {
    return to_d32(genrand_int32());
}
/** generates a random number on [0,1) with 53-bit resolution*/
inline static double to_d64(uint32_t x, uint32_t y) { 
    return ((uint64_t)y << 32 | x) * (1.0/18446744073709551616.0);
}

/** generates a random number on [0,1) with 53-bit resolution*/
inline static double genrand_d64(void) {
    uint32_t x;
    x = genrand_int32();
    return to_d64(x, genrand_int32());
} 

void check_d32(void) {
    int i;
    uint32_t *array = (uint32_t *)dummy;
    double array2[5000];
    double r;

    init_gen_rand(1234);
    printf("generated randoms [0,1)\n");
    init_gen_rand(1234);
    fill_array_int32(array, 5000);
    for (i = 0; i < 5000; i++) {
	array2[i] = to_d32(array[i]);
    }
    init_gen_rand(1234);
    for (i = 0; i < 5000; i++) {
	r = genrand_d32();
	if (r != array2[i]) {
	    printf("\n[0,1) mismatch i = %d: r = %1.20lf, array = %1.20lf\n",
		   i, r, array2[i]);
	    exit(1);
	}
	if (i < 1000) {
	    printf("%1.20lf ", array2[i]);
	    if (i % 3 == 2) {
		printf("\n");
	    }
	}
    }
    printf("\n");
}

void check_d64(void) {
    int i, j;
    uint32_t *array = (uint32_t *)dummy;
    double array2[5000];
    double r;

    init_gen_rand(1234);
    printf("generated randoms [0,1)\n");
    init_gen_rand(1234);
    fill_array_int32(array, 10000);
    for (i = 0, j = 0; i < 5000; i++) {
	array2[i] = to_d64(array[j], array[j+1]);
	j += 2;
    }
    init_gen_rand(1234);
    for (i = 0; i < 5000; i++) {
	r = genrand_d64();
	if (r != array2[i]) {
	    printf("\n[0,1) mismatch i = %d: r = %1.20lf, array = %1.20lf\n",
		   i, r, array2[i]);
	    exit(1);
	}
	if (i < 1000) {
	    printf("%1.20lf ", array2[i]);
	    if (i % 3 == 2) {
		printf("\n");
	    }
	}
    }
    printf("\n");
}

void test_d32(void) {
    uint32_t i, j, k;
    uint64_t clo;
    uint64_t sum;
    uint64_t min;
    uint32_t *array = (uint32_t *)dummy;
    double array2[NUM_RANDS];

    init_gen_rand(1234);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    fill_array_int32(array, NUM_RANDS);
	    for (k = 0; k < NUM_RANDS; k++) {
		array2[k] = to_d32(array[k]);
	    }
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("32bit BLOCK [0, 1) AVE:%4"PRIu64"ms.\n",
	   (sum * 100) / CLOCKS_PER_SEC);
}

void test_d64(void) {
    uint32_t i, j, k, l;
    uint64_t clo;
    uint64_t sum;
    uint64_t min;
    uint32_t *array = (uint32_t *)dummy;
    double array2[NUM_RANDS];

    init_gen_rand(1234);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    fill_array_int32(array, NUM_RANDS * 2);
	    for (k = 0, l = 0; k < NUM_RANDS; k++) {
		array2[k] = to_d64(array[l], array[l+1]);
		l += 2;
	    }
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("64bit BLOCK [0, 1) AVE:%4"PRIu64"ms.\n",
	   (sum * 100) / CLOCKS_PER_SEC);
}

void test_seq_d32(void) {
    uint32_t i, j;
    uint64_t clo;
    uint64_t sum;
    uint64_t min;
    double r = 0;

    init_gen_rand(1234);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT * NUM_RANDS; j++) {
	    r += genrand_d32();
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("32bit SEQ [0, 1) AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %f\n", r);
}

void test_seq_d64(void) {
    uint32_t i, j;
    uint64_t clo;
    uint64_t sum;
    uint64_t min;
    double r = 0;

    init_gen_rand(1234);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT * NUM_RANDS; j++) {
	    r += genrand_d64();
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("64bit SEQ [0, 1) AVE:%4"PRIu64"ms.\n",
	   (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", r);
}

int main(int argc, char *argv[]) {
    if ((argc >= 2) && (strncmp(argv[1],"-v",2) == 0)) {
	check_d32();
	check_d64();
    }
    printf("consumed time for generating %u randoms.\n", NUM_RANDS * TIC_COUNT);
    test_d32();
    test_d64();
    test_seq_d32();
    test_seq_d64();
    return 0;
}
