/* 
 * Search Parity Check Vector
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>
#include "util.h"
#include "dsfmt.h"
#include "dsfmt-util.h"

NTL_CLIENT;

static int mexp;
static int maxdegree;
static int verbose = 0;
static void test_parity(GF2X& f);

int main(int argc, char *argv[]) {
    GF2X f;
    FILE *fp;
    char fname[512];

    if (argc < 2) {
	printf("usage:%s [-v] irr-poly\n", argv[0]);
	exit(1);
    }
    if (argc >= 3) {
	verbose = 1;
	strncpy(fname, argv[2], 512);
    } else {
	verbose = 0;
	strncpy(fname, argv[1], 512);
    }
    mexp = DSFMT::get_rnd_mexp();
    printf("mexp = %d\n", mexp);
    maxdegree = DSFMT::get_rnd_maxdegree();
    printf("filename:%s\n", fname);
    fp = fopen(fname, "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("main");
	fclose(fp);
	exit(1);
    }
    //read_random_param(fp);
    //print_param(stdout);
    readFile(f, fp);
    printf("deg poly = %ld\n", deg(f));
    fclose(fp);
    test_parity(f);
    return 0;
}

static void test_parity(GF2X& f) {
    DSFMT dsfmt;
    GF2X minpoly;
    GF2X q, rem;
    vec_GF2 vec;
    int i;
    int r;
    int result = 0;
    int count;

    if (verbose) {
	count = 10;
    } else {
	count = 100;
    }
    for (i = 0; i < count; i++) {
	if (verbose) printf("------\n");
	if (verbose) printf("==shoki\n");
	dsfmt.init_gen_rand(i + 1);
	vec.SetLength(2 * maxdegree);
	generating_polynomial104(dsfmt, vec, 0, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	DivRem(q, rem, minpoly, f);
	if (deg(rem) != -1) {
	    printf("minpoly = %ld\n", deg(minpoly));
	    printf("rem != 0 deg rempoly = %ld\n", deg(rem));
	    printf("deg q = %ld\n", deg(q));
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
	r = dsfmt.period_certification();
	if (r == 1) {
	    if (verbose) printf("period certification OK\n");
	} else {
	    if (verbose) printf("period certification NG -> OK\n");
	    if (!dsfmt.period_certification()) {
		result = 0;
		printf("period critification didn't change status!!\n");
		break;
	    }
	}
	reset_high_const();
	initial_mask(&sfmt);
	make_zero_state(dsfmt, f);
	sfmt.sfmt[N].u[0] ^= fix[0];
	sfmt.sfmt[N].u[1] ^= fix[1];
	set_high_const();
	initial_mask(&sfmt);
	if (verbose) printf("==zero\n");
	generating_polynomial104(dsfmt, vec, 0, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	if (verbose || deg(minpoly) >= mexp) {
	    printf("minpoly = %ld\n", deg(minpoly));
	}
	if (deg(minpoly) >= mexp) {
	    printf("make zero state failed\n");
	    result = 0;
	    break;
	}
	r = dsfmt.period_certification();
	if (r == 1) {
	    if (verbose) printf("period certification OK [ERROR]\n");
	    result = 0;
	    break;
	} else {
	    if (verbose) printf("period certification NG -> OK\n");
	    if (!dsfmt.period_certification()) {
		result = 0;
		printf("period certification didn't chanege status!!\n");
		break;
	    }
	}
	generating_polynomial104(dsfmt, vec, 0, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	if (verbose || deg(minpoly) < mexp) {
	    printf("minpoly = %ld\n", deg(minpoly));
	}
	if (deg(minpoly) < mexp) {
	    result = 0;
	    break;
	}
	r = dsfmt.period_certification();
	if (r == 1) {
	    if (verbose) printf("period certification OK\n");
	} else {
	    if (verbose) printf("period certification NG -> OK\n");
	    if (!dsfmt.period_certification()) {
		printf("error!!\n");
		return;
	    }
	}
	result++;
    }
    if (result) {
	printf("test successed %d / %d\n", result, count);
    } else {
	printf("test failed at count %d\n", count);
    }
}
