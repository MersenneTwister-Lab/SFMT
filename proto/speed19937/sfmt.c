/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "random-common.h"

#define MEXP 19937

#define WORDSIZE 128
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

static uint32_t sfmt[N][4];
static unsigned int idx;

#define POS1 71
#define SL1 11
#define SL2 7
#define SR1 17

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
    fprintf(fp, "SL2 = %u\n", SL2);
    fprintf(fp, "SR1 = %u\n", SR1);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SR1] = [%u,%u,%u,%u]\n", 
	    POS1, SL1, SL2, SR1);
    fflush(fp);
}

void gen_rand_all(void) {
    int i;

    for (i = 0; i < 2; i++) {
	sfmt[i][0] = sfmt[i][0] ^ (sfmt[i][1] << SL1)
	    ^ sfmt[i + POS1][0] ^ (sfmt[i + POS1][1] >> SR1)
	    ^ sfmt[i + N - 2][0] ^ (sfmt[i + N - 2][3] << SL2);
	sfmt[i][1] = sfmt[i][1] ^ (sfmt[i][0] << SL1)
	    ^ sfmt[i + POS1][1] ^ (sfmt[i + POS1][2] >> SR1)
	    ^ sfmt[i + N - 2][1] ^ (sfmt[i + N - 2][0] << SL2);
	sfmt[i][2] = sfmt[i][2] ^ (sfmt[i][3] << SL1)
	    ^ sfmt[i + POS1][3] ^ (sfmt[i + POS1][3] >> SR1)
	    ^ sfmt[i + N - 2][2] ^ (sfmt[i + N - 2][1] << SL2);
	sfmt[i][3] = sfmt[i][3] ^ (sfmt[i][2] << SL1)
	    ^ sfmt[i + POS1][3] ^ (sfmt[i + POS1][0] >> SR1)
	    ^ sfmt[i + N - 2][3] ^ (sfmt[i + N - 2][2] << SL2);
    }
    for (; i < N - POS1; i++) {
	sfmt[i][0] = sfmt[i][0] ^ (sfmt[i][1] << SL1)
	    ^ sfmt[i + POS1][0] ^ (sfmt[i + POS1][1] >> SR1)
	    ^ sfmt[i - 2][0] ^ (sfmt[i - 2][3] << SL2);
	sfmt[i][1] = sfmt[i][1] ^ (sfmt[i][0] << SL1)
	    ^ sfmt[i + POS1][1] ^ (sfmt[i + POS1][2] >> SR1)
	    ^ sfmt[i - 2][1] ^ (sfmt[i - 2][0] << SL2);
	sfmt[i][2] = sfmt[i][2] ^ (sfmt[i][3] << SL1)
	    ^ sfmt[i + POS1][3] ^ (sfmt[i + POS1][3] >> SR1)
	    ^ sfmt[i - 2][2] ^ (sfmt[i - 2][1] << SL2);
	sfmt[i][3] = sfmt[i][3] ^ (sfmt[i][2] << SL1)
	    ^ sfmt[i + POS1][3] ^ (sfmt[i + POS1][0] >> SR1)
	    ^ sfmt[i - 2][3] ^ (sfmt[i - 2][2] << SL2);
    }
    for (; i < N; i++) {
	sfmt[i][0] = sfmt[i][0] ^ (sfmt[i][1] << SL1)
	    ^ sfmt[i + POS1 - N][0] ^ (sfmt[i + POS1 - N][1] >> SR1)
	    ^ sfmt[i - 2][0] ^ (sfmt[i - 2][3] << SL2);
	sfmt[i][1] = sfmt[i][1] ^ (sfmt[i][0] << SL1)
	    ^ sfmt[i + POS1 - N][1] ^ (sfmt[i + POS1 - N][2] >> SR1)
	    ^ sfmt[i - 2][1] ^ (sfmt[i - 2][0] << SL2);
	sfmt[i][2] = sfmt[i][2] ^ (sfmt[i][3] << SL1)
	    ^ sfmt[i + POS1 - N][3] ^ (sfmt[i + POS1 - N][3] >> SR1)
	    ^ sfmt[i - 2][2] ^ (sfmt[i - 2][1] << SL2);
	sfmt[i][3] = sfmt[i][3] ^ (sfmt[i][2] << SL1)
	    ^ sfmt[i + POS1 - N][3] ^ (sfmt[i + POS1 - N][0] >> SR1)
	    ^ sfmt[i - 2][3] ^ (sfmt[i - 2][2] << SL2);
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
