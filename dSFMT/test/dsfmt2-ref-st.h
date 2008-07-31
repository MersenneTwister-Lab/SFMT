/* double precision SFMT ver. 2. */
#ifndef __DSFMT2_H__
#define __DSFMT2_H__

#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>
#include <stdio.h>

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 104
#define N ((MEXP - 128) / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N + 128)
#define N64 (N * 2)
#define N32 (N * 4)

#define LOW_MASK  UINT64_C(0x000FFFFFFFFFFFFF)
#define LOW_MASK32_1 0x000fffffU
#define LOW_MASK32_2 0xffffffffU
#define HIGH_CONST UINT64_C(0x3FF0000000000000)
#define HIGH_CONST32 0x3ff00000U
//#define HIGH_CONST UINT64_C(0x0000000000000000)
//#define HIGH_CONST32 0x00000000U

union W128_T {
    double d[2];
    uint64_t u[2];
};

typedef union W128_T w128_t;

struct DSFMT2_T {
    w128_t status[N + 1];
    int idx;
    int initialized;
};

typedef struct DSFMT2_T dsfmt2_t;

void setup_param(unsigned int p1, unsigned int p2, unsigned int p3, 
		 unsigned int p4, unsigned int p5, unsigned int p6,
		 unsigned int p7, unsigned int p8, unsigned int p9,
		 unsigned int p10, unsigned int p11, unsigned int p12,
		 unsigned int p13);
unsigned int get_rnd_maxdegree(void);
unsigned int get_rnd_mexp(void);
void print_param(FILE *fp);
void set_high_const(void);
void reset_high_const(void);
void initial_mask(dsfmt2_t *dsfmt);
void init_gen_rand(dsfmt2_t *dsfmt, uint32_t seed);
uint64_t gen_rand104sp(dsfmt2_t *dsfmt, uint64_t arrary[2], int mode);
void get_lung(dsfmt2_t *dsfmt, uint64_t lung[2]);
void add_rnd(dsfmt2_t *dist, dsfmt2_t *src);
void read_random_param(FILE *fp);
int period_certification(dsfmt2_t *dsfmt);
void get_fixed(uint64_t fix[]);
inline double genrand_close1_open2(dsfmt2_t *dsfmt);
char *get_idstring(void);
#endif
