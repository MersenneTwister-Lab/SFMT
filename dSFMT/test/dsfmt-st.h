/* Simple and Fast MT 2006/1/21 */
#ifndef __DSFMT_H__
#define __DSFMT_H__

#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>
#include <stdio.h>

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 104
#define N (MEXP / WORDSIZE)
#define MAXDEGREE (WORDSIZE * (N + 1))

struct DSFMT_T {
    uint64_t status[N + 1][2];
    int idx;
};

typedef struct DSFMT_T dsfmt_t;

void setup_param(uint32_t array[], int *index);
unsigned int get_rnd_maxdegree(void);
unsigned int get_rnd_mexp(void);
void print_param(FILE *fp);

void init_gen_rand(dsfmt_t *dsfmt, uint32_t seed);
uint64_t gen_rand104sp(dsfmt_t *dsfmt, uint64_t arrary[2], int mode);
void gen_rand104spar(dsfmt_t *dsfmt, uint64_t array[][2], int size);
void get_lung(dsfmt_t *dsfmt, uint64_t lung[2]);
void add_rnd(dsfmt_t *dist, dsfmt_t *src);
void read_random_param(FILE *fp);
double genrand_close1_open2(dsfmt_t *dsfmt);
void set_pcv(uint64_t v[2]);
int period_certification(dsfmt_t *dsfmt);
void get_fixed(uint64_t fix[2]);
void initial_mask(dsfmt_t *dsfmt);
void set_high_const(void);
void reset_high_const(void);
char *get_idstring(void);
#endif
