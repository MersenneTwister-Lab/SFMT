#ifndef DSFMTNL_UTIL_H
#define DSFMTNL_UTIL_H

#include "dsfmt.h"

#include <NTL/vec_GF2.h>
NTL_CLIENT;

void generating_polynomial104(DSFMT& dsfmt,
			      vec_GF2& vec, 
			      unsigned int bitpos, 
			      unsigned int maxdegree);

#endif
