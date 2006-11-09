#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include "sfmt.h"
#include "params.h"

static uint32_t sfmt[N + 1][4];
static unsigned int idx;
static uint32_t *sfmt32 = &sfmt[0][0];
static uint32_t parity[4] = {PARITY1, PARITY2, PARITY3, PARITY4};

static void gen_rand_all(void);

static void lshift128(uint32_t out[4], const uint32_t in[4], int shift) {
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in[3] << 32) | ((uint64_t)in[2]);
    tl = ((uint64_t)in[1] << 32) | ((uint64_t)in[0]);

    oh = th << (shift * 8);
    ol = tl << (shift * 8);
    oh |= tl >> (64 - shift * 8);
    out[1] = (uint32_t)(ol >> 32);
    out[0] = (uint32_t)ol;
    out[3] = (uint32_t)(oh >> 32);
    out[2] = (uint32_t)oh;
}

static void do_recursion(uint32_t a[4], uint32_t b[4],
			 uint32_t c[4], uint32_t d[4]) {
    uint32_t x[4];

    lshift128(x, a, SL2);
    a[0] = a[0] ^ x[0] ^ ((b[0] >> SR1) & MSK1) ^ (c[0] >> SR2) ^ (c[0] << SL1)
	^ d[3];
    a[1] = a[1] ^ x[1] ^ ((b[1] >> SR1) & MSK2) ^ (c[1] >> SR2) ^ (c[1] << SL1)
	^ d[2];
    a[2] = a[2] ^ x[2] ^ ((b[2] >> SR1) & MSK3) ^ (c[2] >> SR2) ^ (c[2] << SL1)
	^ d[0];
    a[3] = a[3] ^ x[3] ^ ((b[3] >> SR1) & MSK4) ^ (c[3] >> SR2) ^ (c[3] << SL1)
	^ d[1];
}

static void assign128(uint32_t to[4], uint32_t from[4]) {
    to[0] = from[0];
    to[1] = from[1];
    to[2] = from[2];
    to[3] = from[3];
}

static void xor128(uint32_t to[4], uint32_t from[4]) {
    to[0] ^= from[0];
    to[1] ^= from[1];
    to[2] ^= from[2];
    to[3] ^= from[3];
}

static void gen_rand_all(void) {
    int i;
    uint32_t lung[4];

    assign128(lung, sfmt[N]);
    for (i = 0; i < N; i++) {
	do_recursion(sfmt[i], sfmt[(i + POS1) % N], sfmt[(i + N -1) % N], lung);
	xor128(lung, sfmt[i]);
    }
    assign128(sfmt[N], lung);
}

uint32_t gen_rand32(void)
{
    uint32_t r;

    if (idx >= N32) {
	gen_rand_all();
	idx = 0;
    }
    r = sfmt32[idx];
    idx++;
    return r;
}

void static initial_status_parity_check(void) {
    int inner = 0;
    int i, j;
    uint32_t work;

    for (i = 0; i < 4; i++) {
	work = sfmt[N][i] ^ parity[i];
	for (j = 0; j < 32; j++) {
	    inner ^= work & 1;
	    work = work >> 1;
	}
    }
    if (inner == 1) {
	printf("DEBUG:success in parity check\n");
	return;
    }
    printf("DEBUG:failure in parity check\n");
    for (i = 0; i < 4; i++) {
	work = 1;
	for (j = 0; j < 32; j++) {
	    if ((work & parity[i]) != 0) {
		printf("DEBUG: before = %x\n", sfmt[N][i]);
		sfmt[N][i] ^= work;
		printf("DEBUG: change %dth bit of index %d\n", j, i);
		printf("DEBUG: work = %x, parity[%d] = %d\n", work, i,
		       parity[i]);
		printf("DEBUG: after = %x\n", sfmt[N][i]);
		return;
	    }
	    work = work << 1;
	}
    }
}

void init_gen_rand(uint32_t seed)
{
    int i;

    sfmt32[0] = seed;
    for (i = 1; i < (N + 1) * 4; i++) {
	sfmt32[i] = 1812433253UL * (sfmt32[i - 1] ^ (sfmt32[i - 1] >> 30)) + i;
    }
    idx = N32;
    initial_status_parity_check();
}

#if defined(TEST)
int main(void) {
    int i;
    init_gen_rand(1234);
    for (i = 0; i < 1000; i++) {
	printf("%10u ", gen_rand32());
	if (i % 5 == 4) {
	    printf("\n");
	}
    }
    printf("\n");
    return 0;
}
#endif
