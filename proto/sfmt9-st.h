/* Simple and Fast MT 2006/1/21 */
#ifndef __SFMT_H__
#define __SFMT_H__

#include <stdint.h>
#include <stdio.h>

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 32
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

struct SFMT9_T {
    uint32_t sfmt[N];
    unsigned int idx;
};

typedef struct SFMT9_T sfmt9_t;

void setup_param(unsigned int p1, unsigned int p2, unsigned int p3, 
		 unsigned int p4, unsigned int p5, unsigned int p6,
		 unsigned int p7, unsigned int p8, unsigned int p9);
unsigned int get_rnd_maxdegree(void);
unsigned int get_rnd_mexp(void);
void print_param(FILE *fp);
void print_param2(FILE *fp);

void init_gen_rand(sfmt9_t *sfmt, uint32_t seed);
uint32_t gen_rand(sfmt9_t *sfmt);
void next_state(sfmt9_t *sfmt);
void add_state(sfmt9_t *dist, const sfmt9_t *src);
#endif
