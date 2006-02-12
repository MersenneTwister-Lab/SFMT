#include <string.h>
#include <stdio.h>
#include <iostream>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>

extern "C" {
#include "ht-st.h"
#include "shortbase.h"
}

NTL_CLIENT

int get_equiv_distrib(int bit, ht_rand *p_ht);
void make_zero_state_inv(ht_rand *ht, GF2X& poly);
void test_shortest(char *filename);
void printBinary(FILE *fp, GF2X& poly);

static int mexp;
static int maxdegree;
void generating_polynomial(vec_GF2& vec, ht_rand* ht, unsigned int bitpos, 
			   unsigned int maxdegree)
{
    unsigned int i;
    unsigned int mask = 1UL << bitpos;

    i = 0;
    while ((gen_rand(ht) & mask) == 0) {
	i++;
	if(i > 2 * maxdegree){
	    printf("generating_polynomial:too much zeros\n");
	    vec[0] = 1;
	    return;
	}
    }
    vec[0] = 1;

    for (i=1; i<= 2 * maxdegree-1; i++) {
	if ((gen_rand(ht) & mask) == mask){
	    vec[i] = 1;
	}
    }
}

void printBinary(FILE *fp, GF2X& poly)
{
    int i;
    if (deg(poly) < 0) {
	fprintf(fp, "0deg=-1\n");
	return;
    }
    for(i = 0; i <= deg(poly); i++) {
	if(rep(coeff(poly, i)) == 1) {
	    fprintf(fp, "1");
	} else {
	    fprintf(fp, "0");
	}
	if ((i % 32) == 31) {
	    fprintf(fp, "\n");
	}
    }
    fprintf(fp, "deg=%d\n", deg(poly));
}

void yomitobashi(FILE *fp) {
    char c;
    char bc = '\0';

    while ((c = getc(fp)) != EOF) {
	if (bc == '\n' || bc == '\0') {
	    if (c == '1' || c == '0') {
		ungetc(c, fp);
		return;
	    }
	}
	bc = c;
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

void berlekampMassey(GF2X& minpoly, ht_rand *ht, unsigned int maxdegree,
		     unsigned int bitpos) {
    vec_GF2 genvec = vec_GF2(INIT_SIZE, 2 * maxdegree);
    GF2X zero;

    generating_polynomial(genvec, ht, bitpos, maxdegree);
    if (genvec.length() == 0) {
	minpoly = zero;
	return;
    }
    MinPolySeq(minpoly, genvec, maxdegree);
}

#if 0
int get_equiv_distrib(int bit, ht_rand *p_ht) {
    static ht_rand htnew;
    int shortest;

    //fprintf(stderr, "now start get_equiv %d\n", bit);
    htnew = *p_ht;
    set_bit_len(bit);
    shortest = get_shortest_base(bit, &htnew);
    return shortest;
}
#endif

void make_zero_state_inv(ht_rand *ht, GF2X& poly) {
    static ht_rand htnew;
    int i;

    memset(&htnew, 0, sizeof(htnew));
    //for (i = deg(poly); i >= 0; i--) {
    for (i = 0; i <= deg(poly); i++) {
	if (coeff(poly, i) != 0) {
	    add(&htnew, ht);
	}
	next_state(ht);
    }
    *ht = htnew;
}

void test_shortest(char *filename) {
    FILE *fp;
    GF2X poly;
    GF2X lcmpoly;
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    static ht_rand ht;
    static ht_rand ht_save;
    int i;

    cout << "filename:" << filename << endl;
    fp = fopen(filename, "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("main");
	fclose(fp);
	exit(1);
    }
    read_ht_random(fp);
    init_gen_rand(&ht, 123);
    ht_save = ht;
    print_param(stdout);
    yomitobashi(fp);
    readFile(poly, fp);
    printf("deg poly = %ld\n", deg(poly));
    fclose(fp);
    berlekampMassey(lcmpoly, &ht, maxdegree, 0);
    for (i = 1; i < 32; i++) {
	berlekampMassey(minpoly, &ht, maxdegree, i);
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
    }
  
    printf("lcmpoly\n");
    printBinary(stdout, lcmpoly);
    printf("weight = %ld\n", weight(lcmpoly));
    DivRem(tmp, rempoly, lcmpoly, poly);
    printf("deg tmp = %ld\n", deg(tmp));
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld\n", deg(rempoly));
	exit(1);
    }
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
