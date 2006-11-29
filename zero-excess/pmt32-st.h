/** 
 * @file  pmt32-st.h
 * @brief Pulmonary Mersenne Twister(PMT) pseudorandom number generator.
 * This version uses the data structure.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * @date 2006-09-14
 *
 * Copyright (C) 2006 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */
#ifndef __PMT32_ST_H__
#define __PMT32_ST_H__

#include <inttypes.h>

#if !defined(MEXP)
  #define MEXP 19937
  #define N ((MEXP - 1) / 32)
#elif !defined(N)
  #define N ((MEXP - 1) / 32)
#endif


struct PMT32_T {
    /** the state array. gx[N] is an additional state variable. */
    uint32_t gx[N + 1];
    /** index counter to internal state array */
    int idx;
};
typedef struct PMT32_T pmt32_t;

char *get_idstring(void);
void init_gen_rand(pmt32_t *pmt, uint32_t seed);
void init_by_array(pmt32_t *pmt, uint32_t init_key[], int key_length);
uint32_t gen_rand_int32(pmt32_t *pmt);

#endif
