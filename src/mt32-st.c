#include <stdio.h>
#include <string.h>
#include "sfmt-st.h"

#define MT_N 624
#define MT_M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

unsigned int get_rnd_maxdegree(void)
{
    return 19968;
}

unsigned int get_rnd_mexp(void)
{
    return 19937;
}

void setup_param(unsigned int p1, unsigned int p2, unsigned int p3, 
		 unsigned int p4, unsigned int p5, unsigned int p6,
		 unsigned int p7, unsigned int p8, unsigned int p9) {
}

void print_param(FILE *fp) {
}

void print_param2(FILE *fp) {
}

static void next_state(sfmt_t *sfmt)
{
  unsigned long y;
  static unsigned long mag01[2]={0x0UL, MATRIX_A};
  /* mag01[x] = x * MATRIX_A  for x=0,1 */

  if (sfmt->idx >= MT_N) {
      sfmt->idx = 0;
  }
  y = (sfmt->mt[sfmt->idx] & UPPER_MASK) 
      | (sfmt->mt[(sfmt->idx + 1) % MT_N] & LOWER_MASK);
  sfmt->mt[sfmt->idx] = sfmt->mt[(sfmt->idx + MT_M) % MT_N] 
      ^ (y >> 1) ^ mag01[y & 0x1UL];
}

/*------------------------------------
 * Output initial state 
 ------------------------------------*/
#ifdef OUT_INITIAL
uint64_t gen_rand128(sfmt_t *sfmt, uint64_t *hi, uint64_t *low)
{
    uint32_t i;

    i = sfmt->idx / 4;
    *low = (uint64_t)sfmt->sfmt[i][0] | ((uint64_t)sfmt->sfmt[i][1] << 32);
    *hi = (uint64_t)sfmt->sfmt[i][2] | ((uint64_t)sfmt->sfmt[i][3] << 32);
    next_state(sfmt);
    sfmt->idx += 4;
    sfmt->idx = (sfmt->idx / 4) * 4;
    if (sfmt->idx >= N * 4) {
	sfmt->idx = 0;
    }
    return *hi;
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(mt_rand *rand)
{
  unsigned long y;
  static unsigned long mag01[2]={0x0UL, MATRIX_A};
  /* mag01[x] = x * MATRIX_A  for x=0,1 */

  y = (rand->mt[rand->p] & UPPER_MASK) | (rand->mt[rand->q] & LOWER_MASK);
  y = rand->mt[rand->p] = rand->mt[rand->r] ^ (y >> 1) ^ mag01[y & 0x1UL];

  if (++(rand->p) == MT_N) {
    rand->p = 0;
  }
  if (++(rand->q) == MT_N) {
    rand->q = 0;
  }
  if (++(rand->r) == MT_N) {
    rand->r = 0;
  }

  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680UL;
  y ^= (y << 15) & 0xefc60000UL;
  y ^= (y >> 18);
  return y;

  //return result;
}

/* initializes mt[N] with a seed */
void init_genrand(mt_rand *rand, unsigned long s)
{
  int i;
  rand->mt[0]= s & 0xffffffffUL;
  for (i = 1; i < MT_N; i++) {
    rand->mt[i] = 
      (1812433253UL * (rand->mt[i - 1] 
		       ^ (rand->mt[i - 1] >> 30)) + i); 
    /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    /* In the previous versions, MSBs of the seed affect   */
    /* only MSBs of the array mt[].                        */
    /* 2002/01/09 modified by Makoto Matsumoto             */
    rand->mt[i] &= 0xffffffffUL;
    /* for >32 bit machines */
  }
  rand->p = 0;
  rand->q = 1;
  rand->r = MT_M;
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void init_by_array(mt_rand *rand, unsigned long init_key[], int key_length)
{
  int i, j, k, end;
  init_genrand(rand, 19650218UL);
  i=1; j=0;
  end = (MT_N>key_length ? MT_N : key_length);
  for (k = 0; k < end; k++) {
    rand->mt[i] ^= ((rand->mt[i-1] ^ (rand->mt[i-1] >> 30)) * 1664525UL);
    rand->mt[i] += init_key[j] + j; /* non linear */
    rand->mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
    if (++i >= MT_N) {
      rand->mt[0] = rand->mt[MT_N - 1];
      i=1;
    }
    if (++j >= key_length) {
      j=0;
    }
  }
  for (k = 0; k < (MT_N - 1); k++) {
    rand->mt[i] ^= ((rand->mt[i-1] ^ (rand->mt[i-1] >> 30)) * 1566083941UL);
    rand->mt[i] -= i; /* non linear */
    rand->mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
    if (++i >= MT_N) {
      rand->mt[0] = rand->mt[MT_N-1];
      i=1;
    }
  }
  rand->mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}


void add(mt_rand *a, mt_rand *b) {
  int i;
  int ap;
  int bp;

  ap = a->p;
  bp = b->p;
  for (i = 0; i < MT_N; i++) {
    if (ap >= MT_N) {
      ap = 0;
    }
    if (bp >= MT_N) {
      bp = 0;
    }
    a->mt[ap++] ^= b->mt[bp++];
  }
}

/*
  void generating_polynomial(mt_rand *rand, uint64_t poly[], unsigned int bitpos)
  {
  int i;
  unsigned int mask = 1UL << bitpos;

  //DPRINTMT("in gene:", rand);
  memset(poly, 0, sizeof(uint64_t) * MAXDEGWD);
  i = 0;
  while ((genrand_int32(rand) & mask) == 0) {
  i++;
  if(i > 2 * MAXDEGREE){
  printf("generating_polynomial:too much zeros\n");
  poly[0] = 1;
  DEG(poly) = 0;
  return;
  }
  }
  //DPRINTMT("middle gene:", rand);
  set_coeff(poly,0,1);

  for (i=1; i<= 2 * MAXDEGREE-1; i++) {
  if ((genrand_int32(rand) & mask) == mask){
  set_coeff(poly, i, 1);
  }
  }
  for (; i>=0; i--){
  if (coeff(poly, i)!=0){
  DEG(poly) = i;
  break;
  }
  }
  //DPRINTMT("end gene:", rand);
  }
*/
void print_mt_random(FILE *fp, mt_rand *mt) {
  int i, j;

  fprintf(fp, "index = %u ", mt->p);
  fprintf(fp, "mt:\n");
  for (i = 0; i < MT_N; i++) {
    //for (i = 0; i < 5; i++) {
    for (j = 31; j >= 0; j--) {
      if ((mt->mt[i] & (1 << j)) != 0) {
	fprintf(fp, "%c", '1');
      } else {
	fprintf(fp, "%c", '0');
      }
    }
    fprintf(fp, "\n");
  }
}

void dprint_mt(char *file, int line, char *s, mt_rand *mt) {
  fprintf(stderr, "%s:%d %s", file, line, s);
  print_mt_random(stderr, mt);
}
