/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

//#include <string.h>
//#include <limits.h>
//#include <errno.h>
#include <stdio.h>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include "util.h"

NTL_CLIENT

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
}
