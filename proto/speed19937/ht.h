/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */
#ifndef __HT_H__
#define __HT_H__

#include <stdint.h>

void setup_param(unsigned int p_gmm, unsigned int p_gs0, unsigned int p_gs2, 
		 unsigned int p_gs3, unsigned int p_grot1, 
		 unsigned int p_grot2);
unsigned int get_rnd_maxdegree(void);
unsigned int get_rnd_mexp(void);
void print_param(FILE *fp);

void init_gen_rand(int seed);
uint32_t gen_rand(void);

#endif
