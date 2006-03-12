/* SFMT Search Code, M.Saito */

#include <string.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>

extern "C" {
#include "sfmt.h"
}
//#include "debug.h"

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include "util.h"

NTL_CLIENT;

static unsigned int maxdegree;
static unsigned int mexp;

bool generating_polynomial32(vec_GF2& vec, uint32_t bitpos, uint32_t maxdegree)
{
    unsigned int i;
    uint32_t ran;
    uint32_t mask;
    uint32_t bit;

    mask = (uint32_t)1UL << (31 - bitpos);
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	ran = gen_rand32();
	bit = (ran & mask);
	if (bit) {
	    vec[i] = 1;
	} else {
	    vec[i] = 0;
	}
    }
    return true;
}

bool generating_polynomial64(vec_GF2& vec, uint32_t bitpos, uint32_t maxdegree)
{
    unsigned int i;
    uint64_t ran;
    uint64_t mask;
    uint64_t bit;

    mask = (uint64_t)1ULL << (63 - bitpos);
    for (i = 0 ; i <= 2 * maxdegree - 1; i++) {
	ran = gen_rand64();
	bit = (ran & mask);
	if (bit) {
	    vec[i] = 1;
	} else {
	    vec[i] = 0;
	}
    }
    return true;
}

bool generating_polynomial128_hi(vec_GF2& vec, uint32_t bitpos, 
				 uint32_t maxdegree)
{
    unsigned int i;
    uint64_t hi, low;
    uint64_t mask;
    uint64_t bit;

    mask = (uint64_t)1ULL << (63 - bitpos);
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	gen_rand128(&hi, &low);
	bit = (hi & mask);
	if (bit) {
	    vec[i] = 1;
	} else {
	    vec[i] = 0;
	}
    }
    return true;
}

bool generating_polynomial128_low(vec_GF2& vec, uint32_t bitpos, 
				 uint32_t maxdegree)
{
    unsigned int i;
    uint64_t hi, low;
    uint64_t mask;
    uint64_t bit;

    mask = (uint64_t)1ULL << (63 - bitpos);
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	gen_rand128(&hi, &low);
	bit = (low & mask);
	if (bit) {
	    vec[i] = 1;
	} else {
	    vec[i] = 0;
	}
    }
    return true;
}

bool generating_polynomial128(vec_GF2& vec, uint32_t bitpos, 
			      uint32_t maxdegree)
{
    if (bitpos < 64) {
	return generating_polynomial128_hi(vec, bitpos, maxdegree);
    } else {
	return generating_polynomial128_low(vec, bitpos - 64, maxdegree);
    }
}

bool check128(void) {
    int j;
    bool checkOk;
    GF2X minpoly;
    GF2X lcmpoly;
    vec_GF2 vec;
    long degree;
  
    vec.FixLength(2 * maxdegree);
    init_gen_rand(123);
    checkOk = true;
    for (j = 0; j < 10; j++) {
	if (!generating_polynomial128(vec, j, maxdegree)) {
	    checkOk = false;
	    break;
	}
	berlekampMassey(minpoly, maxdegree, vec);
	degree = deg(minpoly);
	printf("degree = %ld\n", degree);
	if (deg(minpoly) == -1) {
	    checkOk = false;
	    break;
	}
	if (!non_reducible(minpoly, mexp)) {
	    checkOk = false;
	    break;
	}
    }
    if (checkOk) {
	printf("check 128 OK!\n");
	printBinary(stdout, minpoly);
	fflush(stdout);
    } else {
	printf("check 128 NG!\n");
	fflush(stdout);
    }
    return checkOk;
}

bool check64(void) {
    int j;
    bool checkOk;
    GF2X minpoly;
    GF2X lcmpoly;
    vec_GF2 vec;
    long degree;
  
    vec.FixLength(2 * maxdegree);
    init_gen_rand(123);
    checkOk = true;
    for (j = 0; j < 10; j++) {
	if (!generating_polynomial64(vec, j, maxdegree)) {
	    checkOk = false;
	    printf("fail 1\n");
	    break;
	}
	berlekampMassey(minpoly, maxdegree, vec);
	degree = deg(minpoly);
	printf("degree = %ld\n", degree);
	printBinary(stdout, minpoly);
	if (deg(minpoly) == -1) {
	    checkOk = false;
	    printf("fail 2\n");
	    cout << "vec:" << vec << endl;
	    break;
	}
	if (!non_reducible(minpoly, mexp)) {
	    checkOk = false;
	    printf("fail 3 deg=%ld\n", degree);
	    //cout << "vec:" << vec << endl;
	    break;
	}
    }
    if (checkOk) {
	printf("check 64 OK!\n");
	printBinary(stdout, minpoly);
	fflush(stdout);
    } else {
	printf("check 64 NG!\n");
	fflush(stdout);
    }
    return checkOk;
}

bool check32(void) {
    int j;
    bool checkOk;
    GF2X minpoly;
    GF2X lcmpoly;
    vec_GF2 vec;
    long degree;
  
    vec.FixLength(2 * maxdegree);
    init_gen_rand(123);
    checkOk = true;
    for (j = 0; j < 10; j++) {
	if (!generating_polynomial32(vec, j, maxdegree)) {
	    checkOk = false;
	    printf("fail 1\n");
	    break;
	}
	berlekampMassey(minpoly, maxdegree, vec);
	degree = deg(minpoly);
	printf("degree = %ld\n", degree);
	printBinary(stdout, minpoly);
	if (deg(minpoly) == -1) {
	    checkOk = false;
	    printf("fail 2\n");
	    break;
	}
	if (!non_reducible(minpoly, mexp)) {
	    checkOk = false;
	    printf("fail 3 deg=%ld\n", degree);
	    //cout << "vec:" << vec << endl;
	    break;
	}
    }
    if (checkOk) {
	printf("check 32 OK!\n");
	printBinary(stdout, minpoly);
	fflush(stdout);
    } else {
	printf("check 32 NG!\n");
	fflush(stdout);
    }
    return checkOk;
}

int main(int argc, char* argv[]){
    uint32_t p1, p2, p3, p4, p5, p6, p7, p8, p9;

    if (argc != 10) {
	printf("usage:%s p1, p2, ... p9\n", argv[0]);
	exit(1);
    }
    p1 = (uint32_t)strtol(argv[1], NULL, 10);
    p2 = (uint32_t)strtol(argv[2], NULL, 10);
    p3 = (uint32_t)strtol(argv[3], NULL, 10);
    p4 = (uint32_t)strtol(argv[4], NULL, 10);
    p5 = (uint32_t)strtol(argv[5], NULL, 10);
    p6 = (uint32_t)strtol(argv[6], NULL, 10);
    p7 = (uint32_t)strtol(argv[7], NULL, 10);
    p8 = (uint32_t)strtol(argv[8], NULL, 10);
    p9 = (uint32_t)strtol(argv[9], NULL, 10);
    setup_param(p1, p2, p3, p4, p5, p6, p7, p8, p9);
    maxdegree = get_rnd_maxdegree();
    mexp = get_rnd_mexp();
    printf("MEXP = %d\n", mexp);
    print_param(stdout);
    fflush(stdout);
    check128();
    check64();
    check32();
    return 0;
}
