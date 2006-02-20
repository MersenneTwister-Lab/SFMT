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

#include "sfmt-cls.h"
#include "util.h"

NTL_CLIENT;

int non_reducible(GF2X& fpoly, int degree);
void search(unsigned int n);

static unsigned long all_count = 0;
static unsigned long pass_count = 0;
int limit;

const GF2X t2 = GF2X(2, 1);
const GF2X t1 = GF2X(1, 1);

int non_reducible(GF2X& fpoly, int degree) {
    GF2X t2m;
    GF2X t;
    GF2X alpha;
    int m;

    t2m = t2;
    //DPRINT("degree = %u\n", degree);
    //DPRINTPOLY("fpoly =", fpoly);
    if (deg(fpoly) < degree) {
	return 0;
    }
    t = t1;
    t += t2m;
  
    for (m = 1; deg(fpoly) > degree; m++) {
	//DPRINTPOLY("t =", t);
	for(;;) {
	    GCD(alpha, fpoly, t);
	    //DPRINTPOLY("alpha =", alpha);
	    if (IsOne(alpha)) {
		break;
	    }
	    fpoly /= alpha;
	    //DPRINTPOLY("f =", fpoly);
	    if (deg(fpoly) < degree) {
		return 0;
	    }
	}
	if ((deg(fpoly) > degree) && (deg(fpoly) <= degree + m)) {
	    //DPRINT("maybe fpoly is larger m = %d, DEG = %u\n", m, 
	    //     (unsigned int)deg(fpoly));
	    return 0;
	}
	t2m *= t2m;
	t2m %= fpoly;
	add(t, t2m, t1);
    }
    if (deg(fpoly) != degree) {
	return 0;
    }
    return IterIrredTest(fpoly);
}

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
    unsigned int maxdegree;
    unsigned int mexp;
    SFMT sfmt;
    vec_GF2 vec;
  
    maxdegree = get_rnd_maxdegree();
    mexp = get_rnd_mexp();
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

int main(int argc, char* argv[]){
    int n;
    unsigned long seed;

    setup_param(1, 0, 21, 4, 3, 29, 0, 0, 0, 0, 0, 0, 0);

    if (argc != 2) {
	n = 1;
    } else {
	n = atoi(argv[1]);
    }

    printf("MEXP = %d\n", get_rnd_mexp());
    seed = (long)time(NULL);
    printf("seed = %lu\n", seed);
    init_genrand(seed);
    printf("now search %d times\n", n);
    fflush(stdout);
    search(n);

    return 0;
}
