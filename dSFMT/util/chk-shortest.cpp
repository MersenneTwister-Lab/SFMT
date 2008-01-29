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

int fill_state_random(DSFMT& sfmt) {
    static int count = 0;

    if (count > 5000) {
	return 0;
    }
    sfmt.fill_rnd();
    count++;
    return 1;
}

int get_equiv_distrib(int bit, DSFMT& sfmt) {
    DSFMT sfmtnew(sfmt);
    int shortest;

    //fprintf(stderr, "now start get_equiv %d\n", bit);
    set_up(128, bit, 0, msb);
    shortest = get_shortest_base(sfmtnew);
    return shortest;
}

int get_equiv_distrib64(int bit, DSFMT& dsfmt) {
    int dist, min;
    uint32_t mode;

    min = INT_MAX;
    for (mode = 0; mode < 2; mode++) {
	set_up(64, bit, mode, msb);
	dist = get_shortest_base(dsfmt);
	//printf("%d\n", dist);
	if (dist < min) {
	    min = dist;
	}
    }
    return min;
}

void lcm_check1(const DSFMT& sfmt, GF2X& lcmpoly, GF2X& poly) {
    GF2X tmp;
    GF2X rempoly;

    if (check_minpoly104(sfmt, lcmpoly, 0)) {
	printf("check minpoly OK!\n");
	DivRem(tmp, rempoly, lcmpoly, poly);
	if (deg(rempoly) != -1) {
	    printf("rem != 0 deg rempoly = %ld: 0\n", deg(rempoly));
	}
    } else {
	printf("check minpoly NG!\n");
    }
}

void div_check(GF2X& lcmpoly, GF2X& poly, int i) {
    GF2X tmp;
    GF2X rempoly;

    DivRem(tmp, rempoly, lcmpoly, poly);
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld: %d\n", deg(rempoly), i);
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

void get_characteristic(GF2X& lcmpoly, const DSFMT& dsfmt, const GF2X& poly) {
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    vec_GF2 vec;
    DSFMT sfmt(dsfmt);
    int i;
    int lcmcount;

    vec.SetLength(2 * maxdegree);
    generating_polynomial104(sfmt, vec, 0, maxdegree);
    berlekampMassey(lcmpoly, maxdegree, vec);
    DivRem(tmp, rempoly, lcmpoly, poly);
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld: 0\n", deg(rempoly));
    } else {
	printf("divide OK\n");
    }
    //lcm_check1(*sfmt, lcmpoly, poly);
    for (i = 1; i < 104; i++) {
	generating_polynomial104(sfmt, vec, i, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
	// div_check(lcmpoly, poly, i);// for debug
    }
    for (i = 0; i < maxdegree; i++) {
	if (fill_state_random(sfmt) == 0) {
	    break;
	}
//	for (int j = 0; j < 104; j++) {
	    generating_polynomial104(sfmt, vec, i % 104, maxdegree);
	    if (IsZero(vec)) {
		break;
	    }
	    berlekampMassey(minpoly, maxdegree, vec);
	    LCM(tmp, lcmpoly, minpoly);
	    if (deg(tmp) > (long)maxdegree) {
		break;
	    }
	    lcmpoly = tmp;
	    lcmcount++;
	    if (deg(lcmpoly) >= (long)maxdegree) {
		break;
	    }
//	}
    }
    //if (deg(lcmpoly) > maxdegree) {
    if (deg(lcmpoly) != maxdegree) {
	printf("fail to get lcm, deg = %ld\n", deg(lcmpoly));
	//exit(1);
    }
#if 0
    if (check_minpoly104(sfmt_save, lcmpoly, 0)) {
	printf("check minpoly 2 OK!\n");
    } else {
	printf("check minpoly 2 NG!\n");
    }
#endif
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
    //DSFMT sfmt_save(sfmt);
    get_characteristic(lcmpoly, sfmt, poly);
    printf("deg lcm poly = %ld\n", deg(lcmpoly));
    printf("weight = %ld\n", weight(lcmpoly));
    printBinary(stdout, lcmpoly);
    DivRem(tmp, rempoly, lcmpoly, poly);
    printf("deg tmp = %ld\n", deg(tmp));
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld\n", deg(rempoly));
	exit(1);
    }
    //sfmt = sfmt_save;
    make_zero_state(sfmt, tmp);
    generating_polynomial104(sfmt, vec, 0, maxdegree);
    berlekampMassey(minpoly, maxdegree, vec);
    if (deg(minpoly) != mexp) {
	printf("deg zero state = %ld\n", deg(minpoly));
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
