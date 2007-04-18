#include "c99.h"
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <string.h>

#define NUM_RANDS 50000
#define TIC_MAG 1
#define TIC_COUNT 2000

#if defined(__ALTIVEC__)
vector unsigned int dummy[NUM_RANDS / 2 + 1];
#elif defined(__SSE2__)
__m128i dummy[NUM_RANDS / 2 + 1];
#else
double dummy[NUM_RANDS + 2];
#endif
int main(int argc, char *argv[]) {
    uint32_t i, j, k;
    uint64_t clo;
    uint64_t sum;
    uint64_t min;
    double *array = (double *)dummy;
    double r;
    double total = 0;
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
    //printf("BLOCK MIN:%4llums.\n", (min * 1000) / CLOCKS_PER_SEC);
    printf("BLOCK AVE:%4"PRIu64"ms.\n",  (sum * 100) / CLOCKS_PER_SEC);
    min = LONG_MAX;
    sum = 0;
    r = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		r += gen_rand();
	    }
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    total = r;
    //printf("SEQUE MIN:%4llums.\n", (min * 1000) / CLOCKS_PER_SEC);
    printf("SEQ 1 AVE:%4"PRIu64"ms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
    min = LONG_MAX;
    sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    for (k = 0; k < NUM_RANDS; k++) {
		array[k] = gen_rand();
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
    //printf("SEQUE MIN:%4llums.\n", (min * 1000) / CLOCKS_PER_SEC);
    printf("SEQ 2 AVE:%4"PRIu64"ms.\n", (sum * 100)  / CLOCKS_PER_SEC);
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
    //printf("ADD   MIN:%4llums.\n", (min * 1000) / CLOCKS_PER_SEC);
    printf("ADD   AVE:%4"PRIu64"ms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
    return 0;
}
