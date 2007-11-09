#ifndef DSFMTL_UTIL_H
#define DSFMTL_UTIL_H

extern "C" {
  #include "dsfmtN-st.h"
}
#include <NTL/vec_GF2.h>
NTL_CLIENT;

void generating_polynomial104(dsfmt_t *dsfmt, vec_GF2& vec, 
			      unsigned int bitpos, 
			      unsigned int maxdegree);

#endif
