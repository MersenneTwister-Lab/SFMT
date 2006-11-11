/* 
 * Search Parity Check Vector
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>

extern "C" {
#include "sfmt_p1_st.h"
#include "mt19937ar.h"
}

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>
#include "util.h"

NTL_CLIENT;

const int WORD_WIDTH = 128;

/* internal status */
struct IN_STATUS {
    bool zero;
    vec_GF2 next;
    sfmt_t random;
};
typedef struct IN_STATUS in_status;

#define NTH_BIT2(i) (1U << (31 - (i)))

static void initial_status_parity_check(sfmt_t *sfmt);
static void make_zero_state(sfmt_t *sfmt, GF2X& poly);
static void test_parity(GF2X& f);
static void generating_polynomial128_hi(sfmt_t *sfmt, vec_GF2& vec,
					unsigned int bitpos, 
					unsigned int maxdegree);
static void generating_polynomial128_low(sfmt_t *sfmt, vec_GF2& vec,
					 unsigned int bitpos, 
					 unsigned int maxdegree);
static void generating_polynomial128(sfmt_t *sfmt, vec_GF2& vec,
				     unsigned int bitpos, 
				     unsigned int maxdegree);

static uint32_t mexp;
static uint32_t maxdegree;
static uint32_t parity[4];

int main(int argc, char *argv[]) {
    GF2X f;
    FILE *fp;
    int i;

    if (argc != 6) {
	printf("usage:%s filename p1, p2, p3, p4, p5 %d\n", argv[0], argc);
	exit(1);
    }
    mexp = get_rnd_mexp();
    printf("mexp = %d\n", mexp);
    maxdegree = get_rnd_maxdegree();
    printf("filename:%s\n", argv[1]);
    for (i = 0; i < 4; i++) {
	parity[i] = (uint32_t)strtoll(argv[2 + i], NULL, 16);
	printf("parity[%d] = %08x\n", i, parity[i]);
    }
    fp = fopen(argv[1], "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("main");
	fclose(fp);
	exit(1);
    }
    read_random_param(fp);
    print_param(stdout);
    readFile(f, fp);
    printf("deg poly = %ld\n", deg(f));
    fclose(fp);
    test_parity(f);
    return 0;
}
    
static void generating_polynomial128_hi(sfmt_t *sfmt, vec_GF2& vec,
					unsigned int bitpos, 
					unsigned int maxdegree)
{
    unsigned int i;
    uint64_t hi, low;
    uint64_t mask;
    uint64_t bit;

    //DPRINTHT("in gene:", rand);
    mask = (uint64_t)1UL << (63 - bitpos);
    for (i=0; i<= 2 * maxdegree-1; i++) {
	gen_rand128(sfmt, &hi, &low);
	bit = (hi & mask);
	vec[i] = (bit != 0);
    }
    //DPRINTHT("end gene:", rand);
}

static void generating_polynomial128_low(sfmt_t *sfmt, vec_GF2& vec,
					 unsigned int bitpos, 
					 unsigned int maxdegree)
{
    unsigned int i;
    uint64_t hi, low;
    uint64_t mask;
    uint64_t bit;

    //DPRINTHT("in gene:", rand);
    mask = (uint64_t)1UL << (63 - bitpos);
    for (i=0; i<= 2 * maxdegree-1; i++) {
	gen_rand128(sfmt, &hi, &low);
	bit = (low & mask);
	vec[i] = (bit != 0);
    }
    //DPRINTHT("end gene:", rand);
}

static void generating_polynomial128(sfmt_t *sfmt, vec_GF2& vec,
				     unsigned int bitpos, 
				     unsigned int maxdegree) {
    if (bitpos < 64) {
	generating_polynomial128_hi(sfmt, vec, bitpos, maxdegree);
    } else {
	generating_polynomial128_low(sfmt, vec, bitpos - 64, maxdegree);
    }
}

static void test_parity(GF2X& f) {
    sfmt_t sfmt;
    GF2X minpoly;
    vec_GF2 vec;
    int i;

    for (i = 0; i < 10; i++) {
	printf("------\n");
	init_gen_rand(&sfmt, 1234 * (i + 1));
	vec.SetLength(2 * maxdegree);
	generating_polynomial128(&sfmt, vec, 0, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	printf("minpoly = %ld\n", deg(minpoly));
	initial_status_parity_check(&sfmt);

	make_zero_state(&sfmt, f);
	generating_polynomial128(&sfmt, vec, 0, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	printf("minpoly = %ld\n", deg(minpoly));

	initial_status_parity_check(&sfmt);
	generating_polynomial128(&sfmt, vec, 0, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	printf("minpoly = %ld\n", deg(minpoly));
	initial_status_parity_check(&sfmt);
    }
}

static void initial_status_parity_check(sfmt_t *sfmt) {
    int inner = 0;
    int i, j;
    uint32_t work;

    for (i = 0; i < 4; i++) {
	printf("sfmt[N][%d] = %08x\n", i, sfmt->sfmt[N][i]);
	work = sfmt->sfmt[N][i] & parity[i];
	for (j = 0; j < 32; j++) {
	    inner ^= work & 1;
	    work = work >> 1;
	}
    }
    if (inner == 1) {
	printf("DEBUG:success in parity check\n");
	return;
    }
    printf("DEBUG:failure in parity check\n");
    for (i = 0; i < 4; i++) {
	work = 1;
	for (j = 0; j < 32; j++) {
	    if ((work & parity[i]) != 0) {
		printf("DEBUG: before = %x\n", sfmt->sfmt[N][i]);
		sfmt->sfmt[N][i] ^= work;
		printf("DEBUG: change %dth bit of index %d\n", j, i);
		printf("DEBUG: work = %x, parity[%d] = %d\n", work, i,
		       parity[i]);
		printf("DEBUG: after = %x\n", sfmt->sfmt[N][i]);
		return;
	    }
	    work = work << 1;
	}
    }
}

static void make_zero_state(sfmt_t *sfmt, GF2X& poly) {
  static sfmt_t sfmtnew;
  uint64_t hi, low;
  int i;

  memset(&sfmtnew, 0, sizeof(sfmtnew));
  for (i = 0; i <= deg(poly); i++) {
    if (coeff(poly, i) != 0) {
	add_rnd(&sfmtnew, sfmt);
    }
    gen_rand128(sfmt, &hi, &low);
  }
  *sfmt = sfmtnew;
}
