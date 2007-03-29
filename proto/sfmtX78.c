/* SFMT Search Code, M.Saito 2006/2/28 */
/* (1234) 巡回置換１回のみ */
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "sfmt.h"

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 128
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

struct W128_T {
    uint32_t a[4];
};

typedef struct W128_T w128_t;

static uint32_t sfmt[N][4];
static unsigned int idx;

static unsigned int POS1 = 1;
static unsigned int SL1 = 11;
static unsigned int SL2 = 11;
static unsigned int SR1 = 7;
static unsigned int SR2 = 7;
static unsigned int MSK1 = 7;
static unsigned int MSK2 = 7;
static unsigned int MSK3 = 7;
static unsigned int MSK4 = 7;

static void gen_rand_all(void);

unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

void setup_param(uint32_t array[], int *idx) {
    POS1 = array[(*idx)++] % (N-2) + 1;
    SL1 = array[(*idx)++] % (32 - 1) + 1;
    SL2 = (array[(*idx)++] % 4) * 2 + 1; 
    SR1 = array[(*idx)++] % (32 - 1) + 1;
    SR2 = (array[(*idx)++] % 4) * 2 + 1;
    MSK1= array[(*idx)++] | array[(*idx)++] | array[(*idx)++];
    MSK2= array[(*idx)++] | array[(*idx)++] | array[(*idx)++];
    MSK3= array[(*idx)++] | array[(*idx)++] | array[(*idx)++];
    MSK4= array[(*idx)++] | array[(*idx)++] | array[(*idx)++];
    memset(sfmt, 0, sizeof(sfmt));
}

void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    fprintf(fp, "SL2 = %u\n", SL2);
    fprintf(fp, "SR1 = %u\n", SR1);
    fprintf(fp, "SR2 = %u\n", SR2);
    fprintf(fp, "MSK1 = %08x\n", MSK1);
    fprintf(fp, "MSK2 = %08x\n", MSK2);
    fprintf(fp, "MSK3 = %08x\n", MSK3);
    fprintf(fp, "MSK4 = %08x\n", MSK4);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SL3, SL4, SL5, SL6, SL7, SL8,"
	    " SR1, SR2, SR3, SR4] = "
	    "[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u]\n", 
	    POS1, SL1, SR1, MSK1, MSK2, MSK3, MSK4, 0, 0, 
	    0, 0, 0, 0);
    fflush(fp);
}


inline static void rshift128(uint32_t out[4], const uint32_t in[4],
			     int shift) {
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in[3] << 32) | ((uint64_t)in[2]);
    tl = ((uint64_t)in[1] << 32) | ((uint64_t)in[0]);

    oh = th >> (shift * 8);
    ol = tl >> (shift * 8);
    ol |= th << (64 - shift * 8);
    out[1] = (uint32_t)(ol >> 32);
    out[0] = (uint32_t)ol;
    out[3] = (uint32_t)(oh >> 32);
    out[2] = (uint32_t)oh;
}

inline static void lshift128(uint32_t out[4], const uint32_t in[4],
			     int shift) {
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

static inline void do_recursion(uint32_t r[4], uint32_t a[4], uint32_t b[4],
				uint32_t c[4], uint32_t d[4]) {
    uint32_t x[4];
    uint32_t y[4];

    lshift128(x, a, SL2);
    rshift128(y, c, SR2);
    r[0] = a[0] ^ x[0] ^ ((b[0] >> SR1) & MSK1) ^ y[0] ^ (d[0] << SL1);
    r[1] = a[1] ^ x[1] ^ ((b[1] >> SR1) & MSK2) ^ y[1] ^ (d[1] << SL1);
    r[2] = a[2] ^ x[2] ^ ((b[2] >> SR1) & MSK3) ^ y[2] ^ (d[2] << SL1);
    r[3] = a[3] ^ x[3] ^ ((b[3] >> SR1) & MSK4) ^ y[3] ^ (d[3] << SL1);
}

static void gen_rand_all(void) {
    int i;

    for (i = 0; i < N; i++) {
	do_recursion(sfmt[i], sfmt[i], sfmt[(i + POS1) % N],
		     sfmt[(i + N - 2) % N], sfmt[(i + N - 1) % N]);
    }
}

/* for 128 bit check */
inline uint32_t gen_rand(void)
{
    uint32_t r;

    if (idx >= N) {
	gen_rand_all();
	idx = 0;
    }
    r = sfmt[idx][0];
    idx++;
    return r;
}

void fill_array(uint32_t array[][4], int size) {
    int i, j;

    for (i = 0; idx < N && i < size; i++, idx++) {
	for (j = 0; j < 4; j++) {
	    array[i][j] = sfmt[idx][j];
	}
    }
    while (i < size) {
	gen_rand_all();
	for (idx = 0; idx < N && i < size; i++, idx++) {
	    for (j = 0; j < 4; j++) {
		array[i][j] = sfmt[idx][j];
	    }
	}
    }
}

/* for 128 bit check */
void init_gen_rand(uint32_t seed)
{
    int i;

    sfmt[0][0] = seed;
    for (i = 1; i < N * 4; i++) {
	sfmt[i/4][i%4] = 1812433253UL 
	    * (sfmt[(i - 1) / 4][(i - 1) % 4]
	       ^ (sfmt[(i - 1) / 4][(i - 1) % 4] >> 30)) 
	    + i;
    }
    idx = 0;
}
