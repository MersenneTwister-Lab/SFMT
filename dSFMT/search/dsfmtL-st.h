/* Simple and Fast MT 2006/1/21 */
#ifndef DSFMTL_ST_H
#define DSFMTL_ST_H

#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>
#include <stdio.h>

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 104
#define N ((MEXP - 128) / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N + 128)

struct DSFMT_T {
    uint64_t status[N + 1][2];
    int idx;
};

typedef struct DSFMT_T dsfmt_t;

void setup_param(uint32_t array[], int *index);
unsigned int get_rnd_maxdegree(void);
unsigned int get_rnd_mexp(void);
void get_params(int *sl1, int *sl2, uint64_t *msk1, uint64_t *msk2);
void print_param(FILE *fp);

void init_gen_rand(dsfmt_t *dsfmt, uint64_t seed);
uint64_t gen_rand104sp(dsfmt_t *dsfmt, uint64_t arrary[2], int mode);
void gen_rand104spar(dsfmt_t *dsfmt, uint64_t array[][2], int size);
void get_lung(dsfmt_t *dsfmt, uint64_t lung[2]);
void add_rnd(dsfmt_t *dist, dsfmt_t *src);
void read_random_param(FILE *fp);

#endif
