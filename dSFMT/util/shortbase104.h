#ifndef __SHORTBASE128_H__
#define __SHORTBASE128_H__ 1

#include <stdbool.h>
#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>
#include "dsfmt.h"

NTL_CLIENT;

void set_up(uint32_t bit_mode, uint32_t len, uint32_t p_mode, bool msb);
int get_shortest_base(const DSFMT& dsfmt);

#endif
