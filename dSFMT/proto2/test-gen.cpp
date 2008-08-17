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

static void test_gen();

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
    test_gen();
    return 0;
}

static void test_gen() {
    DSFMT dsfmt;
    uint64_t hi_c = 0x3FF0000000000000ULL;
    int i, j;

    dsfmt.init_gen_rand(1, hi_c);
    dsfmt.period_certification(false);
    for (i = 0; i < 1000; i++) {
	printf("%1.15f ", dsfmt.gen_rand());
	if (i % 4 == 3) {
	    printf("\n");
	}
    }
    for (i = 1; i < 100; i++) {
	dsfmt.init_gen_rand(i + 1, hi_c);
	dsfmt.period_certification(false);
	printf("seed = %d\n", i);
	for (j = 0; j < 12; j++) {
	    printf("%1.15f ", dsfmt.gen_rand());
	    if (j % 4 == 3) {
		printf("\n");
	    }
	}
    }
}
