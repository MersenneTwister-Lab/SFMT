/* Simple and Fast MT 2006/1/21 */
#ifndef __SFMT_H__
#define __SFMT_H__

#include <stdint.h>
#include <stdio.h>

inline void setup_param(unsigned int p1, unsigned int p2, unsigned int p3, 
		 unsigned int p4, unsigned int p5, unsigned int p6);
inline unsigned int get_rnd_maxdegree(void);
inline unsigned int get_rnd_mexp(void);
inline unsigned int get_onetime_rnds(void);
inline void print_param(FILE *fp);
inline void print_param2(FILE *fp);
inline void print_state(FILE *fp);

inline void init_gen_rand(uint32_t seed);
inline uint32_t gen_rand(void);
inline void gen_rand_all(void);

#endif
