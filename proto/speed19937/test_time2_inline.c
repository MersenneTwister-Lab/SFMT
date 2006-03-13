#include "c99.h"
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define QUOTE(str) QUOTE_HELPER(str)
#define QUOTE_HELPER(str) # str
#include QUOTE(RANDOM)

#define KAISU 100000
#define TIC_MAG 1
#define TIC_COUNT 100

#ifdef __ppc__
vector unsigned int dummy[KAISU * TIC_COUNT / 4 + 1];
#else
__m128i dummy[KAISU * TIC_COUNT / 4 + 1];
#endif
int main(int argc, char *argv[]) {
    uint32_t i, j;
    uint64_t clo;
    unsigned long long t;
    //unsigned long long max = 0;
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
    clo = clock();
    fill_array_block(array, randoms * TIC_COUNT / block);
    t = clock() - clo;
    //printf("randoms %d\n", randoms * TIC_COUNT);
    //printf("block %d\n", block);
    //printf("randoms / block %d\n", randoms / block);
    //printf("min %.0f\n", (double)min);
    //printf("max %.0f\n", (double)max);
    printf("BURST:%.0f", (double)t * 1000/ CLOCKS_PER_SEC);
    printf(" ms and %u randoms = %.3f ms per %drandoms\n",
	   randoms * TIC_COUNT, 
	   (double)t * 1000 * KAISU / CLOCKS_PER_SEC / randoms / TIC_COUNT,
	   KAISU * TIC_COUNT);
    sleep(1);
    clo = clock();
    for (j = 0; j < randoms * TIC_COUNT; j++) {
	gen_rand();
    }
    t = clock() - clo;
    //sum /= 1000;
    //printf("randoms %d\n", randoms * TIC_COUNT);
    //printf("min %.0f\n", (double)min);
    //printf("max %.0f\n", (double)max);
    printf("SEQUE:%.0f", (double)t * 1000 / CLOCKS_PER_SEC);
    printf(" ms and %u randoms = %.3f ms per %drandoms\n",
	   randoms * TIC_COUNT,
	   (double)t * 1000 * KAISU / CLOCKS_PER_SEC / randoms / TIC_COUNT,
	   KAISU * TIC_COUNT);
    return 0;
}
