#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/GF2XFactoring.h>

#include "util.h"
#include "dsfmt-util.h"
#include "dsfmt.h"

extern "C" {
#include "mt19937blk.h"
}

NTL_CLIENT;

static int mexp;
static int maxdegree;

static bool bit_128 = false;
static bool bit_64 = true;
static bool msb = true;
static char* filename = NULL;
static GF2X g_lcmpoly;
static long g_weight;

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
    readFile(poly, fp, true);
    printf("deg poly = %ld\n", deg(poly));
    fclose(fp);
    printBinary(stdout, poly);
    printf("weight = %ld\n", weight(poly));
}

bool get_characteristic(GF2X& lcmpoly,
			const DSFMT& dsfmt,
			const GF2X& poly) {
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    vec_GF2 vec;
    DSFMT sfmt(dsfmt);
    int i;
    int lcmcount;

    vec.SetLength(2 * maxdegree);
    generating_polynomial128(sfmt, vec, 127, maxdegree);
    berlekampMassey(lcmpoly, maxdegree, vec);
    DivRem(tmp, rempoly, lcmpoly, poly);
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld: 0\n", deg(rempoly));
	exit(1);
    }
    //lcm_check1(*sfmt, lcmpoly, poly);
    for (i = 1; i < 128; i++) {
	generating_polynomial128(sfmt, vec, i, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
	// div_check(lcmpoly, poly, i);// for debug
    }
    for (i = 0; i < maxdegree; i++) {
	if (fill_state_random(sfmt) == 0) {
	    break;
	}
	for (int j = 0; j < 128; j++) {
	    generating_polynomial128(sfmt, vec, j, maxdegree);
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
	}
    }
    if (deg(lcmpoly) != maxdegree) {
	return false;
    } else {
	return true;
    }
}

bool trial(GF2X lcmpoly, GF2X acc, vec_pair_GF2X_long factors, long diff) {
    GF2X tmp;
    GF2X t;
    GF2X rempoly;
    int i;
    bool ok = false;
    bool r;
    long d;
    long w;

    for (i = 0; i < factors.length(); i++) {
	d = deg(factors[i].a);
	if (d == diff && factors[i].b > 0) {
	    tmp =  acc * factors[i].a;
	    t = tmp * lcmpoly;
	    w = weight(t);
	    printf("weight = %ld:", w);
	    printBinary(stdout, tmp);
	    ok = true;
	    if (w < g_weight) {
		g_weight = w;
		g_lcmpoly = t;
	    }
	} else if (d < diff && factors[i].b > 0) {
	    acc *=  factors[i].a;
	    factors[i].b--;
	    r = trial(lcmpoly, acc, factors, diff - d);
	    factors[i].b++;
	    acc /= factors[i].a;
	    ok = ok | r;
	}
    }
    return ok;
}

void search_characteristic(char *filename) {
    GF2X poly;
    GF2X lcmpoly;
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    GF2X acc;
    vec_GF2 vec;
    bool success;
    vec_pair_GF2X_long factors;
    long diff;

    vec.SetLength(2 * maxdegree);
    set_up_random(filename, poly);
    DSFMT sfmt(123);
    //DSFMT sfmt_save(sfmt);
    success = get_characteristic(lcmpoly, sfmt, poly);
    if (!success) {
	DivRem(tmp, rempoly, lcmpoly, poly);
	if (deg(rempoly) >= 0) {
	    printf("deg rempoly = %ld\n", deg(rempoly));
	    exit(1);
	}
	printf("trial\n");
	diff = maxdegree - deg(lcmpoly);
	CanZass(factors, tmp);
	acc = 1;
	g_weight = maxdegree;
	success = trial(lcmpoly, acc, factors, diff);
	if (success) {
	    lcmpoly = g_lcmpoly;
	}
    }
    if (success) {
	printf("deg lcm poly = %ld\n", deg(lcmpoly));
	printf("weight = %ld\n", weight(lcmpoly));
	printBinary(stdout, lcmpoly);
	return;
    }
    printf("deg tmp = %ld\n", deg(tmp));
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld\n", deg(rempoly));
	exit(1);
    }
    //sfmt = sfmt_save;
    make_zero_state(sfmt, tmp);
    generating_polynomial128(sfmt, vec, 0, maxdegree);
    berlekampMassey(minpoly, maxdegree, vec);
    if (deg(minpoly) != mexp) {
	printf("deg zero state = %ld\n", deg(minpoly));
	return;
    }
}

int main(int argc, char *argv[]) {
    option(argc, argv);
    mexp = DSFMT::get_rnd_mexp();
    maxdegree = DSFMT::get_rnd_maxdegree();
    printf("mexp = %d, maxdegree = %d\n", mexp, maxdegree);
    search_characteristic(filename);
    return 0;
}
