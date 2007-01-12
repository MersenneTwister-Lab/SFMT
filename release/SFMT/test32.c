/**
 * @file  test32.c
 * @brief test program for 32-bit output of SFMT19937.
 *
 * @author Mutsuo Saito (Hiroshima-univ)
 * @date 2007-01-11
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

#if defined(ALTIVEC)
#include "SFMTp-alti32.c"
#elif defined(SSE2)
#include <emmintrin.h>
#include "SFMTp-sse2.c"
#else
#include "SFMTp.c"
#endif

#define BLOCK_SIZE 100000
#define COUNT 1000

void check32(void);
void speed32(void);

#if defined(ALTIVEC)
static vector unsigned int array1[BLOCK_SIZE / 4];
static vector unsigned int array2[10000 / 4];
#elif defined(SSE2)
static __m128i array1[BLOCK_SIZE / 4];
static __m128i array2[10000 / 4];
#else
static uint32_t array1[BLOCK_SIZE / 4][4];
static uint32_t array2[10000 / 4][4];
#endif

void check32(void) {
    int i;
    uint32_t *array32 = (uint32_t *)array1;
    uint32_t *array32_2 = (uint32_t *)array2;
    uint32_t ini[4] = {0x1234, 0x5678, 0x9abc, 0xdef0};
    uint32_t r32;

    printf("%s\ngenerated randoms\n", get_idstring());
    printf("init_gen_rand__________\n");
    /* 32 bit generation */
    init_gen_rand(1234);
    fill_array32(array32, 10000);
    fill_array32(array32_2, 10000);
    init_gen_rand(1234);
    for (i = 0; i < 10000; i++) {
	if (i < 1000) {
	    printf("%10u ", array32[i]);
	    if (i % 5 == 4) {
		printf("\n");
	    }
	}
	r32 = gen_rand32();
	if (r32 != array32[i]) {
	    printf("\nmismatch at %d array32:%x gen:%x\n", 
		   i, array32[i], r32);
	    exit(1);
	}
    }
    for (i = 0; i < 700; i++) {
	r32 = gen_rand32();
	if (r32 != array32_2[i]) {
	    printf("\nmismatch at %d array32_2:%x gen:%x\n", 
		   i, array32_2[i], r32);
	    exit(1);
	}
    }
    printf("\n");
    init_by_array(ini, 4);
    printf("init_by_array__________\n");
    fill_array32(array32, 10000);
    fill_array32(array32_2, 10000);
    init_by_array(ini, 4);
    for (i = 0; i < 10000; i++) {
	if (i < 1000) {
	    printf("%10u ", array32[i]);
	    if (i % 5 == 4) {
		printf("\n");
	    }
	}
	r32 = gen_rand32();
	if (r32 != array32[i]) {
	    printf("\nmismatch at %d array32:%x gen:%x\n", 
		   i, array32[i], r32);
	    exit(1);
	}
    }
    for (i = 0; i < 700; i++) {
	r32 = gen_rand32();
	if (r32 != array32_2[i]) {
	    printf("\nmismatch at %d array32_2:%x gen:%x\n", 
		   i, array32_2[i], r32);
	    exit(1);
	}
    }
}

void speed32(void) {
    int i, j;
    clock_t clo;
    clock_t min = LONG_MAX;
    uint32_t *array32 = (uint32_t *)array1;

    /* 32 bit generation */
    init_gen_rand(1234);
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < COUNT; j++) {
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
    for (i = 0; i < 10; i++) {
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
}

int main(int argc, char *argv[]) {
    int speed = 0;

    if ((argc >= 2) && (strncmp(argv[1],"-s",2) == 0)) {
	speed = 1;
    }
    if (speed) {
	speed32();
    } else {
	check32();
    }
    return 0;
}
