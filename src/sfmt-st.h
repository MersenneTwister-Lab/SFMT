/* Simple and Fast MT 2006/1/21 */
#ifndef __SFMT_H__
#define __SFMT_H__

#include <stdint.h>
#include <stdio.h>

void setup_param(unsigned int p1, unsigned int p2, unsigned int p3, 
		 unsigned int p4, unsigned int p5, unsigned int p6,
		 unsigned int p7, unsigned int p8, unsigned int p9);
unsigned int get_rnd_maxdegree(void);
unsigned int get_rnd_mexp(void);
void print_param(FILE *fp);
void print_param2(FILE *fp);

void init_gen_rand(uint32_t seed);
uint32_t gen_rand32(void);
uint64_t gen_rand64(void);
uint64_t gen_rand128(uint64_t *hi, uint64_t *low);

#endif
