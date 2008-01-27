#include "dsfmt.h"
#include "dsfmt-util.h"

void static generating_polynomial104_hi(DSFMT& dsfmt,
					vec_GF2& vec,
					unsigned int bitpos, 
					unsigned int maxdegree) {
    unsigned int i;
    uint64_t ar[maxdegree*2][2];
    uint64_t mask;
    uint64_t bit;

    mask = (uint64_t)1UL << (51 - bitpos);
    dsfmt.gen_rand104spar(ar, 2 * maxdegree);
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	bit = (ar[i][1] & mask);
	vec[i] = (bit != 0);
    }
}

void static generating_polynomial104_low(DSFMT& dsfmt,
					 vec_GF2& vec,
					 unsigned int bitpos, 
					 unsigned int maxdegree) {
    unsigned int i;
    uint64_t ar[maxdegree*2][2];
    uint64_t mask;
    uint64_t bit;

    mask = (uint64_t)1UL << (51 - bitpos);
    dsfmt.gen_rand104spar(ar, 2 * maxdegree);
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	bit = (ar[i][0] & mask);
	vec[i] = (bit != 0);
    }
}

void generating_polynomial104(DSFMT& dsfmt,
			      vec_GF2& vec, 
			      unsigned int bitpos, 
			      unsigned int maxdegree) {
    if (bitpos < 52) {
	generating_polynomial104_hi(dsfmt, vec, bitpos, maxdegree);
    } else {
	generating_polynomial104_low(dsfmt, vec, bitpos - 52, maxdegree);
    }
}
