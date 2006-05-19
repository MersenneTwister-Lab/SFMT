/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */
#ifndef __HT_H__
#define __HT_H__

#include <stdint.h>
#include "types.h"

struct HT_RAND {
  uint32_t gx[NN]; /* the array for the state vector */
  unsigned int index; /* how many among gx[0]..gx[N-1] have been used */ 
};
typedef struct HT_RAND ht_rand;

void setup_param(unsigned int p_gmm, unsigned int p_gs0, unsigned int p_gs2, 
		 unsigned int p_gs3, unsigned int p_grot1, 
		 unsigned int p_grot2);
unsigned int get_rnd_maxdegree(void);
unsigned int get_rnd_mexp(void);
unsigned int get_rnd_nn(void);
void print_param(FILE *fp);

void init_gen_rand(ht_rand *ht, int seed);
uint32_t gen_rand(ht_rand *ht);
uint32_t get_lung(ht_rand *rand);
void next_state(ht_rand *rand);
void print_sequence(FILE *f, ht_rand *ht, unsigned int bitpos);
void dprint_ht(char *file, int line, char *s, ht_rand *ht);
void add(ht_rand *a, ht_rand *b);
void read_ht_random(FILE *f);
void print_ht_random(FILE *fp, ht_rand *ht);
void dprint_ht(char *file, int line, char *s, ht_rand *ht);
void dprintseq(char *file, int line, char *s, ht_rand *ht, unsigned int bitpos);

#endif
