#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>

#include "sfmt-cls.h"
#include "shortbase.h"

NTL_CLIENT;

int get_equiv_distrib(int bit, SFMT& sfmt);
void make_zero_state(SFMT& sfmt, GF2X& poly);
void test_shortest(char *filename);

static int mexp;
static int maxdegree;
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

void LCM(GF2X& lcm, const GF2X& x, const GF2X& y) {
    GF2X gcd;
    mul(lcm, x, y);
    GCD(gcd, x, y);
    lcm /= gcd;
}

void berlekampMassey(GF2X& minpoly, SFMT& sfmt, unsigned int maxdegree,
		     unsigned int bitpos) {
    vec_GF2 genvec = vec_GF2(INIT_SIZE, 2 * maxdegree);
    GF2X zero;

    generating_polynomial(genvec, sfmt, bitpos, maxdegree);
    if (genvec.length() == 0) {
	minpoly = zero;
	return;
    }
    MinPolySeq(minpoly, genvec, maxdegree);
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

void make_zero_state(SFMT& sfmt, GF2X& poly) {
    SFMT sfmtnew;
    int i;

    for (i = 0; i <= deg(poly); i++) {
	if (coeff(poly, i) != 0) {
	    sfmtnew.add(sfmt);
	}
	sfmt.next_state();
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
    SFMT *sfmt;
    SFMT sfmt_save;
    int shortest;
    int i;
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
    sfmt = new SFMT(123);
    sfmt_save = *sfmt;
    print_param(stdout);
    readFile(poly, fp);
    printf("deg poly = %ld\n", deg(poly));
    fclose(fp);
    berlekampMassey(lcmpoly, *sfmt, maxdegree, 0);
    for (i = 1; i < 32; i++) {
	berlekampMassey(minpoly, *sfmt, maxdegree, i);
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
    }
  
    DivRem(tmp, rempoly, lcmpoly, poly);
    printf("deg tmp = %ld\n", deg(tmp));
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld\n", deg(rempoly));
	delete sfmt;
	exit(1);
    }
    printf("weight = %ld\n", weight(lcmpoly));
    *sfmt = sfmt_save;
    make_zero_state(*sfmt, tmp);
    dist_sum = 0;
    count = 0;
    old = 0;
    for (bit = 1; bit <= 32; bit++) {
	shortest = get_equiv_distrib(bit, *sfmt);
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
    }
    printf("D.D:%7d, DUP:%5d\n", dist_sum, count);
    delete sfmt;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("usage:%s filename %d\n", argv[0], argc);
    exit(1);
  }
  mexp = get_rnd_mexp();
  maxdegree = get_rnd_maxdegree();
  printf("mexp = %d\n", mexp);
  test_shortest(argv[1]);
  return 0;
}
