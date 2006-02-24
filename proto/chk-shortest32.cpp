#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>

#include "sfmt-cls.h"
#include "shortbase.h"
#include "util.h"

NTL_CLIENT;

int get_equiv_distrib(int bit, SFMT& sfmt);
void make_zero_state(SFMT& sfmt, const GF2X& poly);
void test_shortest(char *filename);

static int mexp;
static int maxdegree;
static SFMT::MODE_E mode;
static uint32_t p_mode;

void generating_polynomial(vec_GF2& vec, SFMT& sfmt, unsigned int bitpos,
			   unsigned int maxdegree) {
    unsigned int i;
    vec_GF2 rand;

    i = 0;
    while (IsZero(sfmt.gen_rand(rand, bitpos + 1).get(bitpos))) {
	i++;
	if(i > 2 * maxdegree){
	    printf("generating_polynomial:too much zeros\n");
	    vec[0] = 1;
	    return;
	}
    }
    vec[0] = 1;
    
    for (i=1; i<= 2 * maxdegree-1; i++) {
	vec[i] = sfmt.gen_rand(rand, bitpos + 1).get(bitpos);
    }
}

bool check_minpoly(SFMT& sfmt, const GF2X& minpoly, unsigned int bitpos) {
    GF2 sum;
    vec_GF2 rand;
    int i;

    sum = 0;
    for (int j = 0; j < 500; j++) {
	for (i = 0; i <= deg(minpoly); i++) {
	    sfmt.gen_rand(rand, bitpos + 1);
	    if (IsOne(coeff(minpoly, i))) {
		sum += rand.get(bitpos);
	    }
	}
	if (IsOne(sum)) {
	    return false;
	}
    }
    return true;
}

void readFile(GF2X& poly, FILE *fp) {
    char c;
    unsigned int j = 0;

    while ((c = getc(fp)) != EOF) {
	if (c < ' ') {
	    continue;
	} else if (c == '1') {
	    SetCoeff(poly, j, 1);
	    j++;
	} else if (c == '0') {
	    SetCoeff(poly, j, 0);
	    j++;
	} else {
	    break;
	}
    }
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
	sfmt.next_state32();
    }
    sfmt = sfmtnew;
}

void test_shortest(char *filename) {
    unsigned int bit;
    FILE *fp;
    GF2X poly;
    GF2X lcmpoly;
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    SFMT sfmt;
    SFMT sfmt_save;
    vec_GF2 vec;
    int shortest;
    uint32_t i;
    int dist_sum;
    int count;
    int old;

    cout << "filename:" << filename << endl;
    fp = fopen(filename, "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("main");
	fclose(fp);
	exit(1);
    }
    read_random_param(fp);
    sfmt.reseed(123);
    sfmt.set_mode(SFMT::BIT128);
    sfmt_save = sfmt;
    print_param(stdout);
    readFile(poly, fp);
    printf("deg poly = %ld\n", deg(poly));
    fclose(fp);
    vec.SetLength(2 * maxdegree);
    generating_polynomial(vec, sfmt, 0, maxdegree);
    berlekampMassey(lcmpoly, maxdegree, vec);
#if 1
    if (check_minpoly(sfmt, lcmpoly, 0)) {
	printf("check minpoly OK!\n");
    } else {
	printf("check minpoly NG! deg(minpoly) = %ld\n", deg(lcmpoly));
    }
#endif
    for (i = 0; i < 128; i++) {
	//sfmt = sfmt_save;
	generating_polynomial(vec, sfmt, i, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
    }
#if 1
    sfmt = sfmt_save;
    if (check_minpoly(sfmt, lcmpoly, 0)) {
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
#if 1
    sfmt = sfmt_save;
    cout << sfmt;
    make_zero_state(sfmt, lcmpoly);
    cout << sfmt;
    generating_polynomial(vec, sfmt, 0, maxdegree);
    berlekampMassey(minpoly, maxdegree, vec);
    printf("deg zero lcm = %ld\n", deg(minpoly));
#endif
    sfmt = sfmt_save;
    make_zero_state(sfmt, tmp);
    sfmt.set_mode(mode);
#if 1
    generating_polynomial(vec, sfmt, 0, maxdegree);
    berlekampMassey(minpoly, maxdegree, vec);
    printf("deg zero state = %ld\n", deg(minpoly));
#endif
    dist_sum = 0;
    count = 0;
    old = 0;
    for (bit = 1; bit <= p_mode; bit++) {
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
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
	printf("usage:%s mode filename %d\n", argv[0], argc);
	exit(1);
    }
    p_mode = strtol(argv[1], NULL, 10);
    if (errno != 0) {
	perror("main");
	exit (1);
    }
    if (p_mode == 32) {
	mode = SFMT::BIT32;
    } else if (p_mode == 64) {
	mode = SFMT::BIT64;
    } else {
	mode = SFMT::BIT128;
	p_mode = 128;
    }
    mexp = get_rnd_mexp();
    maxdegree = get_rnd_maxdegree();
    printf("mexp = %d\n", mexp);
    printf("mode = %d\n", mode);
    test_shortest(argv[2]);
    return 0;
}
