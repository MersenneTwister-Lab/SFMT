/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sfmt-cls.h"
//#include "debug.h"

static unsigned int POS1 = 1;
static unsigned int SL1 = 11;
static unsigned int SL2 = 7;
static unsigned int SL3 = 7;
static unsigned int SL4 = 7;
static unsigned int SL5 = 7;
static unsigned int SL6 = 7;
static unsigned int SL7 = 7;
static unsigned int SL8 = 7;
static unsigned int SR1 = 17;
static unsigned int SR2 = 17;
static unsigned int SR3 = 17;
static unsigned int SR4 = 17;

unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

void setup_param(unsigned int p1, unsigned int p2, unsigned int p3, 
		 unsigned int p4, unsigned int p5, unsigned int p6,
		 unsigned int p7, unsigned int p8, unsigned int p9,
		 unsigned int p10, unsigned int p11, unsigned int p12,
		 unsigned int p13) {
    POS1 = p1 % (N-2) + 1;
    SL1 = p2 % (32 - 1) + 1;
    SL2 = p3 % (32 - 1) + 1;
    SL3 = p4 % (32 - 1) + 1;
    SL4 = p5 % (32 - 1) + 1;
    SL5 = p6 % (32 - 1) + 1;
    SL6 = p7 % (32 - 1) + 1;
    SL7 = p8 % (32 - 1) + 1;
    SL8 = p9 % (32 - 1) + 1;
    SR1 = p10 % (32 - 1) + 1;
    SR2 = p11 % (32 - 1) + 1;
    SR3 = p12 % (32 - 1) + 1;
    SR4 = p13 % (32 - 1) + 1;
}

void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    fprintf(fp, "SL2 = %u\n", SL2);
    fprintf(fp, "SL3 = %u\n", SL3);
    fprintf(fp, "SL4 = %u\n", SL4);
    fprintf(fp, "SL5 = %u\n", SL5);
    fprintf(fp, "SL6 = %u\n", SL6);
    fprintf(fp, "SL7 = %u\n", SL7);
    fprintf(fp, "SL8 = %u\n", SL8);
    fprintf(fp, "SR1 = %u\n", SR1);
    fprintf(fp, "SR2 = %u\n", SR2);
    fprintf(fp, "SR3 = %u\n", SR3);
    fprintf(fp, "SR4 = %u\n", SR4);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SL3, SL4, SL5, SL6, SL7, SL8,"
	    " SR1, SR2, SR3, SR4] = "
	    "[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u]\n", 
	    POS1, SL1, SL2, SL3, SL4, SL5, SL6, SL7, SL8, 
	    SR1, SR2, SR3, SR4);
    fflush(fp);
}

/* for 128 bit */
void SFMT::next_state(void) {

    if (++idx >= N) {
	idx = 0;
    }
    sfmt[idx][0] = (sfmt[idx][0] << SL1) ^ sfmt[idx][0]
	^ (sfmt[(idx + POS1) % N][0] >> SR1) ^ sfmt[(idx + POS1) % N][1]
	^ (sfmt[(idx + N - 1) % N][0] << SL5); 
    sfmt[idx][1] = (sfmt[idx][1] << SL2) ^ sfmt[idx][1]
	^ (sfmt[(idx + POS1) % N][1] >> SR2) ^ sfmt[(idx + POS1) % N][2]
	^ (sfmt[(idx + N - 1) % N][1] << SL6) ^ sfmt[(idx + N -1) % N][0];
    sfmt[idx][2] = (sfmt[idx][2] << SL3) ^ sfmt[idx][2]
	^ (sfmt[(idx + POS1) % N][2] >> SR3) ^ sfmt[(idx + POS1) % N][3]
	^ (sfmt[(idx + N - 1) % N][2] << SL7) ^ sfmt[(idx + N -1) % N][1];
    sfmt[idx][3] = (sfmt[idx][3] << SL4) ^ sfmt[idx][3]
	^ (sfmt[(idx + POS1) % N][3] >> SR4)
	^ (sfmt[(idx + N - 1) % N][3] << SL8) ^ sfmt[(idx + N -1) % N][2];
}

/* beheave as if it were LITTLE ENDIAN */
vec_GF2& SFMT::gen_rand(vec_GF2& vec, uint32_t len)
{
    uint32_t i, j, k;
    uint32_t tmp;

    next_state();
    k = 0;
    vec.SetLength(len);
    for (i = 0; i < 4; i++) {
	tmp = sfmt[idx][i];
	for (j = 0; j < 32; j++) {
	    vec.put(k, tmp & 1);
	    tmp = tmp >> 1;
	    if (++k >= len) {
		goto owari;
	    }
	}
    }
owari:
    return vec;
}

/* for 128 bit */
void SFMT::init_gen_rand(uint32_t seed)
{
    int i;

    sfmt[0][0] = seed;
    for (i = 1; i < N * 4; i++) {
	sfmt[i/4][i%4] = 1812433253UL 
	    * (sfmt[(i - 1) / 4][(i - 1) % 4]
	       ^ (sfmt[(i - 1) / 4][(i - 1) % 4] >> 30)) 
	    + i;
    }
    idx = N; // this line is for 128 bit.
}

void SFMT::add(const SFMT& src) {
    int i, j;

    for (i = 0; i < N; i ++) {
	for (j = 0; j < 4; j++) {
	    sfmt[i][j] ^= src.sfmt[(i + N + src.idx - idx) % N][j];
	}
    }
}

static unsigned int get_uint(char *line);
static unsigned int get_uint(char *line) {
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
    POS1 = get_uint(line);
    fgets(line, 256, f);
    SL1 = get_uint(line);
    fgets(line, 256, f);
    SL2 = get_uint(line);
    fgets(line, 256, f);
    SL3 = get_uint(line);
    fgets(line, 256, f);
    SL4 = get_uint(line);
    fgets(line, 256, f);
    SL5 = get_uint(line);
    fgets(line, 256, f);
    SL6 = get_uint(line);
    fgets(line, 256, f);
    SL7 = get_uint(line);
    fgets(line, 256, f);
    SL8 = get_uint(line);
    fgets(line, 256, f);
    SR1 = get_uint(line);
    fgets(line, 256, f);
    SR2 = get_uint(line);
    fgets(line, 256, f);
    SR3 = get_uint(line);
    fgets(line, 256, f);
    SR4 = get_uint(line);
}

#if 0
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

