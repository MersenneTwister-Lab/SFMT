/*
 * SIMD based Fast Mersenne Twister (SFMT)
 * written by M.Saito 2006.08.29
 * Copyright 2006
 */

#ifndef __SFMT_H__
#define __SFMT_H__

#include <stdio.h>
#include "c99.h"

INLINE unsigned int get_onetime_rnds(void);

INLINE void init_gen_rand(uint32_t seed);
INLINE uint32_t gen_rand(void);
INLINE void fill_array_block(uint32_t array[], uint32_t block_num);

#endif
