/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <stdio.h>
#include "random-inline.h"

#ifndef MEXP
#define MEXP 19937
#endif
#define WORDSIZE 32
#define N ((MEXP-1)/WORDSIZE)
#define NN (N+1) /* gx[N] is an additional state variable */
#define MAXDEGREE (WORDSIZE*NN)

static uint32_t gx[NN];
static unsigned int idx;

#define GMM 294
#define GS2 5
#define GS3 9
#define GROT1 17
#define GROT2 22

//#define MAX_BLOCKS 10

INLINE static void gen_rand_array(uint32_t array[], uint32_t blocks,
				  uint32_t *lung);
INLINE static void gen_rand_all(void);

INLINE unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

INLINE unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

INLINE unsigned int get_onetime_rnds(void) 
{
    return N;
}

INLINE void print_param(FILE *fp) {
    fprintf(fp, "gmm = %u\n", GMM);
    fprintf(fp, "gs2 = %u\n", GS2);
    fprintf(fp, "gs3 = %u\n", GS3);
    fprintf(fp, "grot1 = %u\n", GROT1);
    fprintf(fp, "grot2 = %u\n", GROT2);
    fflush(fp);
}

INLINE void print_state(FILE *fp) {
    int i;

    for (i = 0; i < N; i++) {
	fprintf(fp, "%08x ", gx[i]);
	if (i % 8 == 7) {
	    fprintf(fp, "\n");
	}
    }
}

INLINE void gen_rand_all(void) {
    uint32_t u;
    unsigned int jump;
    unsigned int i;
  
    idx = 0;
    u = gx[N];
    jump = GMM;
    for (i = 0; jump < N; i++) {
	u ^= (gx[i] >> GROT1) 
	    ^ (gx[i] << GROT2);
	u ^= gx[jump];
	u ^= u << GS2;
	gx[i] ^=  u ^ (u << GS3);
	jump++;
    }
    jump -= N;
    for (; i < N; i++) {
	u ^= (gx[i] >> GROT1) 
	    ^ (gx[i] << GROT2);
	u ^= gx[jump];
	u ^= u << GS2;
	gx[i] ^=  u ^ (u << GS3);
	jump++;
    }
    gx[N] = u;

}

INLINE static void gen_rand_array(uint32_t array[], uint32_t blocks, 
				  uint32_t *lung){
    uint32_t u;
    unsigned int i;
  
    idx = 0;
    u = *lung;
    for (i = 0; i < N - GMM; i++) {
	u ^= (array[i] >> GROT1) 
	    ^ (array[i] << GROT2);
	u ^= array[i + GMM];
	u ^= u << GS2;
	array[i] ^=  u ^ (u << GS3);
    }
    for (; i < N; i++) {
	u ^= (array[i] >> GROT1) 
	    ^ (array[i] << GROT2);
	u ^= array[i + GMM - N];
	u ^= u << GS2;
	array[i] ^=  u ^ (u << GS3);
    }
    for (; i < N * blocks; i++) {
	u ^= (array[i - N] >> GROT1) 
	    ^ (array[i - N] << GROT2);
	u ^= array[i + GMM - N];
	u ^= u << GS2;
	array[i] ^=  u ^ (u << GS3);
    }
    *lung = u;
}

INLINE void fill_array_block(uint32_t array[], uint32_t block_num)
{
#if 0
    while (block_num > MAX_BLOCKS) {
	memcpy(array, gx, sizeof(gx));
	gen_rand_array(array, MAX_BLOCKS);
	memcpy(gx, &array[N * (MAX_BLOCKS - 1)], sizeof(gx));
	array += N * MAX_BLOCKS;
	block_num -= MAX_BLOCKS;
    }
#endif
    if (block_num == 0) {
	return;
    } else if (block_num == 1) {
	gen_rand_all();
	memcpy(array, gx, sizeof(gx) - sizeof(uint32_t));
    } else {
	memcpy(array, gx, sizeof(gx) - sizeof(uint32_t));
	gen_rand_array(array, block_num, &gx[N]);
	memcpy(gx, &array[N * (block_num-1)], sizeof(gx) - sizeof(uint32_t));
    }
}

INLINE uint32_t gen_rand()
{
    if (idx >= N) {
	gen_rand_all();
    }
    return gx[idx++];
}

INLINE void init_gen_rand(uint32_t seed)
{
    int i;
    gx[0] = seed;
    for (i = 1; i < NN; i++) {
	gx[i] = (1812433253UL * (gx[i - 1] ^ (gx[i - 1] >> 30)) + i); 
    }
    idx = N;
}
