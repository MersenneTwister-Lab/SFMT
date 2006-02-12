/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

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

static uint64_t sfmt[N][2];
static unsigned int index;

#define POS1 71
#define SL1 1
#define SL2 5
#define SR1 14

static void gen_rand_all(void);

unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    fprintf(fp, "SL2 = %u\n", SL2);
    fprintf(fp, "SR1 = %u\n", SR1);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SR1] = [%u,%u,%u,%u]\n", 
	    POS1, SL1, SL2, SR1);
    fflush(fp);
}

static void gen_rand_all(void) {
    int i;

    sfmt[0][0] = sfmt[0][0] ^ (sfmt[0][0] << SL1)
	^ sfmt[POS1][0] ^ (sfmt[POS1][1] >> SR1)
	^ sfmt[N - 1][0] ^ (sfmt[N - 1][1] << SL2);
    sfmt[0][1] = sfmt[0][1] ^ (sfmt[0][0] << SL1)
	^ sfmt[POS1][1] ^ (sfmt[POS1][0] >> SR1)
	^ sfmt[N - 1][1] ^ (sfmt[N - 1][0] << SL2);
    for (i = 1; i < N - POS1; i++) {
	sfmt[i][0] = sfmt[i][0] ^ (sfmt[i][0] << SL1)
	    ^ sfmt[i + POS1][0] ^ (sfmt[i + POS1][1] >> SR1)
	    ^ sfmt[i - 1][0] ^ (sfmt[i - 1][1] << SL2);
	sfmt[i][1] = sfmt[i][1] ^ (sfmt[i][0] << SL1)
	    ^ sfmt[i + POS1][1] ^ (sfmt[i + POS1][0] >> SR1)
	    ^ sfmt[i - 1][1] ^ (sfmt[i - 1][0] << SL2);
    }
    for (i = 1; i < N; i++) {
	sfmt[i][0] = sfmt[i][0] ^ (sfmt[i][0] << SL1)
	    ^ sfmt[i + POS1 - N][0] ^ (sfmt[i + POS1 - N][1] >> SR1)
	    ^ sfmt[i - 1][0] ^ (sfmt[i - 1][1] << SL2);
	sfmt[i][1] = sfmt[i][1] ^ (sfmt[i][0] << SL1)
	    ^ sfmt[i + POS1 - N][1] ^ (sfmt[i + POS1 - N][0] >> SR1)
	    ^ sfmt[i - 1][1] ^ (sfmt[i - 1][0] << SL2);
    }
}

uint32_t gen_rand(void)
{
    uint32_t r;

    if (index >= N * 4) {
	gen_rand_all();
	index = 0;
    }
    r = ((uint32_t *)sfmt)[index++];
    return r;
}

void init_gen_rand(uint32_t seed)
{
    int i;

    ((uint32_t *)sfmt)[0] = seed;
    for (i = 1; i < N * 4; i++) {
	((uint32_t *)sfmt)[i] = 1812433253UL 
	    * (((uint32_t *)sfmt)[i - 1]
	       ^ (((uint32_t *)sfmt)[i - 1] >> 30))  + i;
    }
    index = N * 4;
}
