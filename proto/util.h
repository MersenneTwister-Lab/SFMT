#include <stdio.h>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>

NTL_CLIENT

void berlekampMassey(GF2X& minpoly, unsigned int maxdegree, vec_GF2& vec);
void printBinary(FILE *fp, GF2X& poly);
