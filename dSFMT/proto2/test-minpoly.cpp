#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>

#include "shortbase104.h"
#include "util.h"
#include "dsfmt-util.h"
#include "dsfmt.h"

extern "C" {
#include "mt19937blk.h"
}

NTL_CLIENT;

static int mexp;
static int maxdegree;
static char* filename = NULL;

void option(int argc, char * argv[]) {
    int c;
    bool error = false;
    char *pgm = argv[0];
    for (;;) {
	c = getopt(argc, argv, "h:");
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

int fill_state_random(DSFMT& sfmt) {
    static int count = 0;

    if (count > 5000) {
	return 0;
    }
    sfmt.fill_rnd();
    count++;
    return 1;
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

void test_minpoly(DSFMT& sfmt, const GF2X& poly) {
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    vec_GF2 vec;
    //DSFMT sfmt(dsfmt);
    int i;
    bool found;
    uint64_t array[2];

    fill_state_random(sfmt);
    sfmt.init_gen_rand(123, 0);
    //sfmt.gen_rand104sp(array, 0);
    vec.SetLength(2 * maxdegree);
    // これでダメということは初期状態を出すのがダメということ。
    found = false;
    for (i = 0; i < 104; i++) {
	generating_polynomial104(sfmt, vec, i, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	DivRem(tmp, rempoly, minpoly, poly);
	if (deg(rempoly) != -1) {
	    //printf("rem != 0 deg rempoly = %ld: 0\n", deg(rempoly));
	    printf("x:%d\n", (int)deg(minpoly));
	    printBinary(stdout, vec);
	    found = true;
	    break;
	} else {
	    printf("o");
	}
    }
    if (found) {
	return;
    }
    printf("\n");
    for (i = 0; i < 10; i++) {
	if (fill_state_random(sfmt) == 0) {
	    break;
	}
	for (int j = 0; j < 104; j++) {
	    generating_polynomial104(sfmt, vec, j % 104, maxdegree);
	    berlekampMassey(minpoly, maxdegree, vec);
	    DivRem(tmp, rempoly, minpoly, poly);
	    if (deg(rempoly) != -1) {
		printf("x\n");
		found = true;
		break;
	    } else {
		printf("o");
	    }
	}
    }
}


void test_shortest(char *filename) {
    GF2X poly;

    set_up_random(filename, poly);
    DSFMT sfmt(123);
    test_minpoly(sfmt, poly);
}

int main(int argc, char *argv[]) {
    option(argc, argv);
    mexp = DSFMT::get_rnd_mexp();
    maxdegree = DSFMT::get_rnd_maxdegree();
    printf("mexp = %d, maxdegree = %d\n", mexp, maxdegree);
    test_shortest(filename);
    return 0;
}
