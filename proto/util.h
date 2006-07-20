#include <stdio.h>
#include <stdint.h>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>

NTL_CLIENT;

int non_reducible(GF2X& fpoly, int degree);
void berlekampMassey(GF2X& minpoly, unsigned int maxdegree, vec_GF2& vec);
void printBinary(FILE *fp, GF2X& poly);
int32_t gauss_plus(mat_GF2& mat);
void LCM(GF2X& lcm, const GF2X& x, const GF2X& y);
void readFile(GF2X& poly, FILE *fp);
