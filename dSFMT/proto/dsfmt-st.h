/* Simple and Fast MT 2006/1/21 */
#ifndef __DSFMT_H__
#define __DSFMT_H__

#include <inttypes.h>
#include <stdio.h>

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 104
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

struct DSFMT_T {
    uint64_t status[N][2];
    unsigned int idx;
};

typedef struct DSFMT_T dsfmt_t;

void setup_param(unsigned int p1, unsigned int p2, unsigned int p3, 
		 unsigned int p4, unsigned int p5, unsigned int p6,
		 unsigned int p7, unsigned int p8, unsigned int p9,
		 unsigned int p10, unsigned int p11, unsigned int p12,
		 unsigned int p13);
unsigned int get_rnd_maxdegree(void);
unsigned int get_rnd_mexp(void);
void print_param(FILE *fp);
void print_param2(FILE *fp);

void init_gen_rand(dsfmt_t *dsfmt, uint64_t seed);
uint64_t gen_rand104sp(dsfmt_t *dsfmt, uint64_t arrary[2], int mode);
uint64_t get_lung(dsfmt_t *dsfmt);
void add_rnd(dsfmt_t *dist, dsfmt_t *src);
void read_random_param(FILE *fp);

#endif
