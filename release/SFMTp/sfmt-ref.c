#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include "sfmt.h"
#include "params.h"

static uint32_t sfmt[N + 1][4];
static int idx;
static uint32_t *sfmt32 = &sfmt[0][0];
static uint32_t parity[4] = {PARITY1, PARITY2, PARITY3, PARITY4};
static int initialized = 0;
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

static void xor128(uint32_t to[4], uint32_t from[4]) {
    to[0] ^= from[0];
    to[1] ^= from[1];
    to[2] ^= from[2];
    to[3] ^= from[3];
}

static void gen_rand_all(void) {
    int i;

    for (i = 0; i < N; i++) {
	do_recursion(sfmt[i], sfmt[(i + POS1) % N], sfmt[(i + N -1) % N],
		     sfmt[N]);
	xor128(sfmt[N], sfmt[i]);
    }
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

static void initial_status_parity_check(void) {
    int inner = 0;
    int i, j;
    uint32_t work;

    for (i = 0; i < 4; i++) {
	work = sfmt[N][i] & parity[i];
	for (j = 0; j < 32; j++) {
	    inner ^= work & 1;
	    work = work >> 1;
	}
    }
    /* check OK */
    if (inner == 1) {
	return;
    }
    /* check NG, and modification */
    for (i = 0; i < 4; i++) {
	work = 1;
	for (j = 0; j < 32; j++) {
	    if ((work & parity[i]) != 0) {
		sfmt[N][i] ^= work;
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
    initialized = 1;
}

/**
 * This function represents a function used in the initialization
 * by init_by_array
 * @param x 32-bit integer
 * @return 32-bit integer
 */
static uint32_t func1(uint32_t x) {
    return (x ^ (x >> 27)) * (uint32_t)1664525UL;
}

/**
 * This function represents a function used in the initialization
 * by init_by_array
 * @param x 32-bit integer
 * @return 32-bit integer
 */
static uint32_t func2(uint32_t x) {
    return (x ^ (x >> 27)) * (uint32_t)1566083941UL;
}

/**
 * This function initializes the internal state array,
 * with an array of 32-bit integers used as the seeds
 * @param init_key the array of 32-bit integers, used as a seed.
 * @param key_length the length of init_key.
 */
void init_by_array(uint32_t init_key[], int key_length) {
    int i, j, count;
    uint32_t r;
    int LAG;
    int MID;

    if (N >= 623) {
	LAG = 11;
    } else if (N >= 68) {
	LAG = 7;
    } else if (N >= 39) {
	LAG = 5;
    } else {
	LAG = 3;
    }
    MID = (N - LAG) / 2;

    memset(sfmt, 0x8b, sizeof(sfmt));
    if (key_length + 1 > N) {
	count = key_length + 1;
    } else {
	count = N;
    }
    r = func1(sfmt32[0] ^ sfmt32[MID % N] ^ sfmt32[N - 1]);
    sfmt32[MID % N] += r;
    r += key_length;
    sfmt32[(MID + LAG) % N] += r;
    sfmt32[0] = r;
    count--;
    for (i = 1, j = 0; (j < count) && (j < key_length); j++) {
	r = func1(sfmt32[i] ^ sfmt32[(i + MID) % N] ^ sfmt32[(i + N - 1) % N]);
	sfmt32[(i + MID) % N] += r;
	r += init_key[j] + i;
	sfmt32[(i + MID + LAG) % N] += r;
	sfmt32[i] = r;
	i = (i + 1) % N;
    }
    for (; j < count; j++) {
	r = func1(sfmt32[i] ^ sfmt32[(i + MID) % N] ^ sfmt32[(i + N - 1) % N]);
	sfmt32[(i + MID) % N] += r;
	r += i;
	sfmt32[(i + MID + LAG) % N] += r;
	sfmt32[i] = r;
	i = (i + 1) % N;
    }
    for (j = 0; j < N; j++) {
	r = func2(sfmt32[i] + sfmt32[(i + MID) % N] + sfmt32[(i + N - 1) % N]);
	sfmt32[(i + MID) % N] ^= r;
	r -= i;
	sfmt32[(i + MID + LAG) % N] ^= r;
	sfmt32[i] = r;
	i = (i + 1) % N;
    }

    idx = N32;
    initial_status_parity_check();
    initialized = 1;
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
