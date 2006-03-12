/* SFMT Search Code, M.Saito 2006/2/28 */
/* (1234) 巡回置換１回のみ */
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "sfmt9-st.h"

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
		 unsigned int p7, unsigned int p8, unsigned int p9) {
    p1 = p1 % (N - 4);
    if (p1 < 4) {
	p1 += 4;
    }
    POS1 = p1;
    SL1 = p2 % 32;
    SL2 = p3 % 32;
    SR1 = p4 % 32;
}

void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    fprintf(fp, "SL2 = %u\n", SL2);
    fprintf(fp, "SR1 = %u\n", SR1);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "PARAMS[POS1, SL1, SL2, SR1] = [%u,%u,%u,%u]\n", 
	    POS1, SL1, SL2, SR1);
    fflush(fp);
}

void next_state(sfmt9_t *sfmt) {
    sfmt->idx++;
    if (sfmt->idx >= N) {
	sfmt->idx = 0;
    }
    sfmt->sfmt[sfmt->idx] = (sfmt->sfmt[sfmt->idx] << SL1) 
	^ sfmt->sfmt[sfmt->idx]
	^ (sfmt->sfmt[(sfmt->idx + POS1) % N] << SL2) 
	^ sfmt->sfmt[(sfmt->idx + POS1) % N]
	^ (sfmt->sfmt[(sfmt->idx + N - 4) % N] >> SR1) 
	^ sfmt->sfmt[(sfmt->idx + N - 4) % N];
}

uint32_t gen_rand(sfmt9_t *sfmt)
{
    next_state(sfmt);
    return sfmt->sfmt[sfmt->idx];
}

void init_gen_rand(sfmt9_t *sfmt, uint32_t seed)
{
    int i;

    for (i = 0; i < N; i++) {
	sfmt->sfmt[i] = seed;
	seed = 1812433253UL * (seed ^ (seed >> 30)) + i;
    }
    sfmt->idx = 0;
}

void add_state(sfmt9_t *dist, const sfmt9_t *src) {
    uint32_t i;
    uint32_t k;

    k = (N + src->idx - dist->idx) % N;
    for (i = 0; i < N; i++) {
	dist->sfmt[i] ^= src->sfmt[(i + k) % N];
    }
}
