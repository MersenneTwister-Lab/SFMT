/* SFMT Search Code, M.Saito */

#include <string.h>
#include <limits.h>
#include <errno.h>
#include <iostream>
#include "sfmt-st.h"

extern "C" {
#include "mt19937ar.h"
}
//#include "debug.h"

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include "util.h"

NTL_CLIENT;

int non_reducible(GF2X& fpoly, int degree);
void search(unsigned int n);

static unsigned long all_count = 0;
static unsigned long pass_count = 0;
static unsigned int maxdegree;
static unsigned int mexp;
#if 1
static FILE *frandom;
#endif

bool generating_polynomial128_hi(sfmt_t *sfmt, vec_GF2& vec, uint32_t bitpos, 
				 uint32_t maxdegree)
{
    unsigned int i;
    uint64_t hi, low;
    uint64_t mask;
    uint64_t bit;

    mask = (uint64_t)1ULL << (63 - bitpos);
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	gen_rand128(sfmt, &hi, &low);
	bit = (hi & mask);
	if (bit) {
	    vec[i] = 1;
	} else {
	    vec[i] = 0;
	}
    }
    return true;
}

bool generating_polynomial128_low(sfmt_t *sfmt, vec_GF2& vec, uint32_t bitpos, 
				 uint32_t maxdegree)
{
    unsigned int i;
    uint64_t hi, low;
    uint64_t mask;
    uint64_t bit;

    mask = (uint64_t)1ULL << (63 - bitpos);
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	gen_rand128(sfmt, &hi, &low);
	bit = (low & mask);
	if (bit) {
	    vec[i] = 1;
	} else {
	    vec[i] = 0;
	}
    }
    return true;
}

bool generating_polynomial128(sfmt_t *sfmt, vec_GF2& vec, unsigned int bitpos, 
			   unsigned int maxdegree)
{
    if (bitpos < 64) {
	return generating_polynomial128_hi(sfmt, vec, bitpos, maxdegree);
    } else {
	return generating_polynomial128_low(sfmt, vec, bitpos - 64, maxdegree);
    }
}

bool getLCM(GF2X& lcmpoly, sfmt_t *sfmt, const GF2X& poly) {
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    sfmt_t sfmt_save;
    int i;
    vec_GF2 vec;
    int rc;
    int lcmcount;

    vec.SetLength(2 * maxdegree);
    //init_gen_rand(&sfmt, 12345678901ULL);
    sfmt_save = *sfmt;
    rc = generating_polynomial128(sfmt, vec, 0, maxdegree);
    if (!rc) {
	printf("getLCM faile\n");
	return false;
    }
    berlekampMassey(lcmpoly, maxdegree, vec);
    for (i = 1; i < 128; i++) {
	rc = generating_polynomial128(sfmt, vec, i, maxdegree);
	if (!rc) {
	    printf("getLCM faile\n");
	    return false;
	}
	berlekampMassey(minpoly, maxdegree, vec);
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
    }
#if 0
    if (deg(lcmpoly) != (long)maxdegree) {
	printf("fail in 128bit try next initial state\n");
	return false;
    }
#endif
#if 1
    lcmcount = 0;
    while (deg(lcmpoly) < (long)maxdegree) {
	if (lcmcount > 1000) {
	    printf("getLCM faile\n");
	    return false;
	}
	errno = 0;
	fread(sfmt->sfmt, sizeof(uint32_t), N * 4, frandom);
	if (errno) {
	    printf("set_bit:%s\n", strerror(errno));
	    fclose(frandom);
	    exit(1);
	}
	for (int j = 0; j < 128; j++) {
	    rc = generating_polynomial128(sfmt, vec, j, maxdegree);
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
	printf("fail in 128bit try next initial state\n");
	return false;
    }
    DivRem(tmp, rempoly, lcmpoly, poly);
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld\n", deg(rempoly));
	return false;
    }
    return true;
#if 0
    *sfmt = sfmt_save;
    make_zero_state_inv(sfmt, tmp);
    dist_sum = 0;
    count = 0;
    old = 0;
    for (bit = 1; bit <= 32; bit++) {
	shortest = get_equiv_distrib(bit, sfmt);
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
	init_gen_rand(&sfmt, genrand_int32()+3);
	checkOk = true;
	//    for (j = 0; j < 32; j++) {
	for (j = 0; j < 1; j++) {
	    if (!generating_polynomial128(&sfmt, vec, j, maxdegree)) {
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
	if (checkOk && getLCM(lcmpoly, &sfmt, minpoly)) {
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

    //setup_param(1, 0, 21, 4, 3, 29, 0, 0, 0);

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
#if 0
    fclose(frandom);
#endif
    return 0;
}
