/* Simple and Fast MT 2006/1/21 */
#ifndef __SFMT_H__
#define __SFMT_H__
#include "string.h"
#include "c99.h"

INLINE void init_gen_rand(uint32_t seed);
INLINE uint32_t gen_rand(void);
//INLINE void gen_rand_all(void);
INLINE void fill_array(uint32_t array[], int size);
#endif
