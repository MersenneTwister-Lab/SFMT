#include "c99.h"
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <string.h>

#define KAISU (624 * 128)
#define TIC_MAG 1
#define TIC_COUNT (313 * 4)

uint32_t dummy[KAISU / 4 + 1][4];

int main(int argc, char *argv[]) {
    uint32_t i, j;
    uint64_t clo;
    unsigned long long min = LONG_MAX;
    uint32_t randoms;
    uint32_t block;
    uint32_t *array;
    bool verbose = false;

    if ((argc >= 2) && (strncmp(argv[1],"-v",2) == 0)) {
	verbose = true;
    }
    array = (uint32_t *)dummy;
    block = get_onetime_rnds();
    randoms = (KAISU / block) * block;
    init_gen_rand(1234);
    if (verbose) {
	printf("generated randoms\n");
	fill_array_block(array, 1000 / block + 1);
	for (i = 0; i < 1000; i++) {
	    printf("%10u ", array[i]);
	    if (i % 5 == 4) {
		printf("\n");
	    }
	}
    }
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (i = 0; i < TIC_COUNT; i++) {
	    fill_array_block(array, randoms / block);
	}
	clo = clock() - clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("BURST:%.0f", (double)min * 1000/ CLOCKS_PER_SEC);
    printf("ms and %u randoms = %.3fs per %drandoms\n",
	   randoms * TIC_COUNT, 
	   (double)min / CLOCKS_PER_SEC,
	   randoms * TIC_COUNT);
    min = LONG_MAX;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < randoms * TIC_COUNT; j++) {
	    gen_rand();
	}
	clo = clock() - clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("SEQUE:%.0f", (double)min * 1000 / CLOCKS_PER_SEC);
    printf("ms and %u randoms = %.3fs per %drandoms\n",
	   randoms * TIC_COUNT,
	   (double)min / CLOCKS_PER_SEC,
	   randoms * TIC_COUNT);
    return 0;
}
