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

static vector unsigned int array1[BLOCK_SIZE / 2];
static vector unsigned int array2[700 / 4];
int main(int argc, char *argv[]) {
    int i, j;
    uint64_t clo;
    uint64_t min = LONG_MAX;
    uint32_t *array32 = (uint32_t *)array1;
    uint32_t *array32_2 = (uint32_t *)array2;
    uint64_t *array64 = (uint64_t *)array1;
    uint64_t *array64_2 = (uint64_t *)array2;
    uint32_t ini[] = {5, 4, 3, 2, 1};
    int verbose = 0;
    uint32_t r32;
    uint64_t r64;

    if ((argc >= 2) && (strncmp(argv[1],"-v",2) == 0)) {
	verbose = 1;
    }
    if (verbose) {
	printf("generated randoms\n");
	/* 32 bit generation */
	init_gen_rand(1234);
	fill_array32(array32, 1000);
	fill_array32(array32_2, 700);
	init_gen_rand(1234);
	for (i = 0; i < 1000; i++) {
	    printf("%10u ", array32[i]);
	    if (i % 5 == 4) {
		printf("\n");
	    }
	    r32 = gen_rand32();
	    if (r32 != array32[i]) {
		printf("mismatch at %d array32:%u gen:%u", 
		       i, array32[i], r32);
		exit(1);
	    }
	}
	for (i = 0; i < 700; i++) {
	    r32 = gen_rand32();
	    if (r32 != array32_2[i]) {
		printf("mismatch at %d array32_2:%u gen:%u", 
		       i, array32_2[i], r32);
		exit(1);
	    }
	}
	/* 64 bit generation */
	init_gen_rand(1234);
	fill_array64(array64, 500);
	fill_array64(array64_2, 350);
	init_gen_rand(1234);
	for (i = 0; i < 500; i++) {
	    printf("%20llu ", array64[i]);
	    if (i % 3 == 2) {
		printf("\n");
	    }
	    r64 = gen_rand64();
	    if (r64 != array64[i]) {
		printf("\nmismatch at %d array64:%llx gen:%llx\n", 
		       i, array64[i], r64);
		exit(1);
	    }
	}
	for (i = 0; i < 350; i++) {
	    r64 = gen_rand64();
	    if (r64 != array64_2[i]) {
		printf("\nmismatch at %d array64_2:%llx gen:%llx\n", 
		       i, array64_2[i], r64);
		exit(1);
	    }
	}
	printf("\n");
    }
    /* 32 bit generation */
    init_gen_rand(1234);
    for (i = 0; i < 1; i++) {
	clo = clock();
	for (i = 0; i < COUNT; i++) {
	    fill_array32(array32, BLOCK_SIZE);
	}
	clo = clock() - clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("32 bit BLOCK:%.0f", (double)min * 1000/ CLOCKS_PER_SEC);
    printf("ms for %u randoms generation\n",
	   BLOCK_SIZE * COUNT);
    min = LONG_MAX;
    init_gen_rand(1234);
    for (i = 0; i < 1; i++) {
	clo = clock();
	for (j = 0; j < BLOCK_SIZE * COUNT; j++) {
	    gen_rand32();
	}
	clo = clock() - clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("32 bit SEQUE:%.0f", (double)min * 1000 / CLOCKS_PER_SEC);
    printf("ms for %u randoms generation\n",
	   BLOCK_SIZE * COUNT);
    /* 64 bit generation */
    init_by_array(ini, 5);
    for (i = 0; i < 1; i++) {
	clo = clock();
	for (i = 0; i < COUNT; i++) {
	    fill_array64(array64, BLOCK_SIZE);
	}
	clo = clock() - clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("64 bit BLOCK:%.0f", (double)min * 1000/ CLOCKS_PER_SEC);
    printf("ms for %u randoms generation\n",
	   BLOCK_SIZE * COUNT);
    min = LONG_MAX;
    init_by_array(ini, 5);
    for (i = 0; i < 1; i++) {
	clo = clock();
	for (j = 0; j < BLOCK_SIZE * COUNT; j++) {
	    gen_rand64();
	}
	clo = clock() - clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("64 bit SEQUE:%.0f", (double)min * 1000 / CLOCKS_PER_SEC);
    printf("ms for %u randoms generation\n",
	   BLOCK_SIZE * COUNT);
    return 0;
}
