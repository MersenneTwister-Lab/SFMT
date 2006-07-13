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
#include "sfmt-util.h"
#include "shortbase128.h"

NTL_CLIENT;

int non_reducible(GF2X& fpoly, int degree);
void search(unsigned int n);

static unsigned long all_count = 0;
static unsigned long pass_count = 0;
static unsigned int maxdegree;
static unsigned int mexp;
static unsigned int succ = 0;

#if 1
static FILE *frandom;
#endif

bool print_shortest(sfmt_t *sfmt, GF2X& poly) {
    unsigned int bit;
    GF2X lcmpoly;
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    sfmt_t sfmt_save;
    vec_GF2 vec;
    int shortest;
    uint32_t i, j;
    int dist_sum;
    int count;
    int old;
    int lcmcount;

    sfmt->special = false;
    sfmt_save = *sfmt;
    vec.SetLength(2 * maxdegree);
    generating_polynomial128(sfmt, vec, 0, maxdegree);
    berlekampMassey(lcmpoly, maxdegree, vec);
#if 0
    if (check_minpoly128(sfmt, lcmpoly, 0)) {
	printf("check minpoly OK!\n");
	DivRem(tmp, rempoly, lcmpoly, poly);
	if (deg(rempoly) != -1) {
	    printf("rem != 0 deg rempoly = %ld: 0\n", deg(rempoly));
	}

    } else {
	printf("check minpoly NG!\n");
    }
#endif
    for (i = 1; i < 128; i++) {
	//sfmt = sfmt_save;
	generating_polynomial128(sfmt, vec, i, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
#if 0
	DivRem(tmp, rempoly, lcmpoly, poly);
	if (deg(rempoly) != -1) {
	    printf("rem != 0 deg rempoly = %ld: %d\n", deg(rempoly), i);
	}
#endif
    }
#if 1 // 0状態を作るにはこれは不要？
    lcmcount = 0;
    while (deg(lcmpoly) < (long)maxdegree) {
	if (lcmcount > 1000) {
	    printf("failure: deg = %ld\n", deg(lcmpoly));
	    return false;
	}
	errno = 0;
	fill_state_random(sfmt, frandom);
	for (j = 0; j < 10; j++) {
	    int z = (unsigned int)getw(frandom) % 128;
	    generating_polynomial128(sfmt, vec, z, maxdegree);
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
    if (deg(lcmpoly) != (long)maxdegree) {
	printf("fail to get lcm, deg = %ld\n", deg(lcmpoly));
	return false;
    }
#endif
    printf("----------\n");
    printf("succ = %u\n", ++succ);
    printf("deg = %ld\n", deg(poly));
    print_param(stdout);
    //print_param2(stdout);
    printBinary(stdout, poly);
#if 0
    *sfmt = sfmt_save;
    if (check_minpoly128(sfmt, lcmpoly, 0)) {
	printf("check minpoly 2 OK!\n");
    } else {
	printf("check minpoly 2 NG!\n");
    }
#endif
    printf("deg lcm poly = %ld\n", deg(lcmpoly));
    printf("weight = %ld\n", weight(lcmpoly));
    printBinary(stdout, lcmpoly);
    DivRem(tmp, rempoly, lcmpoly, poly);
    printf("deg tmp = %ld\n", deg(tmp));
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld\n", deg(rempoly));
	return false;
    }
    *sfmt = sfmt_save;
#if 0
    printf("chek sfmt status \n");
    for (i = 0; i < 10; i++) {
	printf("%u ", sfmt->sfmt[i / 4][i % 4]);
	if (i % 5 == 4) {
	    printf("\n");
	}
    }
    printf("\n");
#endif
    make_zero_state(sfmt, tmp);
#if 0
    printf("chek sfmt status \n");
    for (i = 0; i < 10; i++) {
	printf("%u ", sfmt->sfmt[i / 4][i % 4]);
	if (i % 5 == 4) {
	    printf("\n");
	}
    }
    printf("\n");
#endif
    sfmt_save = *sfmt;
    // チェック
#if 0
    for (i = 0; i < 128; i++) {
	generating_polynomial128(sfmt, vec, i, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	if (deg(minpoly) != MEXP) {
	    printf("deg zero state = %ld\n", deg(minpoly));
	    for (j = 0; j < 10; j++) {
		printf("%d \n", gen_rand32(sfmt));
	    }
	    cout << "vec =" << vec << endl;
	    cout << "minpoly = " << minpoly << endl; 
	    return false;
	}
    }
#endif
    //check_vector128(sfmt);

#if 1
    dist_sum = 0;
    count = 0;
    old = 0;
    printf("128 bit k-distribution\n");
    for (bit = 1; bit <= 128; bit++) {
	shortest = get_equiv_distrib128(bit, sfmt);
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
    printf("128bit D.D:%7d, DUP:%5d\n", dist_sum, count);
#endif
#if 1
    dist_sum = 0;
    count = 0;
    old = 0;
    printf("64 bit k-distribution\n");
    for (bit = 1; bit <= 64; bit++) {
	shortest = get_equiv_distrib64(bit, sfmt);
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
    printf("64bit D.D:%7d, DUP:%5d\n", dist_sum, count);
#endif
    dist_sum = 0;
    count = 0;
    old = 0;
    printf("32 bit k-distribution\n");
    //printf("start calc distribution\n");
    //fflush(stdout);
    for (bit = 1; bit <= 32; bit++) {
    // DEBUG DEBUG DEBUG
    //for (bit = 1; bit <= 1; bit++) {
	shortest = get_equiv_distrib32(bit, sfmt);
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
    printf("32bit D.D:%7d, DUP:%5d\n", dist_sum, count);
    return true;
}

void search(unsigned int n) {
    int j;
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
	if (checkOk && print_shortest(&sfmt, minpoly)) {
	    printf("----------\n");
	    printf("succ = %u\n", ++succ);
	    printf("deg = %ld\n", deg(minpoly));
	    print_param(stdout);
	    //print_param2(stdout);
	    printBinary(stdout, minpoly);
	    fflush(stdout);
	    //printf("lcm:\n");
	    //printBinary(stdout, lcmpoly);
	    //printf("weight = %ld\n", weight(lcmpoly));
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
    print_version(stdout);
    printf("now search %d times\n", n);
    fflush(stdout);
#if 1
    frandom = fopen("/dev/random", "r");
    if (errno) {
	printf("main:%s\n", strerror(errno));
	exit(1);
    }
#endif
    search(n);
#if 0
    fclose(frandom);
#endif
    return 0;
}
