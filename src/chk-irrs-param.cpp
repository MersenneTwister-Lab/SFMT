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

    i = 0;
    ran = gen_rand32();
    mask = (uint32_t)1UL << (31 - bitpos);
    bit = ran & mask;
    while (!bit) {
	i++;
	if(i > 2 * maxdegree){
	    vec[0] = 1;
	    return false;
	}
	ran = gen_rand32();
	bit = ran & mask;
    }
    vec[0] = 1;
    
    for (i=1; i<= 2 * maxdegree-1; i++) {
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

    i = 0;
    ran = gen_rand64();
    mask = (uint64_t)1ULL << (63 - bitpos);
    bit = ran & mask;
    while (!bit) {
	i++;
	if(i > 2 * maxdegree){
	    vec[0] = 1;
	    return false;
	}
	ran = gen_rand64();
	bit = ran & mask;
    }
    vec[0] = 1;

    for (i=1; i<= 2 * maxdegree-1; i++) {
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

    i = 0;
    gen_rand128(&hi, &low);
    mask = (uint64_t)1ULL << (63 - bitpos);
    bit = hi & mask;
    while (!bit) {
	i++;
	if(i > 2 * maxdegree){
	    vec[0] = 1;
	    return false;
	}
	gen_rand128(&hi, &low);
	bit = hi & mask;
    }
    vec[0] = 1;

    for (i=1; i<= 2 * maxdegree-1; i++) {
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

    i = 0;
    gen_rand128(&hi, &low);
    mask = (uint64_t)1ULL << (63 - bitpos);
    bit = low & mask;
    while (!bit) {
	i++;
	if(i > 2 * maxdegree){
	    vec[0] = 1;
	    return false;
	}
	gen_rand128(&hi, &low);
	bit = low & mask;
    }
    vec[0] = 1;

    for (i=1; i<= 2 * maxdegree-1; i++) {
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
  
    vec.FixLength(2 * maxdegree);
    init_gen_rand(123);
    checkOk = true;
    for (j = 0; j < 128; j++) {
	if (!generating_polynomial128(vec, j, maxdegree)) {
	    checkOk = false;
	    break;
	}
	berlekampMassey(minpoly, maxdegree, vec);
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
  
    vec.FixLength(2 * maxdegree);
    init_gen_rand(123);
    checkOk = true;
    for (j = 0; j < 1; j++) {
	if (!generating_polynomial64(vec, j, maxdegree)) {
	    checkOk = false;
	    break;
	}
	berlekampMassey(minpoly, maxdegree, vec);
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
  
    vec.FixLength(2 * maxdegree);
    init_gen_rand(123);
    checkOk = true;
    for (j = 0; j < 1; j++) {
	if (!generating_polynomial32(vec, j, maxdegree)) {
	    checkOk = false;
	    break;
	}
	berlekampMassey(minpoly, maxdegree, vec);
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
