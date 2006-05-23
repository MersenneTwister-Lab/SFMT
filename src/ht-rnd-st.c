/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ht-st.h"
//#include "debug.h"

static void next_state1(ht_rand *rand);
static void next_state(ht_rand *rand);
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

unsigned int get_rnd_nn(void) {
    return NN;
}

void setup_param(unsigned int p_gmm, unsigned int p_gs0, unsigned int p_gs2, 
		 unsigned int p_gs3, unsigned int p_grot1, 
		 unsigned int p_grot2, unsigned int dmy1, unsigned int dmy2,
		 unsigned int dmy3) {
    gmm = p_gmm % (N - 1) + 1;
    gs0 = p_gs0 % (32 - 1) + 1;
    gs2 = p_gs2 % (32 - 1) + 1;
    gs3 = p_gs3 % (32 - 1) + 1;
    grot1 = p_grot1 % (32 - 1) + 1;
    grot2 = p_grot2 % (32 - 1) + 1;
}

void print_param(FILE *fp) {
    fprintf(fp, "gmm = %u\n", gmm);
    fprintf(fp, "gs2 = %u\n", gs2);
    fprintf(fp, "gs3 = %u\n", gs3);
    fprintf(fp, "grot1 = %u\n", grot1);
    fprintf(fp, "grot2 = %u\n", grot2);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[GMM, GS2, GS3, GROT1, GROT2] = "
            "[%u,%u,%u,%u,%u]＼n", 
            gmm, gs2, gs3, grot1, grot2);
    fflush(fp);
}

static void next_state1(ht_rand *rand) {
    uint32_t u;

    u = rand->gx[N];
    u ^= (rand->gx[rand->index % N] >> grot1) 
	^ (rand->gx[rand->index % N] << grot2);
    u ^= rand->gx[(rand->index + gmm) % N];
    u ^= u << gs2;
    rand->gx[rand->index % N] ^=  u ^ (u << gs3);
    rand->gx[N] = u;
}

static void next_state(ht_rand *rand) {
    uint32_t u;
    uint32_t i;

    u = rand->gx[N];
    for (i = rand->index; i < rand->index + 4; i++) {
	u ^= (rand->gx[i % N] >> grot1) 
	    ^ (rand->gx[i % N] << grot2);
	u ^= rand->gx[(i + gmm) % N];
	u ^= u << gs2;
	rand->gx[i % N] ^=  u ^ (u << gs3);
    }
    rand->gx[N] = u;
}

/* これは初期状態を出力する */
uint32_t gen_rand128sp(ht_rand *rand, uint32_t array[4], uint32_t mode)
{
    uint32_t i;

    i = rand->index + mode;
    array[0] = rand->gx[i];
    array[1] = rand->gx[(i + 1) % N];
    array[2] = rand->gx[(i + 2) % N];
    array[3] = rand->gx[(i + 3) % N];

    next_state(rand);
    rand->index = (rand->index + 4) % N;
    return array[0];
}

uint32_t gen_rand32(ht_rand *rand)
{
    uint32_t r;

    r = rand->gx[rand->index];
    next_state1(rand);
    rand->index = (rand->index + 1) % N;
    return r;
}

#if 0
uint32_t get_lung(ht_rand *rand)
{
    //next_state(rand);
    return rand->gx[N];
}
#endif

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
    rand->index = 0;

#if 0
    for (i=0; i < MEXP * 3; i++){
	next_state(rand);
    }
    next_state(rand);
#endif
}

#if 0
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
#endif

void add_rnd(ht_rand *a, ht_rand *b) {
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

void read_random_param(FILE *f) {
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
#if 0
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
#endif
