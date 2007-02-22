/** 
 * @file dSFMT-alti.h 
 *
 * @brief double precision SIMD oriented Fast Mersenne Twister(dSFMT)
 * pseudorandom number generator
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software.
 * see LICENSE.txt
 */

#ifndef DSFMT_ALTI_H
#define DSFMT_ALTI_H

union W128_T {
    vector unsigned int s;
    uint64_t u[2];
    double d[2];
};

inline static vector unsigned int vec_recursion(vector unsigned int a,
						vector unsigned int b,
						vector unsigned int reg,
						vector unsigned int lung)
    __attribute__((always_inline));
#endif
