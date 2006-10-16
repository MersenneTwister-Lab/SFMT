#ifndef __RANDOM_H__
#define __RANDOM_H__
#include "c99.h"

INLINE void init_gen_rand(uint64_t seed);
INLINE double gen_rand(void);
INLINE void fill_array(double array[], int size);
#endif
