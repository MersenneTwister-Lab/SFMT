/**
 * @file  test64.c
 * @brief test program for 64-bit output of SFMT.
 *
 * @author Mutsuo Saito (Hiroshima-univ)
 *
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */

#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#if defined(__ppc__) && !defined(BIG_ENDIAN64)
  #define BIG_ENDIAN64
#endif

#include "SFMT.c"

#define BLOCK_SIZE 100000
#define BLOCK_SIZE64 50000
#define COUNT 1000

void check32(void);
void speed32(void);
void check64(void);
void speed64(void);

#if defined(ALTIVEC)
static vector unsigned int array1[BLOCK_SIZE / 4];
static vector unsigned int array2[10000 / 4];
#elif defined(SSE2)
static __m128i array1[BLOCK_SIZE / 4];
static __m128i array2[10000 / 4];
#else
static uint64_t array1[BLOCK_SIZE / 4][2];
static uint64_t array2[10000 / 4][2];
#endif

void check64(void) {
    int i;
    uint64_t *array64;
    uint64_t *array64_2;
    uint64_t r;
    uint32_t ini[] = {5, 4, 3, 2, 1};

    array64 = (uint64_t *)array1;
    array64_2 = (uint64_t *)array2;
    if (get_min_array_size64() > 5000) {
	printf("array size too small!\n");
	exit(1);
    }
    printf("generated randoms\n");
    /* 64 bit generation */
    init_by_array(ini, 5);
    fill_array64(array64, 5000);
    fill_array64(array64_2, 5000);
    init_by_array(ini, 5);
    for (i = 0; i < 5000; i++) {
	if (i < 1000) {
	    printf("%20"PRIu64" ", array64[i]);
	    if (i % 3 == 2) {
		printf("\n");
	    }
	}
	r = gen_rand64();
	if (r != array64[i]) {
	    printf("\nmismatch at %d array64:%"PRIx64" gen:%"PRIx64"\n", 
		   i, array64[i], r);
	    exit(1);
	}
    }
    printf("\n");
    for (i = 0; i < 700; i++) {
	r = gen_rand64();
	if (r != array64_2[i]) {
	    printf("\nmismatch at %d array64_2:%"PRIx64" gen:%"PRIx64"\n", 
		   i, array64_2[i], r);
	    exit(1);
	}
    }
}

void speed64(void) {
    int i, j;
    uint64_t clo;
    uint64_t min = LONG_MAX;
    uint64_t *array64 = (uint64_t *)array1;

    if (get_min_array_size64() > BLOCK_SIZE64) {
	printf("array size too small!\n");
	exit(1);
    }
    /* 64 bit generation */
    init_gen_rand(1234);
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < COUNT; j++) {
	    fill_array64(array64, BLOCK_SIZE64);
	}
	clo = clock() - clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("64 bit BLOCK:%.0f", (double)min * 1000/ CLOCKS_PER_SEC);
    printf("ms for %u randoms generation\n",
	   BLOCK_SIZE64 * COUNT);
    min = LONG_MAX;
    init_gen_rand(1234);
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < BLOCK_SIZE64 * COUNT; j++) {
	    gen_rand64();
	}
	clo = clock() - clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("64 bit SEQUE:%.0f", (double)min * 1000 / CLOCKS_PER_SEC);
    printf("ms for %u randoms generation\n",
	   BLOCK_SIZE64 * COUNT);
}

int main(int argc, char *argv[]) {
    int i;
    int speed = 0;
    int bit64 = 0;

    for (i = 1; i < argc; i++) {
	if (strncmp(argv[1],"-s",2) == 0) {
	    speed = 1;
	}
	if (strncmp(argv[1],"-b64",4) == 0) {
	    bit64 = 1;
	}
    }
    if (speed + bit64 == 0) {
	printf("usage:\n%s [-s | -b32 | -b64]\n", argv[0]);
	return 0;
    }
    if (speed) {
	speed64();
    }
    if (bit64) {
	check64();
    }
    return 0;
}
