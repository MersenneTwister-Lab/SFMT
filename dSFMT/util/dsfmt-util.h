#ifndef DSFMT_UTIL_H
#define DSFMT_UTIL_H

#include "dsfmt.h"
#include <NTL/vec_GF2.h>
#include <NTL/GF2X.h>

NTL_CLIENT;

void generating_polynomial104(DSFMT& dsfmt,
			      vec_GF2& vec, 
			      unsigned int bitpos, 
			      unsigned int maxdegree);
void generating_polynomial104(uint64_t seed,
			      vec_GF2& vec, 
			      unsigned int bitpos, 
			      unsigned int maxdegree);
bool check_minpoly104(const DSFMT& sfmt,
		      const GF2X& minpoly,
		      unsigned int bitpos);
void make_zero_state(DSFMT& sfmt, const GF2X& poly);
#endif
