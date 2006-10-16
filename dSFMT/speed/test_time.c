#include "c99.h"
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <string.h>

#define NUM_RANDS 50000
#define TIC_MAG 1
#define TIC_COUNT 200

#ifdef __ppc__
vector unsigned int dummy[NUM_RANDS / 2 + 1];
#else
__m128i dummy[NUM_RANDS / 2 + 1];
#endif
int main(int argc, char *argv[]) {
    uint32_t i, j;
    uint64_t clo;
    uint64_t sum;
    uint64_t min;
    double *array = (double *)dummy;
    double r;
    uint64_t *i64 = (uint64_t *)&r;
    int xor = 0;
    bool verbose = false;

    if ((argc >= 2) && (strncmp(argv[1],"-v",2) == 0)) {
	verbose = true;
    }
    init_gen_rand(1234);
    if (verbose) {
	printf("generated randoms\n");
	init_gen_rand(1234);
	fill_array(array, 5000);
	init_gen_rand(1234);
	for (i = 0; i < 5000; i++) {
	    r = gen_rand();
	    if (r != array[i]) {
		printf("\nmismatch i = %d: r = %1.10lf, array = %1.10lf\n",
		       i, r, array[i]);
		return -1;
	    }
	    if (i < 1000) {
		printf("%1.10f ", array[i]);
		if (i % 5 == 4) {
		    printf("\n");
		}
	    }
	}
    }
    init_gen_rand(1234);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    fill_array(array, NUM_RANDS);
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("consumed time for generating %u randoms.\n", NUM_RANDS * TIC_COUNT);
    printf("BLOCK MIN:%4lldms.\n", (min * 1000) / CLOCKS_PER_SEC);
    printf("      AVE:%4lldms.\n",  (sum * 100) / CLOCKS_PER_SEC);
    min = LONG_MAX;
    r = 0;
    sum = 0;
    xor = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < NUM_RANDS * TIC_COUNT; j++) {
	    r = gen_rand();
	    xor ^= *i64;
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("SEQUE MIN:%4lldms.\n", (min * 1000) / CLOCKS_PER_SEC);
    printf("      AVE:%4lldms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("r = %d\n", xor);
    return 0;
}
