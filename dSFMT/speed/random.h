#ifndef RANDOM_H
#define RANDOM_H
#include "c99.h"

INLINE void init_gen_rand(uint64_t seed);
INLINE double gen_rand(void);
INLINE double genrand_close1_open2(void);
INLINE static double genrand_open_close(void);
INLINE static double genrand_close_open(void);
INLINE double genrand_open_open(void);
INLINE void fill_array_open_close(double array[], int size);
INLINE void fill_array_close_open(double array[], int size);
INLINE void fill_array_open_open(double array[], int size);
INLINE void fill_array_close1_open2(double array[], int size);
INLINE unsigned int get_rnd_maxdegree(void);
INLINE unsigned int get_rnd_mexp(void);
INLINE unsigned int get_onetime_rnds(void);

INLINE static double genrand_close_open(void) {
    return genrand_close1_open2() - 1.0;
}

INLINE static double genrand_open_close(void) {
    return 2.0 - genrand_close1_open2();
}
#endif
