#include "c99.h"
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <string.h>

#define QUOTE(str) QUOTE_HELPER(str)
#define QUOTE_HELPER(str) # str
//#include QUOTE(RANDOM)

#define NUM_RANDS 100000
//#define KAISU (624 * 128)
#define TIC_MAG 1
#define TIC_COUNT 1000
//#define TIC_COUNT (313 * 4)

#ifdef __ppc__
vector unsigned int dummy[NUM_RANDS / 4 + 1];
#else
__m128i dummy[NUM_RANDS / 4 + 1];
#endif
int main(int argc, char *argv[]) {
    uint32_t i, j;
    uint64_t clo;
    unsigned long long min = LONG_MAX;
    //unsigned long long max = 0;
    uint32_t *array = (uint32_t *)dummy;
    uint32_t r;
    bool verbose = false;

    if ((argc >= 2) && (strncmp(argv[1],"-v",2) == 0)) {
	verbose = true;
    }
    array = (uint32_t *)dummy;
    init_gen_rand(1234);
    if (verbose) {
	printf("generated randoms\n");
	init_gen_rand(1234);
	fill_array(array, 5000);
	init_gen_rand(1234);
	for (i = 0; i < 5000; i++) {
	    r = gen_rand();
	    if (r != array[i]) {
		printf("\nmismatch i = %d: r = %x, array = %x\n",
		       i, r, array[i]);
		return -1;
	    }
	    if (i < 1000) {
		printf("%10u ", array[i]);
		if (i % 5 == 4) {
		    printf("\n");
		}
	    }
	}
    }
    init_gen_rand(1234);
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (i = 0; i < TIC_COUNT; i++) {
	    fill_array(array, NUM_RANDS);
	}
	clo = clock() - clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("BURST:%.0f", (double)min * 1000/ CLOCKS_PER_SEC);
    printf("ms for %u randoms.\n",
	   NUM_RANDS * TIC_COUNT);
    min = LONG_MAX;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < NUM_RANDS * TIC_COUNT; j++) {
	    gen_rand();
	}
	clo = clock() - clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("SEQUE:%.0f", (double)min * 1000 / CLOCKS_PER_SEC);
    printf("ms for %u randoms.\n",
	   NUM_RANDS * TIC_COUNT);
    return 0;
}
