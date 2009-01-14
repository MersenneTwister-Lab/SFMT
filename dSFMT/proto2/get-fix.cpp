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
bool chk_fix(DSFMT& fix, DSFMT& con, GF2X& idf);
void show_factorial(GF2X& pol);
bool calc_fixpoint(GF2X& small);

static int mexp;
static int maxdegree;
static bool debug = false;
static char* filename = NULL;
static GF2X mexpirr;

void option(int argc, char * argv[]) {
    int c;
    bool error = false;
    char *pgm = argv[0];
    for (;;) {
	c = getopt(argc, argv, "hd");
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
	case 'd':
	    debug = true;
	    break;
	case 'h':
	default:
	    error = true;
	    break;
	}
    }
    if (error || filename == NULL) {
	printf("%s filename\n", pgm);
	    exit(0);
    }
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

// korede check dekiru nara moto moto fix ari.
bool chk_fix(DSFMT& fix, DSFMT& con, GF2X& idf) {
    DSFMT tmp;
    uint64_t ar[1][2];

    if (debug) printf("===chk fix start ===\n");
    if (debug) printf("deg min fix = %d\n", deg_min_pol(fix, maxdegree));
    if (debug) fix.d_p();
    if (debug) printf("deg min con = %d\n", deg_min_pol(con, maxdegree));
    if (debug) con.d_p();
    tmp = fix;
    tmp.gen_rand104spar(ar, 1);
    if (debug) printf("deg min tmp after gen = %d\n",
		      deg_min_pol(tmp, maxdegree));
    if (debug) tmp.d_p();
    make_zero_state(tmp, idf);
    tmp.add(con);
    if (debug) tmp.d_p();
    if (debug) printf("===chk fix end ===\n");
    return (tmp == fix);
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

void get_lcm(GF2X& lcmpoly, const GF2X& poly) {
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    vec_GF2 vec;
    DSFMT sfmt(123);
    int i;

    vec.SetLength(2 * maxdegree + 1);
    lcmpoly = 1;
    for (i = 0; i < 104; i++) {
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
    for (i = 0; i < maxdegree && (int)deg(lcmpoly) < maxdegree; i++) {
	fill_state_random(sfmt);
	generating_polynomial104(sfmt, vec, i % 104, maxdegree);
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

bool calc_fixpoint(GF2X& small) {
    GF2X a, b, d, idf;
    GF2X t1(1, 1);
    DSFMT sfmt_const0(123);
    DSFMT sfmt_const(123);
    DSFMT sfmt_const2(123);
    DSFMT const_L_save(123);
    uint64_t fix[2];

    if (debug) {
	printf("calc_fixpoint: small=");
	printBinary(stdout, small);
    }
    SetCoeff(t1, 0);
    /* a*poly + b*smallpoly = d */
    XGCD(d, a, b, mexpirr, small);
    if (deg(d) != 0) {
	printf("failure d != 1\n");
	return false;
    }
    b *= small;
    idf = b;
    a *= mexpirr;
    sfmt_const.set_const();
    sfmt_const0 = sfmt_const;
    make_zero_state(sfmt_const, b);
    const_L_save = sfmt_const;
    sfmt_const2.set_const();
    make_zero_state(sfmt_const2, a);
    sfmt_const2.add(sfmt_const);
    if (!(sfmt_const2 == sfmt_const0)) {
	if (debug) {
	    printf("modoranai\n");
	    sfmt_const2.add(sfmt_const0);
	    sfmt_const2.d_p();
	}
	return false;
    }

    /* a*poly + b*t1 = d */
    XGCD(d, a, b, mexpirr, t1);
    if (deg(d) != 0) {
	printf("failure d != 1\n");
	return false;
    }
    make_zero_state(sfmt_const, b);
    sfmt_const.get_lung(fix);
    printf("fix1 0x%16"PRIx64"\n", fix[0]);
    printf("fix2 0x%16"PRIx64"\n", fix[1]);
    if (chk_fix(sfmt_const, const_L_save, idf)) {
	return true;
    }
    return false;
}

void get_fixpoint(char *filename) {
    GF2X poly;
    GF2X lcmpoly;
    GF2X minpoly;
    GF2X smallpoly;
    GF2X tmp;
    GF2X rempoly;
    GF2X filler;
    bool res = false;

    set_up_random(filename, poly);
    get_lcm(lcmpoly, poly);
    mexpirr = poly;
    DivRem(smallpoly, rempoly, lcmpoly, poly);

    printf("deg small poly = %ld\n", deg(smallpoly));
    printf("deg lcm poly = %ld\n", deg(lcmpoly));
    res = calc_fixpoint(smallpoly);
    if (!res) {
	printf("can't find fix point\n");
    }
}

int main(int argc, char *argv[]) {
    option(argc, argv);
    mexp = DSFMT::get_rnd_mexp();
    maxdegree = DSFMT::get_rnd_maxdegree();
    printf("mexp = %d, maxdegree = %d\n", mexp, maxdegree);
    get_fixpoint(filename);
    return 0;
}
