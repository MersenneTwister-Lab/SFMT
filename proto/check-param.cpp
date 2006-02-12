/* ./check-param params -> irr min poly and distribution */
#include <string.h>
#include <stdio.h>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>

extern "C" {
#include "ht-st.h"
#include "shortbase.h"
}

NTL_CLIENT

int get_equiv_distrib(int bit, ht_rand *p_ht);
void make_zero_state_inv(ht_rand *ht, GF2X& poly);
void test_shortest(GF2X& poly);
int test_distrib(ht_rand *ht);
int check_kiyaku(GF2X& irrpoly);
int non_reducible(GF2X& fpoly, int degree);
void printBinary(FILE *fp, GF2X& poly);

static int mexp;
static int maxdegree;
const int limit = 300;

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

int get_equiv_distrib(int bit, ht_rand *p_ht) {
    static ht_rand htnew;
    int shortest;

    //fprintf(stderr, "now start get_equiv %d\n", bit);
    htnew = *p_ht;
    set_bit_len(bit);
    shortest = get_shortest_base(bit, &htnew);
    return shortest;
}

void make_zero_state_inv(ht_rand *ht, GF2X& poly) {
    static ht_rand htnew;
    int i;

    memset(&htnew, 0, sizeof(htnew));
    for (i = deg(poly); i >= 0; i--) {
	if (coeff(poly, i) != 0) {
	    add(&htnew, ht);
	}
	next_state(ht);
    }
    *ht = htnew;
}

void make_zero_state(ht_rand *ht, GF2X& poly) {
    static ht_rand htnew;
    int i;

    memset(&htnew, 0, sizeof(htnew));
    for (i = 0; i <= deg(poly); i++) {
	if (coeff(poly, i) != 0) {
	    add(&htnew, ht);
	}
	next_state(ht);
    }
    *ht = htnew;
}

void test_shortest(GF2X& poly) {
    GF2X lcmpoly;
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    static ht_rand ht;
    static ht_rand ht_save;
    int rc;
    int i;

    init_gen_rand(&ht, 123);
    ht_save = ht;
    berlekampMassey(lcmpoly, &ht, maxdegree, 0);
    for (i = 1; i < 32; i++) {
	berlekampMassey(minpoly, &ht, maxdegree, i);
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
    }
  
    DivRem(tmp, rempoly, lcmpoly, poly);
    printf("deg tmp = %ld\n", deg(tmp));
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld\n", deg(rempoly));
	exit(1);
    }
    ht = ht_save;
    make_zero_state(&ht, tmp);
    rc = test_distrib(&ht); 
    if (rc != 0) {
	make_zero_state_inv(&ht, tmp);
	rc = test_distrib(&ht); 
    }
}

int test_distrib(ht_rand *ht) {
    unsigned int bit;
    int shortest;
    int dist_sum;
    int count;
    int old;

    dist_sum = 0;
    count = 0;
    old = 0;
    for (bit = 1; bit <= 32; bit++) {
	shortest = get_equiv_distrib(bit, ht);
	if (shortest > mexp) {
	    printf("k(%d) = %d\n", bit, shortest);
	    printf("distribution greater than mexp!\n");
	    return -1;
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
    return 0;
}

/* minpoly is not input it's output */
int check_kiyaku(GF2X& minpoly) {
    int j;
    int bmOk;
    ht_rand ht;
  
    init_gen_rand(&ht, 12345);
    bmOk = 1;
    for (j = 0; j < 32; j++) {
	berlekampMassey(minpoly, &ht, maxdegree, j);
	if (deg(minpoly) == -1) {
	    bmOk = 0;
	    break;
	}
	if (!non_reducible(minpoly, mexp)) {
	    bmOk = 0;
	    break;
	}
    }
    if (bmOk) {
	printf("----------\n");
	printf("deg = %ld\n", deg(minpoly));
	print_param(stdout);
	printBinary(stdout, minpoly);
	fflush(stdout);
	return 1;
    } else {
	printf("deg = %ld\n", deg(minpoly));
	print_param(stdout);
	printf("%dth minpoly is reducible\n", j);
	//printBinary(stdout, minpoly);
	fflush(stdout);
	return 0;
    }
}

int non_reducible(GF2X& fpoly, int degree) {
    GF2X t2m = GF2X(2, 1);
    GF2X t1 = GF2X(1, 1);
    GF2X t;
    GF2X alpha;
    int m;
    int count;

    if (deg(fpoly) < degree) {
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
		return 0;
	    }
	}
	if ((deg(fpoly) > degree) && (deg(fpoly) <= degree + m)) {
	    return 0;
	}
	t2m *= t2m;
	count++;
	t2m %= fpoly;
	add(t, t2m, t1);
    }
    if (deg(fpoly) != degree) {
	return 0;
    }
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
    for (;m < degree; m++) {
	t2m *= t2m;
	t2m %= fpoly;
	count++;
    }
    add(t, t1, t2m);
    if (deg(t) == -1) {
	return 1;
    } else {
	return 0;
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

int main(int argc, char *argv[]) {
    char *filename;
    GF2X irrpoly;
    FILE *fp;

    if (argc != 2) {
	printf("usage:%s filename %d\n", argv[0], argc);
	exit(1);
    }
    filename = argv[1];

    mexp = get_rnd_mexp();
    maxdegree = get_rnd_maxdegree();
    printf("mexp = %d\n", mexp);
    printf("filename:%s\n", filename);
    fp = fopen(filename, "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("main");
	fclose(fp);
	exit(1);
    }
    read_ht_random(fp);
    print_param(stdout);
    fclose(fp);
    if (check_kiyaku(irrpoly)) {
	test_shortest(irrpoly);
    }
    return 0;
}
