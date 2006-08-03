/* Simple and Fast MT 2006/1/21 */
#ifndef __SFMT_H__
#define __SFMT_H__
#include "string.h"
#include "c99.h"

INLINE void setup_param(unsigned int p1, unsigned int p2, unsigned int p3, 
		 unsigned int p4, unsigned int p5, unsigned int p6);
INLINE unsigned int get_rnd_maxdegree(void);
INLINE unsigned int get_rnd_mexp(void);
INLINE unsigned int get_onetime_rnds(void);
INLINE void print_param(FILE *fp);
INLINE void print_param2(FILE *fp);
INLINE void print_state(FILE *fp);

INLINE void init_gen_rand(uint32_t seed);
INLINE uint32_t gen_rand(void);
//INLINE void gen_rand_all(void);
INLINE void fill_array_block(uint32_t array[], uint32_t block_num);
INLINE void fill_array(uint32_t array[], uint32_t size);
#endif
