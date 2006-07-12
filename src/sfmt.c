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
static unsigned int SL1 = 11;
static unsigned int SR1 = 7;
static unsigned int MSK1 = 7;
static unsigned int MSK2 = 7;
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
    SL1 = p2 % (32 - 1) + 1;
    SR1 = p3 % (32 - 1) + 1;
    MSK1= p4 | p5;
    MSK2= p6 | p7;
    MSK3= p8 | p9;
    MSK4= p10 | p11;
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
    fprintf(fp, "SL1 = %u\n", SL1);
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
    fprintf(fp, "[POS1, SL1, SR1, MSK1, MSK2, MSK3, MSK4] = "
	    "[%u,%u,%u,%u,%u,%u,%u]\n", 
	    POS1, SL1, SR1, MSK1, MSK2, MSK3, MSK4);
    fflush(fp);
}

static inline void do_recursion(uint32_t a[4], uint32_t b[4], uint32_t c[4]) {
    a[0] = a[0] ^ (b[1] & MSK1) ^ (b[0] << SL1) ^ (c[0] >> SR1);
    a[1] = a[1] ^ (b[2] & MSK2) ^ (b[1] << SL1) ^ (c[1] >> SR1);
    a[2] = a[2] ^ (b[3] & MSK3) ^ (b[2] << SL1) ^ (c[2] >> SR1);
    a[3] = a[3] ^ (b[0] & MSK4) ^ (b[3] << SL1) ^ (c[3] >> SR1);
}

static void gen_rand_all(void) {
    int i;

    for (i = 0; i < N; i++) {
	do_recursion(sfmt[i], sfmt[(i + POS1) % N], sfmt[(i + N - 1) % N]);
    }
}

uint64_t gen_rand128(uint64_t *hi, uint64_t *low)
{
    uint32_t i;

    if (idx >= N * 4) {
	gen_rand_all();
	idx = 0;
    }
    i = idx / 4;
    *low = (uint64_t)sfmt[i][0] | ((uint64_t)sfmt[i][1] << 32);
    *hi = (uint64_t)sfmt[i][2] | ((uint64_t)sfmt[i][3] << 32);
    idx += 4;
    idx = (idx / 4) * 4;
    return *hi;
}

uint32_t gen_rand32(void)
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

/* for 128 bit check */
uint32_t gen_rand(void)
{
    uint32_t r;

    if (idx >= N * 4) {
	gen_rand_all();
	idx = 0;
    }
    r = sfmt[idx / 4][0];
    idx += 4;
    return r;
}

/* for 128 bit check */
void init_gen_rand(uint32_t seed)
{
    int i;

    sfmt[0][0] = seed;
    for (i = 1; i < N * 4; i++) {
	sfmt[i / 4][i % 4] = 1812433253UL 
	    * (sfmt[(i - 1) / 4][(i - 1) % 4]
	       ^ (sfmt[(i - 1) / 4][(i - 1) % 4] >> 30)) 
	    + i;
    }
    idx = 0;
}
