#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "util.h"

extern "C" {
#include "sfmt.h"
}


#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>

NTL_CLIENT

void generating_polynomial(vec_GF2& vec, unsigned int bitpos, 
			   unsigned int maxdegree)
{
    unsigned int i;
    unsigned int mask = 1UL << bitpos;

    vec.FixLength(2 * maxdegree);
    i = 0;
    while ((gen_rand() & mask) == 0) {
	i++;
	if(i > 2 * maxdegree){
	    printf("generating_polynomial:too much zeros\n");
	    vec[0] = 1;
	    return;
	}
    }
    vec[0] = 1;

    for (i=1; i<= 2 * maxdegree-1; i++) {
	if ((gen_rand() & mask) == mask){
	    vec[i] = 1;
	}
    }
}

int main(int argc, char *argv[]) {
    GF2X minpoly;
    vec_GF2 vec;
    uint32_t seed;
    int maxdegree;

    if (argc == 1) {
	seed = 1234;
    } else {
	seed = strtol(argv[1], NULL, 10);
    }

    maxdegree = get_rnd_maxdegree();
    init_gen_rand(seed);
    generating_polynomial(vec, 0, maxdegree);
    printf("vec.length() = %ld\n", vec.length());
    berlekampMassey(minpoly, maxdegree, vec);
    printf("deg(minpoly) = %ld\n", deg(minpoly));
    return 0;
}
