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

int get_equiv_distrib(int bit, DSFMT& sfmt);
void test_shortest(char *filename);

static int mexp;
static int maxdegree;

static bool bit_128 = false;
static bool bit_64 = true;
static bool msb = true;
static char* filename = NULL;

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

int get_equiv_distrib(int bit, DSFMT& dsfmt) {
    DSFMT sfmtnew(dsfmt);
    int shortest;

    //fprintf(stderr, "now start get_equiv %d\n", bit);
    set_up(128, bit, 0, msb);
    shortest = get_shortest_base(sfmtnew);
    return shortest;
}

int get_equiv_distrib64(int bit, DSFMT& dsfmt) {
    DSFMT sfmtnew(dsfmt);
    int dist, min;
    uint32_t mode;

    min = INT_MAX;
    for (mode = 0; mode < 2; mode++) {
	set_up(64, bit, mode, msb);
	dist = get_shortest_base(sfmtnew);
	//printf("%d\n", dist);
	if (dist < min) {
	    min = dist;
	}
	sfmtnew = dsfmt;
    }
    return min;
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

/*
 * その初期値から始まる系列の横104ビットだけを見る。
 */
void get_lcm1(GF2X& lcmpoly, DSFMT& dsfmt, const GF2X& poly) {
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    vec_GF2 vec;
    int i;

    lcmpoly = 1;
    vec.SetLength(2 * maxdegree);
    for (i = 0; i < 104; i++) {
	generating_polynomial104(dsfmt, vec, i, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	DivRem(tmp, rempoly, minpoly, poly);
	if (deg(rempoly) != -1) {
	    printf("get_lcm1 %d:rem != 0 deg rempoly = %ld: 0\n",
		   i, deg(rempoly));
	}
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
    }
    printf("deg lcm = %ld\n", deg(lcmpoly));
}

void calc_shortest(DSFMT& sfmt) {
    int bit;
    int dist_sum;
    int count;
    int old;
    int shortest;

    if (bit_128) {
	dist_sum = 0;
	count = 0;
	old = 0;
	printf("104 bit k-distribution\n");
	for (bit = 1; bit <= 104; bit++) {
	    //for (bit = 1; bit <= 1; bit++) {
	    shortest = get_equiv_distrib(bit, sfmt);
	    dist_sum += mexp / bit - shortest;
	    if (old == shortest) {
		count++;
	    } else {
		old = shortest;
	    }
	    //printf("k(%d) = %d, %d, %d\n", bit, shortest, dist_sum, count);
	    printf("k(%d) = %d\n", bit, shortest);
	    fflush(stdout);
	}
	printf("104bit D.D:%7d, DUP:%5d\n", dist_sum, count);
    }
    if (bit_64) {
	dist_sum = 0;
	count = 0;
	old = 0;
	printf("52 bit k-distribution\n");
	for (bit = 1; bit <= 52; bit++) {
	    shortest = get_equiv_distrib64(bit, sfmt);
	    dist_sum += mexp / bit - shortest;
	    if (old == shortest) {
		count++;
	    } else {
		old = shortest;
	    }
	    //printf("k(%d) = %d, %d, %d\n", bit, shortest, dist_sum, count);
	    printf("k(%d) = %d\n", bit, shortest);
	    fflush(stdout);
	}
	printf("52bit D.D:%7d, DUP:%5d\n", dist_sum, count);
    }
}

void test_shortest(char *filename) {
    GF2X poly;
    GF2X lcmpoly;
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    vec_GF2 vec;

    vec.SetLength(2 * maxdegree);
    set_up_random(filename, poly);
    DSFMT sfmt(123);
    get_lcm1(lcmpoly, sfmt, poly);
    DivRem(tmp, rempoly, lcmpoly, poly);
    printf("deg tmp = %ld\n", deg(tmp));
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld\n", deg(rempoly));
	exit(1);
    }
    make_zero_state(sfmt, tmp);
    generating_polynomial104(sfmt, vec, 0, maxdegree);
    berlekampMassey(minpoly, maxdegree, vec);
    if (minpoly != poly) {
	printf("minpoly is not our irr-poly\n");
	printBinary(stdout, minpoly);
	return;
    }
    calc_shortest(sfmt);
}

int main(int argc, char *argv[]) {
    option(argc, argv);
    mexp = DSFMT::get_rnd_mexp();
    maxdegree = DSFMT::get_rnd_maxdegree();
    printf("mexp = %d, maxdegree = %d\n", mexp, maxdegree);
    test_shortest(filename);
    return 0;
}
