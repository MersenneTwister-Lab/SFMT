/* SFMT Search Code, M.Saito 2006/2/28 */
/* (1234) 巡回置換１回のみ */
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
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
static unsigned int SL2 = 7;
static unsigned int SL3 = 7;
static unsigned int SL4 = 7;
static unsigned int SR1 = 17;
static unsigned int SR2 = 9;
static unsigned int SR3 = 9;
static unsigned int SR4 = 9;

static void gen_rand_all(void);

unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE + 4;
}

unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

void setup_param(unsigned int p1, unsigned int p2, unsigned int p3, 
		 unsigned int p4, unsigned int p5, unsigned int p6,
		 unsigned int p7, unsigned int p8, unsigned int p9) {
    POS1 = p1 % N;
    SL1 = p2 % 32;
    SL2 = p3 % 32;
    SL3 = p4 % 32;
    SL4 = p5 % 32;
    SR1 = p6 % 32;
    SR2 = p7 % 32;
    SR3 = p8 % 32;
    SR4 = p9 % 32;
    memset(sfmt, 0, sizeof(sfmt));
}

void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    fprintf(fp, "SL2 = %u\n", SL2);
    fprintf(fp, "SL3 = %u\n", SL3);
    fprintf(fp, "SL4 = %u\n", SL4);
    fprintf(fp, "SR1 = %u\n", SR1);
    fprintf(fp, "SR2 = %u\n", SR2);
    fprintf(fp, "SR3 = %u\n", SR3);
    fprintf(fp, "SR4 = %u\n", SR4);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SL3, SL4, SR1, SR2, SR3, SR4] = "
	    "[%u,%u,%u,%u,%u,%u,%u,%u,%u]\n", 
	    POS1, SL1, SL2, SL3, SL4, SR1, SR2, SR3, SR4);
    fflush(fp);
}

static void gen_rand_all(void) {
    int i;

    for (i = 0; i < N; i++) {
	sfmt[i][0] = (sfmt[i][0] << SL1) ^ sfmt[i][0]
	    ^ sfmt[(i + POS1) % N][1]
	    ^ (sfmt[(i + N - 1) % N][0] >> SR1) ^ sfmt[(i + N - 1) % N][0];
	sfmt[i][1] = (sfmt[i][1] << SL2) ^ sfmt[i][1]
	    ^ sfmt[(i + POS1) % N][2]
	    ^ (sfmt[(i + N - 1) % N][1] >> SR2) ^ sfmt[(i + N - 1) % N][1];
	sfmt[i][2] = (sfmt[i][2] << SL3) ^ sfmt[i][2]
	    ^ sfmt[(i + POS1) % N][3]
	    ^ (sfmt[(i + N - 1) % N][2] >> SR3) ^ sfmt[(i + N - 1) % N][2];
	sfmt[i][3] = (sfmt[i][3] << SL4) ^ sfmt[i][3]
	    ^ sfmt[(i + POS1) % N][0]
	    ^ (sfmt[(i + N - 1) % N][3] >> SR4) ^ sfmt[(i + N - 1) % N][3];
    }
}

/* for 128 bit check */
uint64_t gen_rand128(uint64_t *hi, uint64_t *low)
{
    uint32_t i;

    assert(idx % 4 == 0);

    if (idx >= N * 4) {
	gen_rand_all();
	idx = 0;
    }
    i = idx / 4;
    *low = (uint64_t)sfmt[i][0] | ((uint64_t)sfmt[i][1] << 32);
    *hi = (uint64_t)sfmt[i][2] | ((uint64_t)sfmt[i][3] << 32);
    idx += 4;
    return *hi;
}

uint64_t gen_rand64(void)
{
    uint64_t r;
    uint32_t i;

    assert(idx % 2 == 0);
 
    if (idx >= N * 4) {
	gen_rand_all();
	idx = 0;
    }
    i = idx / 4;
    r = (uint64_t)sfmt[i][idx % 4] | ((uint64_t)sfmt[i][idx % 4 + 1] << 32);
    idx += 2;
    return r;
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
