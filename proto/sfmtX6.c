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
static unsigned int SL2 = 11;
static unsigned int SL3 = 11;
static unsigned int SR1 = 7;
static unsigned int SR2 = 7;
static unsigned int M1 = 7;
static unsigned int M2 = 7;
static unsigned int M3 = 7;
static unsigned int M4 = 7;
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
    SL2 = (p3 % 4) * 2 + 1; 
    SL3 = p4 % (32 -1) + 1; 
    SR1 = p5 % (32 -1) + 1; 
    SR2 = p6 % (32 -1) + 1;
    M1 = p7 % 4;
    M2 = p8 % 4;
    M3 = p9 % 4;
    M4 = p10 % 4;
    memset(sfmt, 0, sizeof(sfmt));
}

void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    fprintf(fp, "SL2 = %u\n", SL2);
    fprintf(fp, "SL3 = %u\n", SL3);
    fprintf(fp, "SR1 = %u\n", SR1);
    fprintf(fp, "SR2 = %u\n", SR2);
    fprintf(fp, "M1 = %u\n", M1);
    fprintf(fp, "M2 = %u\n", M2);
    fprintf(fp, "M3 = %u\n", M3);
    fprintf(fp, "M4 = %u\n", M4);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SL3, SL4, SL5, SL6, SL7, SL8,"
	    " SR1, SR2, SR3, SR4] = "
	    "[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u]\n", 
	    POS1, SL1, SR1, M1, M2, M3, M4, 0, 0, 
	    0, 0, 0, 0);
    fflush(fp);
}

static inline void rshift128(uint32_t out[4], const uint32_t in[4],
			     int shift) {
    uint64_t t1, t2, o1, o2;

    t1 = ((uint64_t)in[1] << 32) | ((uint64_t)in[0]);
    t2 = ((uint64_t)in[3] << 32) | ((uint64_t)in[2]);

    o1 = t1 >> (shift * 8);
    o1 |= t2 << (64 - shift * 8);
    o2 = t2 >> (shift * 8);
    out[1] = (uint32_t)(o1 >> 32);
    out[0] = (uint32_t)o1;
    out[3] = (uint32_t)(o2 >> 32);
    out[2] = (uint32_t)o2;
}

static inline void lshift128(uint32_t out[4], const uint32_t in[4],
			     int shift) {
    uint64_t t1, t2, o1, o2;

    t1 = ((uint64_t)in[1] << 32) | ((uint64_t)in[0]);
    t2 = ((uint64_t)in[3] << 32) | ((uint64_t)in[2]);

    o1 = t1 << (shift * 8);
    o2 = t2 << (shift * 8);
    o2 |= t1 >> (64 - shift * 8);
    out[1] = (uint32_t)(o1 >> 32);
    out[0] = (uint32_t)o1;
    out[3] = (uint32_t)(o2 >> 32);
    out[2] = (uint32_t)o2;
}

static inline void do_recursion(uint32_t a[4], uint32_t b[4], uint32_t c[4]) {
    uint32_t x[4];

    //lshift128(x, a, 1);
    a[0] = a[M1] ^ (a[0] << SL1) ^ b[1] ^ (b[0] << SL2) ^ c[0] ^ (c[0] >> SR1);
    a[1] = a[M2] ^ (a[1] << SL1) ^ b[2] ^ (b[1] << SL2) ^ c[1] ^ (c[1] >> SR1);
    a[2] = a[M3] ^ (a[2] << SL1) ^ b[3] ^ (b[2] << SL2) ^ c[2] ^ (c[2] >> SR1);
    a[3] = a[M4] ^ (a[3] << SL1) ^ b[0] ^ (b[3] << SL2) ^ c[3] ^ (c[3] >> SR1);
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
