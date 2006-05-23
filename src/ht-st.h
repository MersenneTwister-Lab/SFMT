/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */
#ifndef __HT_H__
#define __HT_H__

#include <stdint.h>
#ifndef MEXP
#define MEXP 19937
#endif
#define WORDSIZE 32
#define N ((MEXP-1)/WORDSIZE)
#define NN (N+1) /* gx[N] is an additional state variable */
#define MAXDEGREE (WORDSIZE*NN)
#define DMAXDEGREE (MAXDEGREE*5+2)
#define LLMASK 0xffffffffffffffffULL

#define MIN(a,b) ((a)>(b)?(b):(a))

//typedef unsigned long long LLint64;
#define WORDLL (sizeof(uint64_t)*8) /* should be 64 */
#define MAXDEGWD (DMAXDEGREE/WORDLL+1)

struct HT_RAND {
  uint32_t gx[NN]; /* the array for the state vector */
  unsigned int index; /* how many among gx[0]..gx[N-1] have been used */ 
};
typedef struct HT_RAND ht_rand;

void setup_param(unsigned int p_gmm, unsigned int p_gs0, unsigned int p_gs2, 
		 unsigned int p_gs3, unsigned int p_grot1, 
		 unsigned int p_grot2, unsigned int dmy1, unsigned int dmy2,
		 unsigned int dmy3);
unsigned int get_rnd_maxdegree(void);
unsigned int get_rnd_mexp(void);
unsigned int get_rnd_nn(void);
void print_param(FILE *fp);
void print_param2(FILE *fp);

void init_gen_rand(ht_rand *ht, int seed);
uint32_t gen_rand32(ht_rand *ht);
uint32_t gen_rand128sp(ht_rand *rand, uint32_t array[4], uint32_t mode);
uint32_t get_lung(ht_rand *rand);
void print_sequence(FILE *f, ht_rand *ht, unsigned int bitpos);
void dprint_ht(char *file, int line, char *s, ht_rand *ht);
void add_rnd(ht_rand *a, ht_rand *b);
void read_random_param(FILE *f);
void print_ht_random(FILE *fp, ht_rand *ht);
void dprint_ht(char *file, int line, char *s, ht_rand *ht);
void dprintseq(char *file, int line, char *s, ht_rand *ht, unsigned int bitpos);

#endif
