/* Simple and Fast MT 2006/1/21 */
#ifndef __SFMT_ST_H__
#define __SFMT_ST_H__

#include <stdint.h>
#include <stdio.h>

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 128
#define NN (MEXP / WORDSIZE + 1)
#define N (NN - 1)
#define MAXDEGREE (WORDSIZE * NN)

struct SFMT_TAG {
    uint32_t sfmt[NN][4];
    uint32_t idx;
};

typedef struct SFMT_TAG sfmt_t;

void setup_param(unsigned int p1, unsigned int p2, unsigned int p3, 
		 unsigned int p4, unsigned int p5, unsigned int p6,
		 unsigned int p7, unsigned int p8, unsigned int p9,
		 unsigned int p10, unsigned int p11, unsigned int p12,
		 unsigned int p13);
unsigned int get_rnd_maxdegree(void);
unsigned int get_rnd_mexp(void);
void print_param(FILE *fp);
void print_param2(FILE *fp);

void init_gen_rand(sfmt_t *sfmt, uint32_t seed);
uint32_t gen_rand32(sfmt_t *sfmt);
uint64_t gen_rand64(sfmt_t *sfmt);
uint64_t gen_rand128(sfmt_t *sfmt, uint64_t *hi, uint64_t *low);
uint32_t gen_rand128sp(sfmt_t *sfmt, uint32_t arrary[4], uint32_t mode);
void get_lung(sfmt_t *sfmt, uint64_t *hi, uint64_t *low);
void add_rnd(sfmt_t *dist, sfmt_t *src);
void read_random_param(FILE *fp);

#endif
