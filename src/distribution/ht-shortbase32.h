#ifndef __SHORTBASE32_H__
#define __SHORTBASE32_H__ 1

#include <stdbool.h>
#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>

#include "ht-st.h"

NTL_CLIENT;

void set_up(uint32_t len);
int get_shortest_base(ht_rand *sfmt);

#endif
