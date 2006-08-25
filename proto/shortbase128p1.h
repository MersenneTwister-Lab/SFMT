#ifndef __SHORTBASE128_H__
#define __SHORTBASE128_H__ 1

#include <stdbool.h>
#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>

extern "C" {
#include "sfmt_p1_st.h"
}

NTL_CLIENT;

void set_up(uint32_t bit_mode, uint32_t len, uint32_t p_mode);
int get_shortest_base(sfmt_t *sfmt);

#endif
