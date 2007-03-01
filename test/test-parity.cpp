/* 
 * Search Parity Check Vector
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
//#include <iostream>

extern "C" {
  #include "sfmt-st.h"
}

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>
#include "util.h"

NTL_CLIENT;

/* internal status */
struct IN_STATUS {
    bool zero;
    vec_GF2 next;
    sfmt_t random;
};
typedef struct IN_STATUS in_status;

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

static int mexp;
static int maxdegree;
static int verbose = 0;

int main(int argc, char *argv[]) {
    GF2X f;
    FILE *fp;
    char fname[256];

    if (argc < 2) {
	printf("usage:%s irr-poly-filename\n", argv[0]);
	exit(1);
    }
    if (argc >= 3) {
	verbose = 1;
	strncpy(fname, argv[2], 256);
    } else {
	verbose = 0;
	strncpy(fname, argv[1], 256);
    }
    mexp = get_rnd_mexp();
    printf("mexp = %d\n", mexp);
    maxdegree = get_rnd_maxdegree();
    fp = fopen(fname, "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("main");
	fclose(fp);
	exit(1);
    }
    readFile(f, fp);
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

    mask = (uint64_t)1UL << (63 - bitpos);
    for (i=0; i<= 2 * maxdegree-1; i++) {
	gen_rand128(sfmt, &hi, &low);
	bit = (hi & mask);
	vec[i] = (bit != 0);
    }
}

static void generating_polynomial128_low(sfmt_t *sfmt, vec_GF2& vec,
					 unsigned int bitpos, 
					 unsigned int maxdegree)
{
    unsigned int i;
    uint64_t hi, low;
    uint64_t mask;
    uint64_t bit;

    mask = (uint64_t)1UL << (63 - bitpos);
    for (i=0; i<= 2 * maxdegree-1; i++) {
	gen_rand128(sfmt, &hi, &low);
	bit = (low & mask);
	vec[i] = (bit != 0);
    }
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
    GF2X q;
    GF2X rem;
    vec_GF2 vec;
    int i;
    int result = 1;

    for (i = 0; i < 100; i++) {
	if (verbose) printf("------\n");
	init_gen_rand(&sfmt, i + 1);
	vec.SetLength(2 * maxdegree);
	generating_polynomial128(&sfmt, vec, 0, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	DivRem(q, rem, minpoly, f);
	if (deg(rem) != -1) {
	    printf("rem != 0 deg rempoly = %ld: 0\n", deg(rem));
	    printf("deg q = %ld: 0\n", deg(q));
	    result = 0;
	    break;
	}
	if (verbose || deg(minpoly) < mexp) {
	    printf("minpoly = %ld\n", deg(minpoly));
	}
	if (deg(minpoly) < mexp) {
	    result = 0;
	    break;
	}
	if (period_certification(&sfmt)) {
	    if (verbose) printf("period certification OK\n");
	} else {
	    if (verbose) printf("period certification NG -> OK\n");
	    if (!period_certification(&sfmt)) {
		result = 0;
		printf("period certification didn't change status\n");
		break;
	    }
	}

	make_zero_state(&sfmt, f);
	generating_polynomial128(&sfmt, vec, 0, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	if (verbose || deg(minpoly) >= mexp) {
	    printf("minpoly = %ld\n", deg(minpoly));
	}
	if (deg(minpoly) >= mexp) {
	    result = 0;
	    break;
	}
	if (period_certification(&sfmt)) {
	    if (verbose) printf("period certification OK\n");
	    result = 0;
	    break;
	} else {
	    if (verbose) printf("period certification NG -> OK\n");
	    if (!period_certification(&sfmt)) {
		result = 0;
		printf("period certification didn't change status\n");
		break;
	    }
	}
	generating_polynomial128(&sfmt, vec, 0, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	if (verbose || deg(minpoly) < mexp) {
	    printf("minpoly = %ld\n", deg(minpoly));
	}
	if (deg(minpoly) < mexp) {
	    result = 0;
	    break;
	}
	if (period_certification(&sfmt)) {
	    if (verbose) printf("period certification OK\n");
	} else {
	    if (verbose) printf("period certification NG -> OK\n");
	    if (!period_certification(&sfmt)) {
		result = 0;
		printf("period certification didn't change status\n");
		break;
	    }
	}
    }
    if (result) {
	printf("test successed\n");
    } else {
	printf("test failed\n");
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
