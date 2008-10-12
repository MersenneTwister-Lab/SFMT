#include "dsfmt.h"
#include "util.h"
#include "dsfmt-util.h"

void generating_polynomial104(DSFMT& dsfmt,
			      vec_GF2& vec,
			      int bitpos, 
			      int maxdegree) {
    int i;
    int pos;
    uint64_t ar[maxdegree * 2][2];
    uint64_t mask;
    uint64_t bit;

    if (bitpos < 52) {
	pos = 1;
	mask = (uint64_t)1UL << (51 - bitpos);
    } else {
	pos = 0;
	mask = (uint64_t)1UL << (103 - bitpos);
    }
    dsfmt.gen_rand104spar(ar, 2 * maxdegree);
    clear(vec);
    for (i = 0; i < 2 * maxdegree; i++) {
	bit = (ar[i][pos] & mask);
	vec[i] = (bit != 0);
    }
}

void generating_polynomial128(DSFMT& dsfmt,
			      vec_GF2& vec,
			      int bitpos, 
			      int maxdegree) {
    int i;
    int pos;
    uint64_t ar[maxdegree * 2][2];
    uint64_t mask;
    uint64_t bit;

    if (bitpos < 64) {
	pos = 1;
	mask = (uint64_t)1UL << (63 - bitpos);
    } else {
	pos = 0;
	mask = (uint64_t)1UL << (128 - bitpos);
    }
    dsfmt.gen_rand104spar(ar, 2 * maxdegree);
    clear(vec);
    for (i = 0; i < 2 * maxdegree; i++) {
	bit = (ar[i][pos] & mask);
	vec[i] = (bit != 0);
    }
}

void generating_polynomial104(uint64_t seed,
			      vec_GF2& vec, 
			      int bitpos, 
			      int maxdegree) {
    DSFMT dsfmt(seed);

    generating_polynomial104(dsfmt, vec, bitpos, maxdegree);
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
	    if ((mask & ar[i][0]) != 0) {
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
	    if ((mask & ar[i][1]) != 0) {
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

int deg_min_pol(DSFMT& dsfmt, int maxdegree) {
    vec_GF2 vec;
    GF2X minpoly;
    DSFMT tmp(dsfmt);

    vec.SetLength(2 * maxdegree);
    generating_polynomial104(tmp, vec, 0, maxdegree);
    berlekampMassey(minpoly, maxdegree, vec);
    return (int)deg(minpoly);
}

int min_pol(GF2X& minpoly, DSFMT& dsfmt, int maxdegree) {
    vec_GF2 vec;
    DSFMT tmp(dsfmt);

    vec.SetLength(2 * maxdegree);
    generating_polynomial104(tmp, vec, 0, maxdegree);
    berlekampMassey(minpoly, maxdegree, vec);
    return (int)deg(minpoly);
}
