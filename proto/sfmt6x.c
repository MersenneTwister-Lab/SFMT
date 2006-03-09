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

static unsigned int POS1 = 1;
static unsigned int SL1 = 11;
static unsigned int SL2 = 7;
static unsigned int SL3 = 7;
static unsigned int SL4 = 7;
static unsigned int SL5 = 7;
static unsigned int SL6 = 7;
static unsigned int SL7 = 7;
static unsigned int SL8 = 7;
static unsigned int SR1 = 17;
static unsigned int SR2 = 9;
static unsigned int SR3 = 9;
static unsigned int SR4 = 9;

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
    POS1 = p1 % (N-2) + 1;
    SL1 = p2 % (32 - 1) + 1;
    //SL2 = p3 % (32 - 1) + 1;
    //SL3 = p4 % (32 - 1) + 1;
    //SL4 = p5 % (32 - 1) + 1;
    //SL5 = p6 % (32 - 1) + 1;
    //SL6 = p7 % (32 - 1) + 1;
    //SL7 = p8 % (32 - 1) + 1;
    //SL8 = p9 % (32 - 1) + 1;
    SR1 = p10 % (32 - 1) + 1;
    //SR2 = p11 % (32 - 1) + 1;
    //SR3 = p12 % (32 - 1) + 1;
    //SR4 = p13 % (32 - 1) + 1;
    memset(sfmt, 0, sizeof(sfmt));
}

void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    //fprintf(fp, "SL2 = %u\n", SL2);
    //fprintf(fp, "SL3 = %u\n", SL3);
    //fprintf(fp, "SL4 = %u\n", SL4);
    //fprintf(fp, "SL5 = %u\n", SL5);
    //fprintf(fp, "SL6 = %u\n", SL6);
    //fprintf(fp, "SL7 = %u\n", SL7);
    //fprintf(fp, "SL8 = %u\n", SL8);
    fprintf(fp, "SR1 = %u\n", SR1);
    //fprintf(fp, "SR2 = %u\n", SR2);
    //fprintf(fp, "SR3 = %u\n", SR3);
    //fprintf(fp, "SR4 = %u\n", SR4);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SL3, SL4, SL5, SL6, SL7, SL8,"
	    " SR1, SR2, SR3, SR4] = "
	    "[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u]\n", 
	    POS1, SL1, SL2, SL3, SL4, 0, 0, 0, 0, 
	    SR1, SR2, SR3, SR4);
    fflush(fp);
}

static void gen_rand_all(void) {
    int i;

    for (i = 0; i < N; i++) {
	sfmt[i] = (sfmt[i] << SL1) ^ sfmt[i]
	    ^ sfmt[(i + POS1) % N]
	    ^ (sfmt[(i + N - 1) % N] >> SR1) ^ sfmt[(i + N - 1) % N];
    }
}

uint32_t gen_rand(void)
{
    uint32_t r;

    if (idx >= N) {
	gen_rand_all();
	idx = 0;
    }
    r = sfmt[idx];
    idx++;
    return r;
}

void init_gen_rand(uint32_t seed)
{
    int i;

    sfmt[0] = seed;
    for (i = 1; i < N; i++) {
	seed = 1812433253UL * (seed ^ (seed >> 30)) + i;
	sfmt[i] = seed;
    }
    idx = N;
}
