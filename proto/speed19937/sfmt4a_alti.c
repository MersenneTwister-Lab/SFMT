/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "sfmt.h"

#define MEXP 19937

#define WORDSIZE 128
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

static vector unsigned int sfmt[N];
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

void print_state(FILE *fp) {
    int i;
    for (i = 0; i < N; i++) {
	fprintf(fp, "%08vlx ", sfmt[i]);
	if (i % 2 == 1) {
	    fprintf(fp, "\n");
	}
    }
}

void gen_rand_all(void) {
    int i;
    vector unsigned int a, b, c, r;

    a = sfmt[0];
    b = sfmt[POS1];
    c = sfmt[N - 1];
    r = vec_xor(vec_xor(
		    vec_xor(
			vec_sl(a, (vector unsigned int) SL1),
			a),
		    vec_xor(
			vec_sr(b, (vector unsigned int) SR1),
			vec_slo(b, (vector unsigned char)(4 << 3)))
		    ),
		vec_xor(
		    vec_sl(c, (vector unsigned int) SL2),
		    vec_sro(c, (vector unsigned char)(4 << 3)))
	);
    sfmt[0] = r;
    for (i = 1; i < N - POS1; i++) {
	a = sfmt[i];
	b = sfmt[i + POS1];
	c = r;
	r = vec_xor(vec_xor(
			vec_xor(
			    vec_sl(a, (vector unsigned int) SL1),
			    a),
			vec_xor(
			    vec_sr(b, (vector unsigned int) SR1),
			    vec_slo(b, (vector unsigned char)(4 << 3)))
			),
		    vec_xor(
			vec_sl(c, (vector unsigned int) SL2),
			vec_sro(c, (vector unsigned char)(4 << 3)))
	    );
	sfmt[i] = r;
    }
    for (; i < N; i++) {
	a = sfmt[i];
	b = sfmt[i + POS1 - N];
	c = r;
	r = vec_xor(vec_xor(
			vec_xor(
			    vec_sl(a, (vector unsigned int) SL1),
			    a),
			vec_xor(
			    vec_sr(b, (vector unsigned int) SR1),
			    vec_slo(b, (vector unsigned char)(4 << 3)))
			),
		    vec_xor(
			vec_sl(c, (vector unsigned int) SL2),
			vec_sro(c, (vector unsigned char)(4 << 3)))
	    );
	sfmt[i] = r;
    }
}

uint32_t gen_rand(void)
{
    uint32_t r;
    uint32_t *sfmtp = (uint32_t *)sfmt;

    if (idx >= N * 4) {
	gen_rand_all();
	idx = 0;
    }
    r = sfmtp[idx++];
    return r;
}

void init_gen_rand(uint32_t seed)
{
    int i;
    uint32_t *sfmtp = (uint32_t *)sfmt;

    sfmtp[0] = seed;
    for (i = 1; i < N * 4; i++) {
	sfmtp[i] = 1812433253UL 
	    * (sfmtp[i - 1] ^ (sfmtp[i - 1] >> 30)) + i;
    }
    idx = N * 4;
}
