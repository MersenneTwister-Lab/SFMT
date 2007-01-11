/**
 * @file  test64.c
 * @brief test program for 64-bit output of SFMTp.
 *
 * @author Mutsuo Saito (Hiroshima-univ)
 * @date 2007-01-10
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

#if defined(SSE2)
#include <emmintrin.h>
#include "SFMTp-sse2.c"
#elif defined(ALTIVEC)
#include "SFMTp-alti64.c"
#elif defined(BIG)
#include "SFMTp-big64.c"
#else
#include "SFMTp.c"
#endif

#define BLOCK_SIZE 50000
#define COUNT 2000

void check64(void);
void speed64(void);

#if defined(SSE2)
static __m128i array1[BLOCK_SIZE / 2];
static __m128i array2[700 / 2];
#elif defined(ALTIVEC)
static vector unsigned int array1[BLOCK_SIZE / 2];
static vector unsigned int array2[700 / 2];
#else
static uint64_t array1[BLOCK_SIZE];
static uint64_t array2[700];
#endif

void check64(void) {
    int i;
    uint64_t *array64;
    uint64_t *array64_2;
    uint64_t r;
    uint32_t ini[] = {5, 4, 3, 2, 1};

    array64 = (uint64_t *)array1;
    array64_2 = (uint64_t *)array2;
    printf("generated randoms\n");
    /* 64 bit generation */
    init_by_array(ini, 5);
    fill_array64(array64, 1000);
    fill_array64(array64_2, 700);
    init_by_array(ini, 5);
    for (i = 0; i < 1000; i++) {
	printf("%20"PRIu64" ", array64[i]);
	if (i % 3 == 2) {
	    printf("\n");
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

    /* 64 bit generation */
    init_gen_rand(1234);
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < COUNT; j++) {
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
    init_gen_rand(1234);
    for (i = 0; i < 10; i++) {
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
}

int main(int argc, char *argv[]) {
    int speed = 0;

    if ((argc >= 2) && (strncmp(argv[1],"-s",2) == 0)) {
	speed = 1;
    }
    if (speed) {
	speed64();
    } else {
	check64();
    }
    return 0;
}
