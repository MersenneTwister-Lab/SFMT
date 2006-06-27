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

static uint32_t sfmt[N][4];
static unsigned int idx;

static unsigned int POS1 = 1;
static int SL1 = 11;
static unsigned int SR1 = 7;
static unsigned int MSK1 = 7;
static unsigned int  MSK2 = 7;
static unsigned int MSK3 = 7;
static unsigned int MSK4 = 7;
//static unsigned int SL7 = 7;
//static unsigned int SL8 = 7;
//static unsigned int SR1 = 17;
//static unsigned int SR2 = 9;
//static unsigned int SR3 = 9;
//static unsigned int SR4 = 9;

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
    SR1 = p2 % 2;
    SL1 = p3 % (8 - 1) + 1;
    MSK1= p4 | p5 | p6;
    MSK2= p6 | p7 | p8;
    MSK3= p9 | p10 | p11;
    MSK4= p11 | p12 | p13;
    //SL7 = p8 % (32 - 1) + 1;
    //SL8 = p9 % (32 - 1) + 1;
    //SR1 = p10 % (32 - 1) + 1;
    //SR2 = p11 % (32 - 1) + 1;
    //SR3 = p12 % (32 - 1) + 1;
    //SR4 = p13 % (32 - 1) + 1;
    memset(sfmt, 0, sizeof(sfmt));
}

void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %d\n", SL1);
    fprintf(fp, "SR1 = %u\n", SR1);
    fprintf(fp, "MSK1 = %08x\n", MSK1);
    fprintf(fp, "MSK2 = %08x\n", MSK2);
    fprintf(fp, "MSK3 = %08x\n", MSK3);
    fprintf(fp, "MSK4 = %08x\n", MSK4);
    //fprintf(fp, "SL7 = %u\n", SL7);
    //fprintf(fp, "SL8 = %u\n", SL8);
    //fprintf(fp, "SR1 = %u\n", SR1);
    //fprintf(fp, "SR2 = %u\n", SR2);
    //fprintf(fp, "SR3 = %u\n", SR3);
    //fprintf(fp, "SR4 = %u\n", SR4);
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

static inline void do_recursion(uint32_t a[4], uint32_t b[4], uint32_t c[4]) {
    uint64_t t;
    uint32_t bb[4];
    uint32_t aa[4];
    int i;

    t = ((uint64_t)b[1] << 32) | ((uint64_t)b[0]);
    if (SR1 == 0) {
	t = t << SL1;
    } else {
	t = t >> SL1;
    }
    bb[0] = (uint32_t)(t >> 32);
    bb[1] = (uint32_t)t;
    t = ((uint64_t)b[3] << 32) | ((uint64_t)b[2]);
    if (SR1 == 0) {
	t = t << SL1;
    } else {
	t = t >> SL1;
    }
    bb[2] = (uint32_t)(t >> 32);
    bb[3] = (uint32_t)t;
    for (i = 0; i < 4; i++) {
	aa[i] = a[i];
    }
    a[0] = aa[1] ^ b[1] ^ bb[0] ^ (c[0] & MSK1);
    a[1] = aa[2] ^ b[3] ^ bb[1] ^ (c[1] & MSK2);
    a[2] = aa[3] ^ b[0] ^ bb[2] ^ (c[2] & MSK3);
    a[3] = aa[0] ^ b[2] ^ bb[3] ^ (c[3] & MSK4);
}

static void gen_rand_all(void) {
    int i;

    for (i = 0; i < N; i++) {
	do_recursion(sfmt[i], sfmt[(i + POS1) % N], sfmt[(i + N - 1) % N]);
    }
}

/* for 128 bit check */
uint32_t gen_rand(void)
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
