#include "c99.h"
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <string.h>

#define QUOTE(str) QUOTE_HELPER(str)
#define QUOTE_HELPER(str) # str
#include QUOTE(RANDOM)

#define KAISU 10000000
#ifdef __ppc__
vector unsigned int dummy;
#else
__m128i dummy;
#endif
static uint32_t array[700 * 100];
int main(int argc, char *argv[]) {
    uint32_t i, j;
    clock_t clo;
    clock_t min = INT_MAX;
    uint32_t randoms;
    uint32_t block;
    bool verbose = false;

    if ((argc >= 2) && (strncmp(argv[1],"-v",2) == 0)) {
	verbose = true;
    }
    block = get_onetime_rnds();
    randoms = (KAISU / block) * block;
    init_gen_rand(1234);
    if (verbose) {
	//printf("init states\n");
	//print_state(stdout);
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
	for (i = 0; i < randoms / block / 20; i++) {
	    fill_array_block(array, 20);
	}
	clo = clock() - clo;
	if (clo < min) {
	    min = clo;
	}
	//sum += clo;
    }
    //sum /= 1000;
    printf("randoms %d\n", randoms);
    printf("block %d\n", block);
    printf("randoms / block %d\n", randoms / block);
    printf("min %.0f\n", (double)min);
    printf("BURST:%.0f", (double)min * 1000/ CLOCKS_PER_SEC);
    printf(" ms and %u randoms = %.3f ms per %drandoms\n",
	   randoms, (double)min * 1000 * KAISU / CLOCKS_PER_SEC / randoms,
	   KAISU);
    min = UINT_MAX;
    //sum = 0;
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < randoms; j++) {
	    gen_rand();
	}
	clo = clock() - clo;
	if (clo < min) {
	    min = clo;
	}
	//sum += clo;
    }
    //sum /= 1000;
    printf("randoms %d\n", randoms);
    printf("min %.0f\n", (double)min);
    printf("SEQUE:%.0f", (double)min * 1000 / CLOCKS_PER_SEC);
    printf(" ms and %u randoms = %.3f ms per %drandoms\n",
	   randoms, (double)min * 1000 * KAISU / CLOCKS_PER_SEC / randoms,
	   KAISU);
    return 0;
}
