extern "C" {
#include "dsfmtL2-st.h"
}
#include "dsfmtL2-util.h"

void static generating_polynomial104_hi(dsfmt_t *dsfmt, vec_GF2& vec,
					unsigned int bitpos, 
					unsigned int maxdegree) {
    unsigned int i;
    static uint64_t ar[MAXDEGREE*2][2];
    uint64_t mask;
    uint64_t bit;

    mask = (uint64_t)1UL << (51 - bitpos);
    gen_rand104spar(dsfmt, ar, 2 * maxdegree);
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	bit = (ar[i][1] & mask);
	vec[i] = (bit != 0);
    }
}

void static generating_polynomial104_low(dsfmt_t *dsfmt, vec_GF2& vec,
					 unsigned int bitpos, 
					 unsigned int maxdegree) {
    unsigned int i;
    static uint64_t ar[MAXDEGREE*2][2];
    uint64_t mask;
    uint64_t bit;

    mask = (uint64_t)1UL << (51 - bitpos);
    gen_rand104spar(dsfmt, ar, 2 * maxdegree);
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	bit = (ar[i][0] & mask);
	vec[i] = (bit != 0);
    }
}

void generating_polynomial104(dsfmt_t *dsfmt, vec_GF2& vec, 
			      unsigned int bitpos, 
			      unsigned int maxdegree) {
    if (bitpos < 52) {
	generating_polynomial104_hi(dsfmt, vec, bitpos, maxdegree);
    } else {
	generating_polynomial104_low(dsfmt, vec, bitpos - 52, maxdegree);
    }
}
