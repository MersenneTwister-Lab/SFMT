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

extern "C" {
    #include "mt19937blk.h"
}

NTL_CLIENT;

static int mexp;
static int maxdegree;
static int verbose = 0;
static uint64_t high3ff = 0x3FF0000000000000ULL;

static void test_parity0(const GF2X& f);
static void test_parity(const GF2X& f, const GF2X& smallf);

int main(int argc, char *argv[]) {
    GF2X f;
    GF2X smallf;
    GF2X charp;
    GF2X rem;
    FILE *fp;
    char fname[512];
    int c;
    char dum[256];
    unsigned int seed;

    if (argc < 2) {
	printf("usage:%s [-v] test-file\n", argv[0]);
	exit(1);
    }
    if (argc >= 3) {
	verbose = 1;
	strncpy(fname, argv[2], 512);
    } else {
	verbose = 0;
	strncpy(fname, argv[1], 512);
    }
    seed = (unsigned int)time(NULL);
    printf("seed = %u\n", seed);
    mt_init(seed);

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
    DSFMT::read_random_param(fp);
    DSFMT::print_param(stdout);
    c = getc(fp);
    if (isdigit(c)) {
	ungetc(c, fp);
    } else {
	fgets(dum, 256, fp);
    }
    readFile(f, fp);
    printf("deg poly = %ld\n", deg(f));
    readFile(charp, fp, true);
    printf("deg charp = %ld\n", deg(charp));
    fclose(fp);
    DivRem(smallf, rem, charp, f);
    if (deg(rem) != -1) {
	printf("deg rem = %ld\n", deg(rem));
	return 1;
    }
    test_parity0(f);
    test_parity(f, smallf);
    return 0;
}

void get_dsfmtfix(DSFMT& dsfmt_fix, const GF2X& poly, const GF2X& smallpoly) {
    GF2X a, b, d;
    GF2X t1(1, 1);

    SetCoeff(t1, 0);
    /* a*poly + b*smallpoly = d */
    XGCD(d, a, b, poly, smallpoly);
    if (deg(d) != 0) {
	printf("%s(%d):failure d != 1\n", __FILE__, __LINE__);
    }
    b *= smallpoly;
    dsfmt_fix.set_const();
    //dsfmt_fix.mask_status();
    make_zero_state(dsfmt_fix, b);

    /* a*poly + b*t1 = d */
    XGCD(d, a, b, poly, t1);
    if (deg(d) != 0) {
	printf("%s(%d):failure d != 1\n", __FILE__, __LINE__);
    }
    make_zero_state(dsfmt_fix, b);
    printf("dsfmt_fix\n");
    dsfmt_fix.d_p();
}

static void test_parity(const GF2X& f, const GF2X& smallf) {
    DSFMT dsfmt;
    DSFMT dsfmt_fix;
    GF2X minpoly;
    GF2X q, rem;
    vec_GF2 vec;
    int i;
    int r;
    int result = 0;
    int count;
    int mdegree = maxdegree * 2; // for affine.

    get_dsfmtfix(dsfmt_fix, f, smallf);
    if (verbose) {
	count = 10;
    } else {
	count = 100;
    }
    for (i = 0; i < count; i++) {
	if (verbose) printf("------\n");
	if (verbose) printf("==shoki (%d)\n", i);
	dsfmt.init_gen_rand(i + 1, high3ff);
	min_pol(minpoly, dsfmt, mdegree);
	DivRem(q, rem, minpoly, f);
	if (deg(rem) != -1) {
	    //dsfmt.d_p();
	    printf("minpoly = %ld\n", deg(minpoly));
	    printf("rem != 0 deg rempoly = %ld\n", deg(rem));
	    printf("deg q = %ld\n", deg(q));
	    //result = 0;
	    //break;
	}
	if (verbose || deg(minpoly) < mexp) {
	    printf("minpoly = %ld\n", deg(minpoly));
	}
	if (deg(minpoly) < mexp) {
	    continue;
	}
	r = dsfmt.period_certification(true);
	if (r == 1) {
	    if (verbose) printf("period certification OK\n");
	} else {
	    if (verbose) printf("period certification NG -> OK\n");
	    if (!dsfmt.period_certification(true)) {
		printf("period critification didn't change status!!\n");
		continue;
	    }
	}
	/* 周期保証後に最小多項式をチェックするべきであったが */
	min_pol(minpoly, dsfmt, mdegree);
	DivRem(q, rem, minpoly, f);
	if (deg(rem) != -1) {
	    printf("minpoly = %ld\n", deg(minpoly));
	    printf("rem != 0 deg rempoly = %ld\n", deg(rem));
	    printf("deg q = %ld\n", deg(q));
	    print_factorial(minpoly);
	    continue;
	}
	dsfmt.init_gen_rand(i + 1, 0);
	make_zero_state(dsfmt, f);
	dsfmt.add(dsfmt_fix);
	dsfmt.mask_status();
	if (verbose) printf("==zero\n");
	min_pol(minpoly, dsfmt, mdegree);
	if (verbose || deg(minpoly) >= mexp) {
	    printf("minpoly = %ld\n", deg(minpoly));
	}
	if (deg(minpoly) >= mexp) {
	    printf("make zero state failed\n");
	    continue;
	}
	r = dsfmt.period_certification(false);
	if (r == 1) {
	    if (verbose) printf("period certification OK [ERROR]\n");
	    dsfmt.d_p();
	    continue;
	} else {
	    if (verbose) printf("period certification NG -> OK\n");
	    if (!dsfmt.period_certification(false)) {
		printf("period certification didn't chanege status!!\n");
		continue;
	    }
	}
	min_pol(minpoly, dsfmt, mdegree);
	if (verbose || deg(minpoly) < mexp) {
	    printf("minpoly = %ld\n", deg(minpoly));
	}
	if (deg(minpoly) < mexp) {
	    continue;
	}
	r = dsfmt.period_certification(false);
	if (r == 1) {
	    if (verbose) printf("period certification OK\n");
	} else {
	    if (verbose) printf("period certification NG -> OK\n");
	    if (!dsfmt.period_certification(false)) {
		printf("error!!\n");
		return;
	    }
	}
	result++;
    }
    printf("test successed %d / %d\n", result, count);
}

static void test_parity0(const GF2X& f) {
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
    printf("start parity zero\n");
    for (i = 0; i < count; i++) {
	if (verbose) printf("------\n");
	if (verbose) printf("==shoki (%d)\n", i);
	dsfmt.init_gen_rand(i + 1, 0);
	min_pol(minpoly, dsfmt, maxdegree);
	DivRem(q, rem, minpoly, f);
	if (deg(rem) != -1) {
	    printf("minpoly = %ld\n", deg(minpoly));
	    printf("rem != 0 deg rempoly = %ld\n", deg(rem));
	    printf("deg q = %ld\n", deg(q));
	    //result = 0;
	    //break;
	}
	if (verbose || deg(minpoly) < mexp) {
	    printf("minpoly = %ld\n", deg(minpoly));
	}
	if (deg(minpoly) < mexp) {
	    continue;
	}
	r = dsfmt.period_certification(true);
	if (r == 1) {
	    if (verbose) printf("period certification OK\n");
	} else {
	    if (verbose) printf("period certification NG -> OK\n");
	    if (!dsfmt.period_certification(true)) {
		printf("period critification didn't change status!!\n");
		continue;
	    }
	}
	/* 周期保証の後で最小多項式をチェックするべきではあったが */
	min_pol(minpoly, dsfmt, maxdegree);
	DivRem(q, rem, minpoly, f);
	if (deg(rem) != -1) {
	    printf("minpoly = %ld\n", deg(minpoly));
	    printf("rem != 0 deg rempoly = %ld\n", deg(rem));
	    printf("deg q = %ld\n", deg(q));
	    print_factorial(minpoly);
	    continue;
	}
	//dsfmt.fill_rnd(0);
	dsfmt.init_gen_rand(i + 1, 0);
	//dsfmt.d_p();
	make_zero_state(dsfmt, f);
	if (verbose) printf("==zero\n");
	min_pol(minpoly, dsfmt, maxdegree);
	if (verbose || deg(minpoly) >= mexp) {
	    printf("minpoly = %ld\n", deg(minpoly));
	}
	if (deg(minpoly) >= mexp) {
	    printf("make zero state failed\n");
	    continue;
	}
	//dsfmt.d_p();
	r = dsfmt.period_certification(true);
	if (r == 1) {
	    if (verbose) printf("period certification OK [ERROR]\n");
	    continue;
	} else {
	    if (verbose) printf("period certification NG -> OK\n");
	    if (!dsfmt.period_certification(true)) {
		printf("period certification didn't chanege status!!\n");
		continue;
	    }
	}
	min_pol(minpoly, dsfmt, maxdegree);
	if (verbose || deg(minpoly) < mexp) {
	    printf("minpoly = %ld\n", deg(minpoly));
	}
	if (deg(minpoly) < mexp) {
	    continue;
	}
	r = dsfmt.period_certification(true);
	if (r == 1) {
	    if (verbose) printf("period certification OK\n");
	} else {
	    if (verbose) printf("period certification NG -> OK\n");
	    if (!dsfmt.period_certification(true)) {
		printf("error!!\n");
		return;
	    }
	}
	result++;
    }
    printf("test successed %d / %d\n", result, count);
}
