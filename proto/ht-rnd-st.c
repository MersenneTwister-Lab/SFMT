/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ht-st.h"
#include "debug.h"

static unsigned int gmm;
static unsigned int gs0;
static unsigned int gs2;
static unsigned int gs3;
static unsigned int grot1;
static unsigned int grot2;

unsigned int get_rnd_maxdegree(void)
{
  return MAXDEGREE;
}

unsigned int get_rnd_mexp(void)
{
  return MEXP;
}

void setup_param(unsigned int p_gmm, unsigned int p_gs0, unsigned int p_gs2, 
		 unsigned int p_gs3, unsigned int p_grot1, 
		 unsigned int p_grot2) {
  gmm = p_gmm;
  gs0 = p_gs0;
  gs2 = p_gs2;
  gs3 = p_gs3;
  grot1 = p_grot1;
  grot2 = p_grot2;
}

void print_param(FILE *fp) {
  fprintf(fp, "gmm = %u\n", gmm);
  fprintf(fp, "gs2 = %u\n", gs2);
  fprintf(fp, "gs3 = %u\n", gs3);
  fprintf(fp, "grot1 = %u\n", grot1);
  fprintf(fp, "grot2 = %u\n", grot2);
  fflush(fp);
}

void next_state(ht_rand *rand) {
  uint32_t u;
  int jump_index;

  rand->index++;
  if (rand->index >= N) {
    rand->index = 0;
  }
  jump_index = rand->index + gmm;
  if (jump_index >= N) {
    jump_index -= N;
  }
  u = rand->gx[N];
  u ^= (rand->gx[rand->index] >> grot1) 
    ^ (rand->gx[rand->index] << grot2);
  u ^= rand->gx[jump_index];
  u ^= u << gs2;
  rand->gx[rand->index] ^=  u ^ (u << gs3);
  rand->gx[N] = u;
}

uint32_t gen_rand(ht_rand *rand)
{
  next_state(rand);
  return rand->gx[rand->index];
}

/*
unsigned uint32_t peek_next(ht_rand *rand) {
  return rand->gx[rand->index];
}
*/

void init_gen_rand(ht_rand *rand, int seed)
{
  int i;
  for (i=0; i < NN; i++) {
    rand->gx[i] = (uint32_t)(seed + i);
    seed = seed * 31415UL;
  }
  rand->index = N-1;

  for (i=0; i < MEXP * 3; i++){
    next_state(rand);
  }
  next_state(rand);
}

/* for debug */
void print_sequence(FILE *f, ht_rand *ht, unsigned int bitpos) {
  ht_rand copy;
  unsigned int mask = 1U << bitpos;
  int i;

  copy = *ht;
  for (i = 0; i < MAXDEGREE; i++) {
    if ((gen_rand(&copy) & mask) != 0) {
      fputc('1', f);
    } else {
      fputc('0', f);
    }
    if ((i % WORDLL) == WORDLL - 1) {
      fprintf(f, "\n");
    }
  }
  fputc('\n', f);
}

void add(ht_rand *a, ht_rand *b) {
  int i;
  int ap;
  int bp;

  ap = a->index;
  bp = b->index;
  for (i = 0; i < N; i++) {
    if (ap >= N) {
      ap = 0;
    }
    if (bp >= N) {
      bp = 0;
    }
    a->gx[ap++] ^= b->gx[bp++];
  }
  a->gx[N] ^= b->gx[N];
}


unsigned int get_uint(char *line);
unsigned int get_uint(char *line) {
  unsigned int result;

  for (;(*line) && (*line != '=');line++);
  if (!*line) {
    fprintf(stderr, "WARN:can't get = in get_uint\n");
    return 0;
  }
  line++;
  errno = 0;
  result = (unsigned int)strtol(line, NULL, 10);
  if (errno) {
    fprintf(stderr, "WARN:format error:%s", line);
  }
  return result;
}

void read_ht_random(FILE *f) {
  char line[256];

  fgets(line, 256, f);
  fgets(line, 256, f);
  fgets(line, 256, f);
  gmm = get_uint(line);
  fgets(line, 256, f);
  gs2 = get_uint(line);
  fgets(line, 256, f);
  gs3 = get_uint(line);
  fgets(line, 256, f);
  grot1 = get_uint(line);
  fgets(line, 256, f);
  grot2 = get_uint(line);
}

void print_ht_random(FILE *fp, ht_rand *ht) {
  int i, j;

  fprintf(fp, "index = %u ", ht->index);
  fprintf(fp, "gfsr:\n");
  for (i = 0; i < NN; i++) {
    for (j = 31; j >= 0; j--) {
      if ((ht->gx[i] & (1 << j)) != 0) {
	fprintf(fp, "%c", '1');
      } else {
	fprintf(fp, "%c", '0');
      }
    }
    fprintf(fp, "\n");
  }
}

void dprint_ht(char *file, int line, char *s, ht_rand *ht) {
  fprintf(stderr, "%s:%d %s", file, line, s);
  print_ht_random(stderr, ht);
}

void dprintseq(char *file, int line, char *s, ht_rand *ht, unsigned int bitpos)
{
  fprintf(stderr, "%s:%d %s\n", file, line, s);
  if (ht == NULL) {
    fprintf(stderr, "NULL\n");
  } else {
    print_sequence(stderr, ht, bitpos);
  }
}
