/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "random-inline.h"

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 128
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

static uint32_t sfmt[N][4];
static unsigned int idx;

static unsigned int POS1 = 37;
static unsigned int SL1 = 2;
static unsigned int SR1 = 7;
static unsigned int MSK1 = 0x75c43b4fUL;
static unsigned int MSK2 = 0xb47d5030UL;
static unsigned int MSK3 = 0x244d9e76UL;
static unsigned int MSK4 = 0xd7fd6406UL;

unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

unsigned int get_onetime_rnds(void) {
    return N * 4;
}

void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    fprintf(fp, "SR1 = %u\n", SR1);
    fprintf(fp, "MSK1 = %u\n", MSK1);
    fprintf(fp, "MSK2 = %u\n", MSK2);
    fprintf(fp, "MSK3 = %u\n", MSK3);
    fprintf(fp, "MSK4 = %u\n", MSK4);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SL3, SL4, SR1, SR2, SR3, SR4] ="
	    " [%u,%u,%u,%u,%u,%u,%u,%u,%u]\n", 
	    POS1, SL1, SR1, MSK1, MSK2, MSK3, MSK4, 0, 0);
    fflush(fp);
}

void print_state(FILE *fp) {
    int i, j;
    for (i = 0; i < N; i++) {
	for (j = 0; j < 4; j++) {
	    printf("%08x ", sfmt[i][j]);
	}
	if (i % 2 == 1) {
	    printf("\n");
	}
    }
}

static inline void do_recursion(uint32_t a[4], uint32_t b[4], uint32_t c[4]) {
    a[0] = a[0] ^ (a[0] << SL1) ^ b[1] ^ (b[0] & MSK1) ^ c[0] ^ (c[0] >> SR1);
    a[1] = a[1] ^ (a[1] << SL1) ^ b[2] ^ (b[1] & MSK2) ^ c[1] ^ (c[1] >> SR1);
    a[2] = a[2] ^ (a[2] << SL1) ^ b[3] ^ (b[2] & MSK3) ^ c[2] ^ (c[2] >> SR1);
    a[3] = a[3] ^ (a[3] << SL1) ^ b[0] ^ (b[3] & MSK4) ^ c[3] ^ (c[3] >> SR1);
}

void gen_rand_all(void) {
    int i;

    for (i = 0; i < N; i++) {
	do_recursion(sfmt[i], sfmt[(i + POS1) % N], sfmt[(i + N - 1) % N]);
    }
}

uint32_t gen_rand(void)
{
    uint32_t r;

    if (idx >= N * 4) {
	gen_rand_all();
	idx = 0;
    }
    r = sfmt[idx / 4][idx % 4];
    idx++;
    return r;
}

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
    idx = N * 4;
}
