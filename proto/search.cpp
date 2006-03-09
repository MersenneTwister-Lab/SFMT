/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <limits.h>
#include <errno.h>
#include "util.h"

extern "C" {
#include "sfmt.h"
#include "mt19937ar.h"
#include "debug.h"
}

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>

NTL_CLIENT

int non_reducible(GF2X& fpoly, int degree);
void search(unsigned int n);

static unsigned long long all_count = 0;
static unsigned long long pass1_count = 0;
static unsigned long long pass2_count = 0;
static unsigned long long pass3_count = 0;
static unsigned long long pass4_count = 0;
int limit;

int non_reducible(GF2X& fpoly, int degree) {
    GF2X t2m = GF2X(2, 1);
    GF2X t1 = GF2X(1, 1);
    GF2X t;
    GF2X alpha;
    int m;
    int count;

    DPRINT("degree = %u", degree);
    if (deg(fpoly) < degree) {
	DPRINT("degree = %u", deg(fpoly));
	return 0;
    }
    count = 1;
    t = t1;
    t += t2m;
  
    for (m = 1; deg(fpoly) > degree; m++) {
	for(;;) {
	    GCD(alpha, fpoly, t);
	    if (IsOne(alpha)) {
		break;
	    }
	    fpoly /= alpha;
	    if (deg(fpoly) < degree) {
		DPRINT("degree = %u", deg(fpoly));
		return 0;
	    }
	}
	if ((deg(fpoly) > degree) && (deg(fpoly) <= degree + m)) {
	    DPRINT("deg(fpoly) = %u", deg(fpoly));
	    return 0;
	}
	t2m *= t2m;
	count++;
	t2m %= fpoly;
	add(t, t2m, t1);
    }
    if (deg(fpoly) != degree) {
	DPRINT("degree = %u\n", degree);
	return 0;
    }
    pass1_count++;
    for (; m <= limit; m++) {
	for(;;) {
	    GCD(alpha, fpoly, t);
	    if (IsOne(alpha)) {
		break;
	    }
	    fpoly /= alpha;
	    if (deg(fpoly) < degree) {
		return 0;
	    }
	}
	t2m *= t2m;
	count++;
	t2m %= fpoly;
	add(t, t2m, t1);
    }
    pass2_count++;
    for (;m < degree; m++) {
	t2m *= t2m;
	t2m %= fpoly;
	count++;
    }
    add(t, t1, t2m);
    if (deg(t) == -1) {
	pass3_count++;
	return 1;
    } else {
	return 0;
    }
}

void generating_polynomial(vec_GF2& vec, unsigned int bitpos, 
			   unsigned int maxdegree)
{
    unsigned int i;
    unsigned int mask = 1UL << bitpos;

    i = 0;
    clear(vec);
    while ((gen_rand() & mask) == 0) {
	i++;
	if(i > 2 * maxdegree){
	    printf("generating_polynomial:too much zeros\n");
	    vec[0] = 1;
	    return;
	}
    }
    vec[0] = 1;

    for (i=1; i<= 2 * maxdegree-1; i++) {
	if ((gen_rand() & mask) == mask){
	    vec[i] = 1;
	}
    }
}

void search(unsigned int n) {
    int j;
    unsigned int succ = 0;
    int bmOk;
    GF2X minpoly;
    vec_GF2 vec;
    unsigned int maxdegree;
    unsigned int mexp;
  
    maxdegree = get_rnd_maxdegree();
    mexp = get_rnd_mexp();
    vec.FixLength(2 * maxdegree);

    for (;;) {
	setup_param(genrand_int32(),
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
	init_gen_rand(genrand_int32()+3);
	//print_param2(stdout);
	bmOk = 1;
	//    for (j = 0; j < 32; j++) {
	for (j = 0; j < 2; j++) {
	    generating_polynomial(vec, j, maxdegree);
	    berlekampMassey(minpoly, maxdegree, vec);
	    if (deg(minpoly) == -1) {
		bmOk = 0;
		if (j == 1) {
		    printf("failure 1\n");
		    fflush(stdout);
		}
		break;
	    }
	    all_count++;
	    if (!non_reducible(minpoly, mexp)) {
		bmOk = 0;
		if (j == 1) {
		    printf("failure 2 deg = %ld\n", deg(minpoly));
		    fflush(stdout);
		}
		break;
	    } else {
		printBinary(stdout, minpoly);
		fflush(stdout);
	    }
	}
	if (bmOk) {
	    pass4_count++;
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
	    printf("count = %llu\n", all_count);
	    printf("pass1 = %llu\n", pass1_count);
	    printf("pass2 = %llu\n", pass2_count);
	    printf("pass3 = %llu\n", pass3_count);
	    printf("pass4 = %llu\n", pass4_count);
	    fflush(stdout);
	}
    }
    printf("count = %llu\n", all_count);
    printf("pass1 = %llu\n", pass1_count);
    printf("pass2 = %llu\n", pass2_count);
    printf("pass3 = %llu\n", pass3_count);
    printf("pass4 = %llu\n", pass4_count);
    fflush(stdout);
}

int main(int argc, char* argv[]){
    int n;
    unsigned long seed;

    setup_param(1, 0, 21, 4, 3, 29, 2, 2, 2, 0, 0, 0, 0);

    if (argc != 3) {
	limit = 32;
	n = 1;
    } else {
	limit = atoi(argv[1]);
	n = atoi(argv[2]);
    }

    printf("MEXP = %d\n", get_rnd_mexp());
    seed = (long)time(NULL);
    printf("seed = %lu\n", seed);
    init_genrand(seed);
    printf("search limit degree = %d\n", limit);
    printf("now search %d times\n", n);
    fflush(stdout);
    search(n);

    return 0;
}
