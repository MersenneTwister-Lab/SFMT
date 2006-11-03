#include "c99.h"
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <string.h>

#define NUM_RANDS 50000
#define TIC_MAG 1
#define TIC_COUNT 2000

int main(int argc, char *argv[]) {
    uint32_t i, j;
    uint64_t clo;
    uint64_t sum;
    uint64_t min;
    double r;
    double total;
    bool verbose = false;

    if ((argc >= 2) && (strncmp(argv[1],"-v",2) == 0)) {
	verbose = true;
    }
    init_genrand(1234);
    min = LONG_MAX;
    sum = 0;
    total = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < NUM_RANDS * TIC_COUNT; j++) {
	    r = genrand_real1();
	    total += r;
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("consumed time for generating %u randoms.\n", NUM_RANDS * TIC_COUNT);
    printf("REAL1 MIN:%4lldms.\n", (min * 1000) / CLOCKS_PER_SEC);
    printf("      AVE:%4lldms.\n",  (sum * 100) / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
    min = LONG_MAX;
    r = 0;
    sum = 0;
    total = 0;
    init_genrand(1234);
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < NUM_RANDS * TIC_COUNT; j++) {
	    r = genrand_real2();
	    total += r;
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("REAL2 MIN:%4lldms.\n", (min * 1000) / CLOCKS_PER_SEC);
    printf("      AVE:%4lldms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
    min = LONG_MAX;
    r = 0;
    sum = 0;
    total = 0;
    init_genrand(1234);
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < NUM_RANDS * TIC_COUNT; j++) {
	    r = genrand_res53();
	    total += r;
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("REAL53 MIN:%4lldms.\n", (min * 1000) / CLOCKS_PER_SEC);
    printf("       AVE:%4lldms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
    min = LONG_MAX;
    r = 0;
    sum = 0;
    total = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < NUM_RANDS * TIC_COUNT; j++) {
	    total += 1.0;
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("ADD   MIN:%4lldms.\n", (min * 1000) / CLOCKS_PER_SEC);
    printf("      AVE:%4lldms.\n", (sum * 100)  / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
    return 0;
}
