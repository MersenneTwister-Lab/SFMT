/** 
 * @file  sfmt.c
 * @brief SIMD oriented Fast Mersenne Twister
 *
 * @author Mutsuo Saito (Hiroshima-univ)
 * @date 2006-08-29
 *
 * Copyright (C) 2006 Mutsuo Saito. All rights reserved.
 * @see LICENSE
 */

#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "sfmt.h"

#define BLOCK_SIZE 100000
#define COUNT 1000

int main(int argc, char *argv[]) {
    int i, j;
    uint64_t clo;
    uint64_t min = LONG_MAX;
    uint32_t array[BLOCK_SIZE];
    uint32_t array2[700];
    int verbose = 0;
    uint32_t r;

    if ((argc >= 2) && (strncmp(argv[1],"-v",2) == 0)) {
	verbose = 1;
    }
    if (verbose) {
	printf("generated randoms\n");
	init_gen_rand(1234);
	fill_array(array, 1000);
	fill_array(array2, 700);
	init_gen_rand(1234);
	for (i = 0; i < 1000; i++) {
	    printf("%10u ", array[i]);
	    if (i % 5 == 4) {
		printf("\n");
	    }
	    r = gen_rand();
	    if (r != array[i]) {
		printf("mismatch at %d array:%u gen:%u", i, array[i], r);
		exit(1);
	    }
	}
	for (i = 0; i < 700; i++) {
	    r = gen_rand();
	    if (r != array2[i]) {
		printf("mismatch at %d array2:%u gen:%u", i, array2[i], r);
		exit(1);
	    }
	}
    }
    init_gen_rand(1234);
    for (i = 0; i < 1; i++) {
	clo = clock();
	for (i = 0; i < COUNT; i++) {
	    fill_array(array, BLOCK_SIZE);
	}
	clo = clock() - clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("BLOCK:%.0f", (double)min * 1000/ CLOCKS_PER_SEC);
    printf("ms for %u randoms generation\n",
	   BLOCK_SIZE * COUNT);
    min = LONG_MAX;
    init_gen_rand(1234);
    for (i = 0; i < 1; i++) {
	clo = clock();
	for (j = 0; j < BLOCK_SIZE * COUNT; j++) {
	    gen_rand();
	}
	clo = clock() - clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("SEQUE:%.0f", (double)min * 1000 / CLOCKS_PER_SEC);
    printf("ms for %u randoms generation\n",
	   BLOCK_SIZE * COUNT);
    return 0;
}
