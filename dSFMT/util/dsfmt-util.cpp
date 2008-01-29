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

bool static check_minpoly104_hi(const DSFMT& sfmt, const GF2X& minpoly,
				unsigned int bitpos) {
    uint32_t sum;
    int size = DSFMT::get_rnd_maxdegree() + 10;
    DSFMT tmp(sfmt);
    uint64_t ar[size][2];
    uint64_t mask;
    int i;

    sum = 0;
    mask = (uint64_t)1UL << (51 - bitpos);
    for (int j = 0; j < 10; j++) {
	tmp.gen_rand104spar(ar, size);
	for (i = 0; i <= deg(minpoly); i++) {
	    if (mask & ar[i][0] != 0) {
		sum ^= 1;
	    }
	}
	if (sum != 0) {
	    return false;
	}
    }
    return true;
}

bool static check_minpoly104_low(const DSFMT& sfmt, const GF2X& minpoly,
				 unsigned int bitpos) {
    uint32_t sum;
    int size = DSFMT::get_rnd_maxdegree() + 10;
    DSFMT tmp(sfmt);
    uint64_t ar[size][2];
    uint64_t mask;
    int i;

    sum = 0;
    mask = (uint64_t)1UL << (51 - bitpos);
    for (int j = 0; j < 10; j++) {
	tmp.gen_rand104spar(ar, size);
	for (i = 0; i <= deg(minpoly); i++) {
	    if (mask & ar[i][1] != 0) {
		sum ^= 1;
	    }
	}
	if (sum != 0) {
	    return false;
	}
    }
    return true;
}

bool check_minpoly104(const DSFMT& sfmt, const GF2X& minpoly,
		      unsigned int bitpos) {
    if (bitpos < 52) {
	return check_minpoly104_hi(sfmt, minpoly, bitpos);
    } else {
	return check_minpoly104_low(sfmt, minpoly, bitpos - 52);
    }
}

void make_zero_state(DSFMT& dsfmt, const GF2X& poly) {
    DSFMT tmp(dsfmt);
    bool first = true;
    uint64_t ar[2];
    int i;

    for (i = 0; i <= deg(poly); i++) {
	if (coeff(poly, i) != 0) {
	    if (first) {
		dsfmt = tmp;
		first = false;
	    } else {
		dsfmt.add(tmp);
	    }
	}
	tmp.gen_rand104sp(ar, 0);
    }
}
