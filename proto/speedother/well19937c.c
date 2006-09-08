/* ***************************************************************************** */
/* Copyright:      Francois Panneton and Pierre L'Ecuyer, University of Montreal */
/*                 Makoto Matsumoto, Hiroshima University                        */
/* Notice:         This code can be used freely for personal, academic,          */
/*                 or non-commercial purposes. For commercial purposes,          */
/*                 please contact P. L'Ecuyer at: lecuyer@iro.UMontreal.ca       */
/* ***************************************************************************** */

#include <string.h>
#if defined(__GNUC__)
  #include <stdint.h>
#endif
#include <stdio.h>
#include "random-inline.h"
#include <assert.h>
#define W 32
#define R 624
#define R1 623
#define P 31
#define MASKU (0xffffffffU>>(W-P))
#define MASKL (~MASKU)
//#define M1 70
#define M1 553
//#define M2 179
#define M2 444
//#define M3 449
#define M3 174

#define MAT0POS(t,v) (v^(v>>t))
#define MAT0NEG(t,v) (v^(v<<(-(t))))
#define MAT1(v) v
#define MAT3POS(t,v) (v>>t)

/* To obtain the WELL19937c, uncomment the following line */
//#define TEMPERING
#define TEMPERB 0xe46e1700U
#define TEMPERC 0x9b868000U

//#define V0            STATE[state_i]
//#define VM1Over       STATE[state_i+M1-R]
//#define VM1           STATE[state_i+M1]
//#define VM2Over       STATE[state_i+M2-R]
//#define VM2           STATE[state_i+M2]
//#define VM3Over       STATE[state_i+M3-R]
//#define VM3           STATE[state_i+M3]
//#define VRm1          STATE[state_i-1]
//#define VRm1Under     STATE[state_i+R-1]
//#define VRm2          STATE[state_i-2]
//#define VRm2Under     STATE[state_i+R-2]

//#define newV0         STATE[state_i-1]
//#define newV0Under    STATE[state_i-1+R]
//#define newV1         STATE[state_i]
#define newVRm1       STATE[state_i-2]
#define newVRm1Under  STATE[state_i-2+R]

#define FACT 2.32830643653869628906e-10

static int state_i = 0;
static int mti = R;
static uint32_t STATE[R];
static uint32_t INITIAL_STATE[R]={ /* initial 25 seeds, change as you wish */
    0x95f24dab, 0x0b685215, 0xe76ccae7, 0xaf3ec239, 0x715fad23,
    0x24a590ad, 0x69e4b5ef, 0xbf456141, 0x96bc1b7b, 0xa7bdf825,
    0xc1de75b7, 0x8858a9c9, 0x2da87693, 0xb657f9dd, 0xffdc8a9f,
    0x8121da71, 0x8b823ecb, 0x885d05f5, 0x4e20cd47, 0x5a9ad5d9,
    0x512c0c03, 0xea857ccd, 0x4cc1d30f, 0x8891a8a1, 0xa6b7aadb
};
//       double (*WELLRNG19937a) (void);
//static int sw = 1;

//static unsigned int y;

INLINE void init_gen_rand(uint32_t seed);
INLINE void init_gen_rand(uint32_t seed)
{
    memcpy(STATE, INITIAL_STATE, sizeof(uint32_t) * R);
    STATE[0] = seed;
    state_i = 0;
    mti = 0;
}


INLINE static void do_recursion(uint32_t *u, uint32_t *v, uint32_t v1,
				uint32_t v2, uint32_t vm1, uint32_t vm2,
				uint32_t vm3) {
    unsigned int z0, z1, z2;

    z0 = (v1 & MASKL) | (v2 & MASKU);
    z1 = MAT0NEG(-25, *u) ^ MAT0POS(27, vm1);
    z2 = MAT3POS(9, vm2) ^ MAT0POS(1, vm3);
    *v = z1 ^ z2;
    *u = MAT1 (z0) ^ MAT0NEG (-9, z1) 
	^ MAT0NEG (-21, z2) ^ MAT0POS (21, *v);
}

INLINE static void gen_rand_all(void);
INLINE static void gen_rand_all(void) {
    int i;
    uint32_t u;

    u = STATE[R - 1];
    for (i = 0; i + M3 < R1; i++) {
	do_recursion(&u, &STATE[i], STATE[i + 1], STATE[i + 2],
		     STATE[i + M1], STATE[i + M2], STATE[i + M3]);
    }
    for (; i + M2 < R1; i++) {
	do_recursion(&u, &STATE[i], STATE[i + 1], STATE[i + 2],
		     STATE[i + M1], STATE[i + M2], STATE[i + M3 - R1]);
    }
    for (; i + M1 < R1; i++) {
	do_recursion(&u, &STATE[i], STATE[i + 1], STATE[i + 2],
		     STATE[i + M1], STATE[i + M2 - R1], STATE[i + M3 - R1]);
    }
    for (; i + 2 < R1; i++) {
	do_recursion(&u, &STATE[i], STATE[i + 1], STATE[i + 2],
		     STATE[i + M1 - R1], STATE[i + M2 - R1], 
		     STATE[i + M3 - R1]);
    }
    do_recursion(&u, &STATE[i], STATE[i + 1], STATE[i + 2 - R1], 
		 STATE[i + M1 - R1], STATE[i + M2 - R1], STATE[i + M3 - R1]);
    i++;
    do_recursion(&u, &STATE[i], STATE[i + 1 - R1], STATE[i + 2 - R1],
		 STATE[i + M1 - R1], STATE[i + M2 - R1], STATE[i + M3 - R1]);
    STATE[R - 1] = u;
}

INLINE static void gen_rand_array(uint32_t array[], int size) {
    int i, j;
    uint32_t u, y;

    u = STATE[R - 1];
    for (i = 0; i + M3 < R1; i++) {
	do_recursion(&u, &array[i], STATE[i + 1], STATE[i + 2],
		     STATE[i + M1], STATE[i + M2], STATE[i + M3]);
    }
    for (; i + M2 < R1; i++) {
	do_recursion(&u, &array[i], STATE[i + 1], STATE[i + 2], STATE[i + M1],
		     STATE[i + M2], array[i + M3 - R1]);
    }
    for (; i + M1 < R1; i++) {
	do_recursion(&u, &array[i], STATE[i + 1], STATE[i + 2], STATE[i + M1],
		     array[i + M2 - R1], array[i + M3 - R1]);
    }
    for (; i + 2 < R1; i++) {
	do_recursion(&u, &array[i], STATE[i + 1], STATE[i + 2],
		     array[i + M1 - R1], array[i + M2 - R1],
		     array[i + M3 - R1]);
    }
    do_recursion(&u, &array[i], STATE[i + 1], array[i + 2 - R1],
		 array[i + M1 - R1], array[i + M2 - R1], array[i + M3 - R1]);
    i++;
    do_recursion(&u, &array[i], array[i + 1 - R1], array[i + 2 - R1],
		 array[i + M1 - R1], array[i + M2 - R1],
		 array[i + M3 - R1]);
    i++;
    for (;i < size - R1; i++) {
	do_recursion(&u, &array[i], array[i + 1 - R1], array[i + 2 - R1],
		     array[i + M1 - R1], array[i + M2 - R1],
		     array[i + M3 - R1]);
	/* Tempering */
	y = array[i - R1];
	y = y ^ ((y << 7) & TEMPERB);
	y = y ^ ((y << 15) & TEMPERC);
	array[i - R1] = y;
    }
    for (j = 0; j < 2 * R1 - size; j++) {
	STATE[j] = array[j + size - R1];
    }
    for (;i < size; i++, j++) {
	do_recursion(&u, &array[i], array[i + 1 - R1], array[i + 2 - R1],
		     array[i + M1 - R1], array[i + M2 - R1],
		     array[i + M3 - R1]);
	STATE[j] = array[i];
	y = array[i - R1];
	y = y ^ ((y << 7) & TEMPERB);
	y = y ^ ((y << 15) & TEMPERC);
	array[i - R1] = y;
    }
    for (j = size - R1;j < size; j++) {
	y = array[j];
	y = y ^ ((y << 7) & TEMPERB);
	y = y ^ ((y << 15) & TEMPERC);
	array[j] = y;
    }
    STATE[R - 1] = u;
}

INLINE uint32_t gen_rand(void);
INLINE uint32_t gen_rand(void)
{
    uint32_t y;

    if (mti >= R) {		/* generate N words at one time */
	gen_rand_all();
	mti = 0;
    }

    y = STATE[mti++];

    /* Tempering */
    y = y ^ ((y << 7) & TEMPERB);
    y = y ^ ((y << 15) & TEMPERC);
    return y;
}

INLINE void fill_array(uint32_t array[], int size)
{
    assert(size >= R1);
    gen_rand_array(array, size);
}

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif
