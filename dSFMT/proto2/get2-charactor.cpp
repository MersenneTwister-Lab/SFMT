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

NTL_CLIENT;

int get_equiv_distrib(int bit, DSFMT& sfmt);
void test_shortest(char *filename);
void chk_fix(DSFMT& fix, DSFMT& con);
void show_factorial(GF2X& pol);

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

    if (count >= maxdegree) {
	return 0;
    }
    sfmt.fill_rnd_all(count);
    count++;
    return 1;
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

void lcm_check1(const DSFMT& sfmt, GF2X& lcmpoly, GF2X& poly) {
    GF2X tmp;
    GF2X rempoly;

    if (check_minpoly104(sfmt, lcmpoly, 0)) {
	printf("check minpoly OK!\n");
	DivRem(tmp, rempoly, lcmpoly, poly);
	if (deg(rempoly) != -1) {
	    printf("lcm_chk1:rem != 0 deg rempoly = %ld: 0\n", deg(rempoly));
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
	printf("div_check: rem != 0 deg rempoly = %ld: %d\n", deg(rempoly), i);
    }
}

void set_up_random(char *filename, GF2X& poly) {
    FILE *fp;
    int c;

    printf("filename:%s\n", filename);
    fp = fopen(filename, "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("main");
	fclose(fp);
	exit(1);
    }
    //mt_init(1234);
    DSFMT::read_random_param(fp);
    DSFMT::print_param(stdout);
    c = getc(fp);
    if (isdigit(c)) {
	ungetc(c, fp);
    } else {
	for(;getc(fp) != '\n';);
    }
    readFile(poly, fp);
    printf("deg poly = %ld\n", deg(poly));
    fclose(fp);
    printBinary(stdout, poly);
}

void chk_fix(DSFMT& fix, DSFMT& con) {
    DSFMT tmp;
    uint64_t ar[1][2];

    printf("===chk fix start ===\n");
    printf("deg min fix = %d\n", deg_min_pol(fix, maxdegree));
    fix.d_p();
    printf("deg min con = %d\n", deg_min_pol(con, maxdegree));
    con.d_p();
    tmp = fix;
    tmp.gen_rand104spar(ar, 1);
    printf("deg min tmp after gen = %d\n", deg_min_pol(tmp, maxdegree));
    tmp.d_p();
    tmp.add(con);
    tmp.add(fix);
    tmp.d_p();
    printf("===chk fix end ===\n");
}

void get_lcm(GF2X& lcmpoly, const GF2X& poly) {
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    vec_GF2 vec;
    DSFMT sfmt(123);
    int i;
    int lcmcount;

    vec.SetLength(2 * maxdegree + 1);
    generating_polynomial104(sfmt, vec, 0, maxdegree);
    berlekampMassey(lcmpoly, maxdegree, vec);
    DivRem(tmp, rempoly, lcmpoly, poly);
    if (deg(rempoly) != -1) {
	printf("get_lcm 1:rem != 0 deg rempoly = %ld: 0\n", deg(rempoly));
	printf("maxdegree = %d\n", maxdegree);
	printf("len vec = %ld\n", vec.length());
	printf("deg minpoly = %ld\n", deg(lcmpoly));
	show_factorial(lcmpoly);
	sfmt.d_p();
    } else {
	printf("divide OK\n");
    }
    //lcm_check1(*sfmt, lcmpoly, poly);
    for (i = 1; i < 104; i++) {
	generating_polynomial104(sfmt, vec, i, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	DivRem(tmp, rempoly, minpoly, poly);
	if (deg(rempoly) != -1) {
	    printf("get_lcm:rem != 0 deg rempoly = %ld: i = %d\n",
		   deg(rempoly), i);
	    printf("deg minpoly = %ld\n", deg(lcmpoly));
	    show_factorial(lcmpoly);
	    return;
	}
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
	// div_check(lcmpoly, poly, i);// for debug
    }
    for (i = 0; i < maxdegree; i++) {
	if (fill_state_random(sfmt) == 0) {
	    break;
	}
	for (int j = 0; j < 104; j++) {
	    generating_polynomial104(sfmt, vec, j, maxdegree);
	    if (IsZero(vec)) {
		break;
	    }
	    berlekampMassey(minpoly, maxdegree, vec);
	    DivRem(tmp, rempoly, minpoly, poly);
	    if (deg(rempoly) != -1) {
		printf("get_lcm:rem != 0 deg rempoly = %ld: i = %d, j = %d\n",
		       deg(rempoly), i, j);
		printf("deg minpoly = %ld\n", deg(lcmpoly));
		show_factorial(lcmpoly);
		return;
	    }
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
	printf("fail to get lcm, deg = %ld != %d\n", deg(lcmpoly), maxdegree);
    }
#if 0
    if (check_minpoly104(sfmt_save, lcmpoly, 0)) {
	printf("check minpoly 2 OK!\n");
    } else {
	printf("check minpoly 2 NG!\n");
    }
#endif
}

void fill_poly(GF2X& small, int diff) {
    vec_pair_GF2X_long factors;
    int i;
    
    CanZass(factors, small);
    for (i = 0; i < factors.length(); i++) {
	if (deg(factors[i].a) <= diff) {
	    printf("deg = %d, mul = %d",
		   (int)deg(factors[i].a), (int)factors[i].b);
	    if (deg(factors[i].a) > 30) {
		printf("\n");
	    } else {
		printf(" : ");
		printBinary(stdout, factors[i].a);
	    }
	}
    }
}

void show_factorial(GF2X& pol) {
    vec_pair_GF2X_long factors;
    int i;
    
    CanZass(factors, pol);
    for (i = 0; i < factors.length(); i++) {
	printf("deg = %d, mul = %d",
	       (int)deg(factors[i].a), (int)factors[i].b);
	if (deg(factors[i].a) > 30) {
	    printf("\n");
	} else {
	    printf(" : ");
	    printBinary(stdout, factors[i].a);
	}
    }
}

void get_characteristic(char *filename) {
    GF2X poly;
    GF2X lcmpoly;
    GF2X minpoly;
    GF2X smallpoly;
    GF2X tmp;
    GF2X rempoly;
    GF2X a, b, d;
    vec_GF2 vec;
    uint64_t lung[2];
    GF2X t1(1, 1);

    SetCoeff(t1, 0);
    //cout << "t1 = " << t1 << endl;
    vec.SetLength(2 * maxdegree);
    set_up_random(filename, poly);
    DSFMT sfmt(123);
    DSFMT sfmt_const(123);
    DSFMT sfmt_const2(123);
    DSFMT const_L_save(123);
    //DSFMT sfmt_save(sfmt);
    get_lcm(lcmpoly, poly);
    if (deg(lcmpoly) < maxdegree) {
	DivRem(tmp, rempoly, lcmpoly, poly);
	if (deg(rempoly) != -1) {
	    printf("get_characteristic 1:rem != 0 deg rempoly = %ld\n",
		   deg(rempoly));
	    show_factorial(lcmpoly);
	    exit(1);
	}
	fill_poly(tmp, maxdegree - (int)deg(lcmpoly));
    }
    if (deg(lcmpoly) != maxdegree) {
	printf("can't get characteristic polynomial!\n");
	show_factorial(lcmpoly);
	return;
    }
    printf("deg lcm poly = %ld\n", deg(lcmpoly));
    printf("characteristic polynomial weight = %ld\n", weight(lcmpoly));
    DivRem(smallpoly, rempoly, lcmpoly, poly);
    printf("deg tmp = %ld\n", deg(tmp));
    if (deg(rempoly) != -1) {
	printf("get_characteristic 2:rem != 0 deg rempoly = %ld\n",
	       deg(rempoly));
	show_factorial(lcmpoly);
	return;
    }
    printBinary(stdout, lcmpoly);
    /* a*poly + b*smallpoly = d */
    XGCD(d, a, b, poly, smallpoly);
    if (deg(d) != 0) {
	printf("failure d != 1\n");
    }
    b *= smallpoly;
    a *= poly;
    sfmt_const.set_const();
    make_zero_state(sfmt_const, b);
    const_L_save = sfmt_const;
    sfmt_const2.set_const();
    make_zero_state(sfmt_const2, a);
    sfmt_const2.add(sfmt_const);
    printf("kakunin modoru\n");
    sfmt_const2.d_p();

    /* a*poly + b*t1 = d */
    XGCD(d, a, b, poly, t1);
    if (deg(d) != 0) {
	printf("failure d != 1\n");
    }
    make_zero_state(sfmt_const, b);
    sfmt_const.get_lung(lung);
    sfmt_const.d_p();
    printf("fix[0] = %016"PRIx64"\n", lung[0]);
    printf("fix[1] = %016"PRIx64"\n", lung[1]);
#if 0
    chk_fix(sfmt_const, const_L_save);
    printf("deg of minpoly of fix = %d\n", deg_min_pol(sfmt_const, maxdegree));
    make_zero_state(sfmt_const, poly);
    printf("deg of minpoly of fix = %d\n", deg_min_pol(sfmt_const, maxdegree));
    sfmt_const.d_p();
#endif
}

int main(int argc, char *argv[]) {
    option(argc, argv);
    mexp = DSFMT::get_rnd_mexp();
    maxdegree = DSFMT::get_rnd_maxdegree();
    printf("mexp = %d, maxdegree = %d\n", mexp, maxdegree);
    get_characteristic(filename);
    return 0;
}