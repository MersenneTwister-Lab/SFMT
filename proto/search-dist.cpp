/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <limits.h>
#include <errno.h>
#include <iostream>

extern "C" {
#include "mt19937ar.h"
}
//#include "debug.h"

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/GF2XFactoring.h>

#include "shortbase.h"
#include "sfmt-cls.h"
#include "util.h"

NTL_CLIENT;

int non_reducible(GF2X& fpoly, int degree);
void search(unsigned int n);
int get_distribution(SFMT& sfmt, GF2X& poly);
int get_equiv_distrib(int bit, SFMT& sfmt);

static unsigned long all_count = 0;
static unsigned long pass_count = 0;
//int limit;
static unsigned int maxdegree;
static unsigned int mexp;

const GF2X t2 = GF2X(2, 1);
const GF2X t1 = GF2X(1, 1);

void generating_polynomial(SFMT& sfmt, vec_GF2& vec, unsigned int bitpos, 
			   unsigned int maxdegree)
{
    unsigned int i;
    vec_GF2 random;

    //DPRINTHT("in gene:", rand);
    i = 0;
    while (IsZero(sfmt.gen_rand(random, bitpos + 1).get(bitpos))) {
	i++;
	if(i > 2 * maxdegree){
	    printf("generating_polynomial:too much zeros\n");
	    vec[0] = 1;
	    return;
	}
    }
    //DPRINTHT("middle gene:", rand);
    vec[0] = 1;

    for (i=1; i<= 2 * maxdegree-1; i++) {
	vec[i] = sfmt.gen_rand(random, bitpos + 1).get(bitpos);
    }
    //DPRINTHT("end gene:", rand);
}

void search(unsigned int n) {
    int j;
    unsigned int succ = 0;
    bool bmOk;
    GF2X minpoly;
    SFMT sfmt;
    vec_GF2 vec;
    int rc;
  
    vec.FixLength(2 * maxdegree);
    for (;;) {
	setup_param(
	    genrand_int32(),
	    genrand_int32(),
	    genrand_int32(),
	    genrand_int32(),
	    genrand_int32(),
	    genrand_int32(),
	    genrand_int32(),
	    genrand_int32(),
	    genrand_int32(),
	    genrand_int32(),
	    genrand_int32(),
	    genrand_int32(),
	    genrand_int32());
	sfmt.reseed(genrand_int32()+3);
	bmOk = true;
	//    for (j = 0; j < 32; j++) {
	for (j = 0; j < 1; j++) {
	    generating_polynomial(sfmt, vec, j, maxdegree);
	    berlekampMassey(minpoly, maxdegree, vec);
	    if (deg(minpoly) == -1) {
		bmOk = false;
		break;
	    }
	    all_count++;
	    if (!non_reducible(minpoly, mexp)) {
		bmOk = false;
		break;
	    }
	}
	if (bmOk) {
	    pass_count++;
	    printf("----------\n");
	    printf("succ = %u\n", ++succ);
	    printf("deg = %ld\n", deg(minpoly));
	    print_param(stdout);
	    printBinary(stdout, minpoly);
	    fflush(stdout);
	    rc = get_distribution(sfmt, minpoly);
	    if (rc == 0) {
		succ--;
	    }
	    if (succ >= n) {
		break;
	    }
	}
	if (all_count % 10000 == 0) {
	    printf("count = %lu\n", all_count);
	    printf("pass = %lu\n", pass_count);
	    fflush(stdout);
	}
    }
    printf("count = %lu\n", all_count);
    printf("pass = %lu\n", pass_count);
    fflush(stdout);
}

int get_equiv_distrib(int bit, SFMT& sfmt) {
    static SFMT sfmtnew;
    int shortest;

    //fprintf(stderr, "now start get_equiv %d\n", bit);
    sfmtnew = sfmt;
    set_bit_len(bit);
    shortest = get_shortest_base(bit, sfmtnew);
    return shortest;
}

void make_zero_state(SFMT& sfmt, const GF2X& poly) {
    SFMT sfmtnew;
    int i;

    for (i = 0; i <= deg(poly); i++) {
    //for (i = deg(poly); i >= 0; i--) {
	if (coeff(poly, i) != 0) {
	    sfmtnew.add(sfmt);
	}
	sfmt.next_state();
    }
    sfmt = sfmtnew;
}

int get_distribution(SFMT& sfmt, GF2X& poly) {
    unsigned int bit;
    GF2X lcmpoly;
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    SFMT sfmt_save;
    vec_GF2 vec;
    uint32_t shortest;
    int i;
    int dist_sum;
    int count;
    uint32_t old;

    sfmt.reseed(123);
    sfmt_save = sfmt;
    vec.SetLength(2 * maxdegree);
    generating_polynomial(sfmt, vec, 0, maxdegree);
    berlekampMassey(lcmpoly, maxdegree, vec);
    for (i = 0; i < 128; i++) {
	//sfmt = sfmt_save;
	generating_polynomial(sfmt, vec, i, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
    }
    printf("deg lcm poly = %ld\n", deg(lcmpoly));
    if (deg(lcmpoly) != (long)maxdegree) {
	printf("failure!\n");
	fflush(stdout);
	return 0;
    }
    printf("weight = %ld\n", weight(lcmpoly));
    printBinary(stdout, lcmpoly);
    DivRem(tmp, rempoly, lcmpoly, poly);
    printf("deg tmp = %ld\n", deg(tmp));
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld\n", deg(rempoly));
	exit(1);
    }
    sfmt = sfmt_save;
    make_zero_state(sfmt, tmp);
    dist_sum = 0;
    count = 0;
    old = 0;
    for (bit = 1; bit <= 128; bit++) {
	shortest = get_equiv_distrib(bit, sfmt);
	if (shortest > mexp) {
	    printf("k(%d) = %d\n", bit, shortest);
	    printf("distribution greater than mexp!\n");
	    exit(1);
	}
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
    printf("D.D:%7d, DUP:%5d\n", dist_sum, count);
    return 1;
}

int main(int argc, char* argv[]){
    int n;
    unsigned long seed;

    setup_param(1, 0, 21, 4, 3, 29, 0, 0, 0, 0, 0, 0, 0);

    if (argc != 2) {
	n = 1;
    } else {
	n = atoi(argv[1]);
    }

    maxdegree = get_rnd_maxdegree();
    mexp = get_rnd_mexp();
    printf("MEXP = %d\n", mexp);
    seed = (long)time(NULL);
    printf("seed = %lu\n", seed);
    init_genrand(seed);
    printf("now search %d times\n", n);
    fflush(stdout);
    search(n);

    return 0;
}
