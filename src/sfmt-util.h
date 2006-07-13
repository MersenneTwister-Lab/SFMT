#ifndef __SFMT_UTIL_H__
#define __SFMT_UTIL_H__ 1

#include <stdbool.h>
#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>

#include "sfmt-st.h"

NTL_CLIENT;

void make_zero_state(sfmt_t *sfmt, const GF2X& poly);
void fill_state_random(sfmt_t *sfmt, FILE *frandom);
bool generating_polynomial128(sfmt_t *sfmt, vec_GF2& vec, unsigned int bitpos, 
			      unsigned int maxdegree);
void generating_polynomial32(sfmt_t *sfmt, vec_GF2& vec,
			     unsigned int bitpos, 
			     unsigned int maxdegree);
#endif
