/* SFMT Search Code, M.Saito 2006/2/28 */
/* (1234) 巡回置換１回のみ */
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "sfmt.h"

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 32
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

static uint32_t sfmt[N];
static unsigned int idx;

static unsigned int POS1 = 11;
static unsigned int SL1 = 5;
static unsigned int SL2 = 1;
static unsigned int SR1 = 1;

static void gen_rand_all(void);

unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

void setup_param(unsigned int p1, unsigned int p2, unsigned int p3, 
		 unsigned int p4, unsigned int p5, unsigned int p6,
		 unsigned int p7, unsigned int p8, unsigned int p9,
		 unsigned int p10, unsigned int p11, unsigned int p12,
		 unsigned int p13) {
    p1 = p1 % (N - 4);
    if (p1 < 4) {
	p1 += 4;
    }
    POS1 = p1;
    SL1 = p2 % 32;
    SL2 = p3 % 32;
    SR1 = p4 % 32;
    memset(sfmt, 0, sizeof(sfmt));
}

void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    fprintf(fp, "SL2 = %u\n", SL2);
    fprintf(fp, "SR1 = %u\n", SR1);
    fflush(fp);
}

#if 0
void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SL3, SL4, SL5, SL6, SL7, SL8,"
	    " SR1, SR2, SR3, SR4] = "
	    "[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u]\n", 
	    POS1, SL1, SL2, SL3, SL4, 0, 0, 0, 0, 
	    SR1, SR2, SR3, SR4);
    fflush(fp);
}
#endif

static void gen_rand_all(void) {
    int i;

    for (i = 0; i < N; i++) {
	sfmt[i] = (sfmt[i] << SL1) ^ sfmt[i]
	    ^ (sfmt[(i + POS1) % N] << SL2) ^ sfmt[(i + POS1) % N]
	    ^ (sfmt[(i + N - 4) % N] >> SR1) ^ sfmt[(i + N - 4) % N];
    }
}

uint32_t gen_rand(void)
{
    uint32_t r;

    if (idx >= N) {
	gen_rand_all();
	idx = 0;
    }
    r = sfmt[idx++];
    return r;
}

void init_gen_rand(uint32_t seed)
{
    int i;

    for (i = 0; i < N; i++) {
	sfmt[i] = seed;
	seed = 1812433253UL * (seed ^ (seed >> 30)) + i;
    }
    idx = N;
}
