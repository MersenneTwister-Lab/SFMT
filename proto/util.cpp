/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <stdio.h>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/GF2XFactoring.h>
#include "util.h"

NTL_CLIENT;

int non_reducible(GF2X& fpoly, int degree) {
    static const GF2X t2(2, 1);
    static const GF2X t1(1, 1);
    GF2X t2m;
    GF2X t;
    GF2X alpha;
    int m;

    t2m = t2;
    if (deg(fpoly) < degree) {
	return 0;
    }
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
//	if ((deg(fpoly) > degree) && (deg(fpoly) <= degree + m)) {
//	    return 0;
//	}
	t2m *= t2m;
	t2m %= fpoly;
	add(t, t2m, t1);
    }
    if (deg(fpoly) != degree) {
	return 0;
    }
    return IterIrredTest(fpoly);
}

void berlekampMassey(GF2X& minpoly, unsigned int maxdegree, vec_GF2& genvec) 
{
    GF2X zero;

    if (genvec.length() == 0) {
	minpoly = zero;
	return;
    }
    MinPolySeq(minpoly, genvec, maxdegree);
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
	/* printf("%1d", (unsigned int)(poly[j] >> (i % WORDLL)) & 0x1ULL);*/
	if ((i % 32) == 31) {
	    fprintf(fp, "\n");
	}
    }
    fprintf(fp, "deg=%ld\n", deg(poly));
}

void LCM(GF2X& lcm, const GF2X& x, const GF2X& y) {
    GF2X gcd;
    mul(lcm, x, y);
    GCD(gcd, x, y);
    lcm /= gcd;
}

int32_t gauss_plus(mat_GF2& mat) {
    int32_t rank;
    int32_t rows;
    int32_t cols;
    int32_t i, j, pos;
    
    rank = gauss(mat);
    rows = mat.NumRows();
    cols = mat.NumCols();
    pos = 0;
    for (i = 0; i < rows; i++) {
	while ((i + pos < cols) && IsZero(mat.get(i, i + pos))) {
	    pos++;
	}
	if (i + pos >= cols) {
	    break;
	}
	//cout << "mat[i] = " << mat[i] << endl;
	for (j = 0; j < i; j++) {
	    //cout << "mat[j] = " << mat[j] << endl;
	    if (IsOne(mat.get(j, i + pos))) {
		mat[j] += mat[i];
		//cout << "new mat[j] = " << mat[j] << endl;
	    }
	}
    }
    return rank;
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
