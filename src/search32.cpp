/* SFMT Search Code, M.Saito */

#include <string.h>
#include <limits.h>
#include <errno.h>
#include <iostream>

extern "C" {
#include "mt19937ar.h"
#include "sfmt-st.h"
}
//#include "debug.h"

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include "util.h"

NTL_CLIENT;

void search(unsigned int n);

static unsigned long all_count = 0;
static unsigned long pass_count = 0;
static unsigned int maxdegree;
static unsigned int mexp;
#if 1
static FILE *frandom;
#endif

bool generating_polynomial32(sfmt_t *sfmt, vec_GF2& vec, uint32_t bitpos, 
			     uint32_t maxdegree)
{
    unsigned int i;
    uint32_t rand;
    uint32_t mask;
    uint32_t bit;

    i = 0;
    rand = gen_rand32(sfmt);
    mask = (uint64_t)1UL << (31 - bitpos);
    bit = rand & mask;
    while (!bit) {
	i++;
	if(i > 2 * maxdegree){
	    vec[0] = 1;
	    return false;
	}
	rand = gen_rand32(sfmt);
	bit = rand & mask;
    }
    vec[0] = 1;

    for (i=1; i<= 2 * maxdegree-1; i++) {
	rand = gen_rand32(sfmt);
	bit = (rand & mask);
	if (bit) {
	    vec[i] = 1;
	} else {
	    vec[i] = 0;
	}
    }
    return true;
}

bool getLCM(GF2X& lcmpoly, uint32_t seed, const GF2X& poly) {
    unsigned int bit;
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    sfmt_t sfmt;
    sfmt_t sfmt_save;
    uint32_t shortest;
    int i;
    int dist_sum;
    int count;
    uint32_t old;
    vec_GF2 vec;
    int rc;
    int lcmcount;

    vec.SetLength(2 * maxdegree);
    init_gen_rand(&sfmt, seed);
    sfmt_save = sfmt;
    rc = generating_polynomial32(&sfmt, vec, 0, maxdegree);
    if (!rc) {
	return false;
    }
    berlekampMassey(lcmpoly, maxdegree, vec);
#if 1
    DivRem(tmp, rempoly, lcmpoly, poly);
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld: %d\n", deg(rempoly), i);
    } else {
	printf("OK tmp = %ld, lcm = %ld\n", deg(tmp), deg(lcmpoly));
    }
#endif
    for (i = 1; i < 32; i++) {
	sfmt = sfmt_save;
	rc = generating_polynomial32(&sfmt, vec, i, maxdegree);
	if (!rc) {
	    return false;
	}
	berlekampMassey(minpoly, maxdegree, vec);
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
#if 1
	DivRem(tmp, rempoly, minpoly, poly);
	if (deg(rempoly) != -1) {
	    printf("rem != 0 deg rempoly = %ld: %d\n", deg(rempoly), i);
	} else {
	    printf("OK tmp = %ld, lcm = %ld\n", deg(tmp), deg(minpoly));
	}
#endif
    }
#if 1
    if (deg(lcmpoly) != (long)maxdegree) {
	printf("fail in 32bit try to add initial state deg=%ld\n",
	       deg(lcmpoly));
	//return false;
    }
#endif
#if 1
    lcmcount = 0;
    while (deg(lcmpoly) < (long)maxdegree) {
	if (lcmcount > 1000) {
	    printf("failure\n");
	    return false;
	}
	errno = 0;
	fread(sfmt.sfmt, sizeof(uint32_t), N * 4, frandom);
	if (errno) {
	    perror("set_bit");
	    fclose(frandom);
	    exit(1);
	}
	for (int j = 0; j < 32; j++) {
	    rc = generating_polynomial32(&sfmt, vec, j, maxdegree);
	    if (!rc) {
		break;
	    }
	    berlekampMassey(minpoly, maxdegree, vec);
	    LCM(tmp, lcmpoly, minpoly);
	    lcmpoly = tmp;
	    lcmcount++;
	    if (deg(lcmpoly) >= (long)maxdegree) {
		break;
	    }
	}
    }
#endif
    if (deg(lcmpoly) != (long)maxdegree) {
	printf("fail in 32bit try next initial state %ld\n", deg(lcmpoly));
	return false;
    }
    DivRem(tmp, rempoly, lcmpoly, poly);
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld\n", deg(rempoly));
	return false;
    }
    return true;
#if 0
    sfmt = sfmt_save;
    make_zero_state_inv(&sfmt, tmp);
    dist_sum = 0;
    count = 0;
    old = 0;
    for (bit = 1; bit <= 32; bit++) {
	shortest = get_equiv_distrib(bit, &sfmt);
	if (shortest > mexp) {
	    printf("k(%d) = %d\n", bit, shortest);
	    printf("distribution greater than mexp!\n");
	    return false;
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
    fflush(stdout);
    return true;
#endif
}

void search(unsigned int n) {
    int j;
    unsigned int succ = 0;
    bool checkOk;
    GF2X minpoly;
    GF2X lcmpoly;
    sfmt_t sfmt;
    vec_GF2 vec;
    uint32_t seed;
  
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
	    genrand_int32());
	seed = genrand_int32()+3;
	init_gen_rand(&sfmt, seed);
	checkOk = true;
	for (j = 0; j < 1; j++) {
	    if (!generating_polynomial32(&sfmt, vec, j, maxdegree)) {
		checkOk = false;
		break;
	    }
	    berlekampMassey(minpoly, maxdegree, vec);
	    if (deg(minpoly) == -1) {
		checkOk = false;
		break;
	    }
	    all_count++;
	    if (!non_reducible(minpoly, mexp)) {
		checkOk = false;
		break;
	    }
	}
	if (checkOk && getLCM(lcmpoly, seed, minpoly)) {
	    printf("----------\n");
	    printf("succ = %u\n", ++succ);
	    printf("deg = %ld\n", deg(minpoly));
	    print_param(stdout);
	    //print_param2(stdout);
	    printBinary(stdout, minpoly);
	    fflush(stdout);
	    printf("lcm:\n");
	    printBinary(stdout, lcmpoly);
	    printf("weight = %ld\n", weight(lcmpoly));
	    pass_count++;
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

    setup_param(1, 0, 21, 4, 3, 29, 0, 0, 0);

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
#if 1
    frandom = fopen("/dev/random", "r");
    if (errno) {
	perror("main");
	exit(1);
    }
#endif
    search(n);
#if 1
    fclose(frandom);
#endif
    return 0;
}
