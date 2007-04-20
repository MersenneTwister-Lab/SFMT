/** 
 * @file SFMT.h 
 *
 * @brief SIMD oriented Fast Mersenne Twister(SFMT) pseudorandom
 * number generator
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2006, 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software.
 * see LICENSE.txt
 *
 * @note We assume that your system has stdint.h.  If your system
 * doesn't have stdint.h, you have to typedef uint32_t and uint64_t,
 */

#ifndef SFMT_H
#define SFMT_H

#include <stdio.h>
#include <stdint.h>


void init_gen_rand(uint32_t seed);
void init_by_array(uint32_t init_key[], int key_length);

/**
* This function generates and returns 32-bit pseudorandom number.
 * init_gen_rand or init_by_array must be called before this function.
 * @return 32-bit pseudorandom number
 */
uint32_t gen_rand32(void);
/**
* This function generates and returns 64-bit pseudorandom number.
 * init_gen_rand or init_by_array must be called before this function.
 * The function gen_rand64 should not be called after gen_rand32,
 * unless an initialization is again executed. 
 * @return 64-bit pseudorandom number
 */
uint64_t gen_rand64(void);

void fill_array32(uint32_t array[], int size);
void fill_array64(uint64_t array[], int size);

inline char *get_idstring(void);
inline int get_min_array_size32(void);
inline int get_min_array_size64(void);

/* These real versions are due to Isaku Wada */
/** generates a random number on [0,1]-real-interval */
inline static double to_real1(uint32_t v)
{
    return v * (1.0/4294967295.0); 
    /* divided by 2^32-1 */ 
}

/** generates a random number on [0,1]-real-interval */
inline static double genrand_real1(void)
{
    return to_real1(gen_rand32());
}

/** generates a random number on [0,1)-real-interval */
inline static double to_real2(uint32_t v)
{
    return v * (1.0/4294967296.0); 
    /* divided by 2^32 */
}

/** generates a random number on [0,1)-real-interval */
inline static double genrand_real2(void)
{
    return to_real2(gen_rand32());
}

/** generates a random number on (0,1)-real-interval */
inline static double to_real3(uint32_t v)
{
    return (((double)v) + 0.5)*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/** generates a random number on (0,1)-real-interval */
inline static double genrand_real3(void)
{
    return to_real3(gen_rand32());
}
/** These real versions are due to Isaku Wada */

/** generates a random number on [0,1) with 53-bit resolution*/
inline static double to_res53(uint64_t v) 
{ 
    return v * (1.0/18446744073709551616.0L);
}

/** generates a random number on [0,1) with 53-bit resolution*/
inline static double genrand_res53(void) 
{ 
    return to_res53(gen_rand64());
} 
#endif
