/* ************************************************************************** */
/* Copyright:   Francois Panneton and Pierre L'Ecuyer, University of Montreal */
/*              Makoto Matsumoto, Hiroshima University                        */
/* Notice:      This code can be used freely for personal, academic,          */
/*              or non-commercial purposes. For commercial purposes,          */
/*              please contact P. L'Ecuyer at: lecuyer@iro.UMontreal.ca       */
/* ************************************************************************** */
#include <string.h>
#if defined(__GNUC__)
  #include <stdint.h>
#endif
#include <stdio.h>
#include "random-inline.h"
#include <assert.h>

#define W 32
#define R 623
#define N 1024
#define MASK_N 0x3ff
#define P 31
#define MASKU (0xffffffffU>>(W-P))
#define MASKL (~MASKU)
#define M1 70
#define M2 179
#define M3 449
#define MM1 (R - M1)
#define MM2 (R - M2)
#define MM3 (R - M3)
/* To obtain the WELL19937c, uncomment the following line */
#define TEMPERING
#define TEMPERB 0xe46e1700U
#define TEMPERC 0x9b868000U

static int idx = 0;
static uint32_t STATE[N];
static uint32_t lung;
void InitWELLRNG19937a (uint32_t *init);
INLINE void init_gen_rand(uint32_t seed);
INLINE uint32_t gen_rand(void);
INLINE static uint32_t xorsr(uint32_t x, int shift);
INLINE static uint32_t xorsl(uint32_t x, int shift);
INLINE static uint32_t do_recursion(uint32_t *lung, uint32_t v1,
				    uint32_t v2, uint32_t vm1,
				    uint32_t vm2, uint32_t vm3);

void InitWELLRNG19937a (uint32_t *init){
    int i, j;

    idx = 0;
    lung = init[0];
    memset(STATE, 0, sizeof(STATE));
    for (i = R, j = 0; j < R; j++) {
	STATE[j] = init[i--];
    }
}

INLINE void init_gen_rand(uint32_t seed)
{
    uint32_t init_vec[R + 1];
    int i;

    memset(init_vec, 0, sizeof(init_vec));
    for (i = 0; i <= R; i++) {
	init_vec[i] = seed + i;
    }
    InitWELLRNG19937a(init_vec);
}

INLINE static uint32_t xorsr(uint32_t x, int shift) {
    return x ^ (x >> shift);
}

INLINE static uint32_t xorsl(uint32_t x, int shift) {
    return x ^ (x << shift);
}

INLINE uint32_t gen_rand (void) {
    uint32_t z0, z1, z2;
    uint32_t y;

    z0 = (STATE[idx] & MASKL) | (STATE[(idx + 1) & MASK_N] & MASKU);
    z1 = xorsl(lung, 25) ^ xorsr(STATE[(idx + MM1) & MASK_N], 27);
    z2 = (STATE[(idx + MM2) & MASK_N] >> 9) 
	^ xorsr(STATE[(idx + MM3) & MASK_N], 1);
    STATE[(idx + R) & MASK_N] = z1 ^ z2;
    lung = z0 ^ xorsl(z1, 9) ^ xorsl(z2, 21) 
	^ xorsr(STATE[(idx + R) & MASK_N], 21);
    idx = (idx + 1) & MASK_N;
    // tempering
    y = lung ^ ((lung << 7) & TEMPERB);
    y = y ^ ((y << 15) & TEMPERC);
    return y;
}

INLINE static uint32_t do_recursion(uint32_t *lu, uint32_t v1,
				    uint32_t v2, uint32_t vm1,
				    uint32_t vm2, uint32_t vm3) {
    uint32_t z0, z1, z2, r;

    z0 = (v1 & MASKL) | (v2 & MASKU);
    z1 = xorsl(*lu, 25) ^ xorsr(vm1, 27);
    z2 = (vm2 >> 9) ^ xorsr(vm3, 1);
    r = z1 ^ z2;
    *lu = z0 ^ xorsl(z1, 9) ^ xorsl(z2, 21) ^ xorsr(r, 21);
    return r;
}

INLINE void fill_array(uint32_t array[], int size)
{
    int i;
    uint32_t r, y, lun;
    assert(size >= 2 * R);

    lun = lung;
    for (i = 0; i + MM1 < R; i++) {
	r = do_recursion(&lun, STATE[i], STATE[i + 1], STATE[i + MM1],
			 STATE[i + MM2], STATE[i + MM3]);
	array[i + R] = r;
	y = lun ^ ((lun << 7) & TEMPERB);
	y = y ^ ((y << 15) & TEMPERC);
	array[i] = y;
    }
    for (; i + MM2 < R; i++) {
	r = do_recursion(&lun, STATE[i], STATE[i + 1], array[i + MM1],
			 STATE[i + MM2], STATE[i + MM3]);
	array[i + R] = r;
	y = lun ^ ((lun << 7) & TEMPERB);
	y = y ^ ((y << 15) & TEMPERC);
	array[i] = y;
    }
    for (; i + MM3 < R; i++) {
	r = do_recursion(&lun, STATE[i], STATE[i + 1], array[i + MM1],
			 array[i + MM2], STATE[i + MM3]);
	array[i + R] = r;
	y = lun ^ ((lun << 7) & TEMPERB);
	y = y ^ ((y << 15) & TEMPERC);
	array[i] = y;
    }
    for (; i + 1 < R; i++) {
	r = do_recursion(&lun, STATE[i], STATE[i + 1], array[i + MM1],
			 array[i + MM2], array[i + MM3]);
	array[i + R] = r;
	y = lun ^ ((lun << 7) & TEMPERB);
	y = y ^ ((y << 15) & TEMPERC);
	array[i] = y;
    }
    r = do_recursion(&lun, STATE[i], array[i + 1], array[i + MM1],
		     array[i + MM2], array[i + MM3]);
    array[i + R] = r;
    y = lun ^ ((lun << 7) & TEMPERB);
    y = y ^ ((y << 15) & TEMPERC);
    array[i] = y;
    i++;
    for (;i + R < size; i++) {
	r = do_recursion(&lun, array[i], array[i + 1], array[i + MM1],
			 array[i + MM2], array[i + MM3]);
	array[i + R] = r;
	y = lun ^ ((lun << 7) & TEMPERB);
	y = y ^ ((y << 15) & TEMPERC);
	array[i] = y;
    }
    memcpy(STATE, &array[i], sizeof(uint32_t) * R);
    lung = lun;
    for (; i < size; i++) {
	array[i] = gen_rand();
    }
}

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif


