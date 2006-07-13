#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>

#include "shortbase128.h"
#include "util.h"

#include "sfmt-st.h"
#include "sfmt-util.h"

NTL_CLIENT;

void test_shortest(sfmt_t *sfmt, GF2X& poly);

static uint32_t seed;
static int mexp;
static int maxdegree;
static FILE *frandom;

void test_shortest(sfmt_t *sfmt, GF2X& poly) {
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
	    return;
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
    if (deg(lcmpoly) != maxdegree) {
	printf("fail to get lcm, deg = %ld\n", deg(lcmpoly));
	exit(1);
    }
#endif
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
	exit(1);
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
	    return;
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
}

int main(int argc, char *argv[]) {
    char *filename;
    FILE *fp;
    GF2X poly;
    sfmt_t sfmt;

    if (argc < 2) {
	printf("usage:%s filename [seed]\n", argv[0]);
	exit(1);
    }
    if (argc >= 3) {
	seed = strtol(argv[2], NULL, 10);
    } else {
	seed = 123;
    }
    if (errno) {
	printf("main:%s\n", strerror(errno));
	exit(1);
    }
    mexp = get_rnd_mexp();
    maxdegree = get_rnd_maxdegree();
    printf("mexp = %d\n", mexp);
    frandom = fopen("/dev/urandom", "r");
    if (errno) {
	printf("main:%s\n", strerror(errno));
	exit(1);
    }

    filename = argv[1];
    printf("filename:%s\n", filename);
    fp = fopen(filename, "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	printf("main:%s\n", strerror(errno));
	fclose(fp);
	exit(1);
    }
    read_random_param(fp);
    init_gen_rand(&sfmt, seed);
    sfmt.special = false;
    print_param(stdout);
    readFile(poly, fp);
    printf("deg poly = %ld\n", deg(poly));
    fclose(fp);
    printBinary(stdout, poly);

    test_shortest(&sfmt, poly);
    fclose(frandom);
    return 0;
}
