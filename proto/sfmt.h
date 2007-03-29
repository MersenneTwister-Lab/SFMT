/* Simple and Fast MT 2006/1/21 */
#ifndef __SFMT_H__
#define __SFMT_H__

#include <stdint.h>
#include <stdio.h>

void setup_param(uint32_t rndarray[], int *idx);
unsigned int get_rnd_maxdegree(void);
unsigned int get_rnd_mexp(void);
void print_param(FILE *fp);
void print_param2(FILE *fp);

void init_gen_rand(uint32_t seed);
uint32_t gen_rand(void);
void fill_array(uint32_t array[][4], int size);
#endif
