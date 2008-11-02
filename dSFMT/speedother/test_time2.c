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
    init(1234);
    min = LONG_MAX;
    sum = 0;
    total = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < NUM_RANDS * TIC_COUNT; j++) {
	    r = generate();
	    total += r;
	}
	clo = clock() - clo;
	sum += clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("consumed time for generating %u randoms.\n", NUM_RANDS * TIC_COUNT);
    printf("double MIN:%4"PRIu64"ms.\n", (min * 1000) / CLOCKS_PER_SEC);
    printf("       AVE:%4"PRIu64"ms.\n",  (sum * 100) / CLOCKS_PER_SEC);
    printf("total = %lf\n", total);
    return 0;
}
