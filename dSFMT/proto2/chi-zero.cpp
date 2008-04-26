#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/GF2XFactoring.h>

#include "shortbase104.h"
#include "util.h"
#include "dsfmt-util.h"
#include "dsfmt.h"

extern "C" {
#include "mt19937blk.h"
}

NTL_CLIENT;

int get_equiv_distrib(int bit, DSFMT& sfmt);
void test_shortest(char *filename);

static int mexp;
static int maxdegree;

static bool bit_128 = false;
static bool bit_64 = true;
static bool msb = true;
static char* filename = NULL;
static uint64_t high = 0x3ff0000000000000ULL;

void option(int argc, char * argv[]) {
    int c;
    bool first = true;
    bool error = false;
    char *pgm = argv[0];
    for (;;) {
	c = getopt(argc, argv, "hrab:");
	if (error) {
	    break;
	}
	if (c == -1) {
	    if (optind >= 0) {
		filename = argv[optind];
	    }
	    break;
	}
	switch (c) {
	case 'a':
	    bit_128 = true;
	    bit_64 = true;
	    break;
	case 'r':
	    msb = false;
	    break;
	case 'b':
	    if (first) {
		first = false;
		bit_128 = false;
		bit_64 = false;
	    }
	    if (strcmp("128", optarg) == 0) {
		bit_128 = true;
		break;
	    }
	    if (strcmp("64", optarg) == 0) {
		bit_64 = true;
		break;
	    }
	case 'h':
	default:
	    error = true;
	    break;
	}
    }
    if (error || filename == NULL) {
	printf("%s [-a | -b128 -b64] [-r] filename\n", pgm);
	    exit(0);
    }
}

void set_up_random(char *filename, GF2X& poly) {
    FILE *fp;

    printf("filename:%s\n", filename);
    fp = fopen(filename, "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("main");
	fclose(fp);
	exit(1);
    }
    mt_init(1234);
    DSFMT::read_random_param(fp);
    DSFMT::print_param(stdout);
    readFile(poly, fp);
    printf("deg poly = %ld\n", deg(poly));
    fclose(fp);
    printBinary(stdout, poly);
}

void print_factor(GF2X& poly) {
    vec_pair_GF2X_long factors;
    int i;

    CanZass(factors, poly);
    for (i = 0; i < factors.length(); i++) {
	printf("mul = %d, ", (int)factors[i].b);
	printBinary(stdout, factors[i].a);
    }
}

void get_zero_char(const GF2X& poly) {
    GF2X lcmpoly;
    GF2X gcdpoly;
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    vec_GF2 vec;
    DSFMT dsfmt(1234);
    int i;

    dsfmt.init_gen_rand(0, high);
    vec.SetLength(2 * maxdegree);
    generating_polynomial104(dsfmt, vec, 0, maxdegree);
    berlekampMassey(lcmpoly, maxdegree, vec);
    gcdpoly = lcmpoly;
    for (i = 1; i < 104; i++) {
	generating_polynomial104(dsfmt, vec, i, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
	GCD(tmp, gcdpoly, minpoly);
	gcdpoly = tmp;
    }
    DivRem(tmp, rempoly, lcmpoly, poly);
    if (deg(rempoly) != -1) {
	printf("ZERO:rem != 0 deg rempoly = %ld: 0\n", deg(rempoly));
    } else {
	printf("degree of LCM ZERO = %ld amari = %ld\n",
	       deg(lcmpoly), deg(tmp));
	printf("print low degree part factorization\n");
	print_factor(tmp);
    }
    DivRem(tmp, rempoly, gcdpoly, poly);
    printf("degree of GCD ZERO = %ld amari = %ld\n",
	   deg(gcdpoly), deg(tmp));
    if (deg(tmp) != 0) {
	printf("print low degree part factorization\n");
	print_factor(tmp);
    }
}

void test_shortest(char *filename) {
    GF2X poly;
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;

    set_up_random(filename, poly);
    get_zero_char(poly);
}

int main(int argc, char *argv[]) {
    option(argc, argv);
    mexp = DSFMT::get_rnd_mexp();
    maxdegree = DSFMT::get_rnd_maxdegree();
    printf("mexp = %d, maxdegree = %d\n", mexp, maxdegree);
    test_shortest(filename);
    return 0;
}
