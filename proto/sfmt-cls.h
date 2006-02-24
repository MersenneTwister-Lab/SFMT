/* Simple and Fast MT 2006/1/21 */
#ifndef __SFMT_H__
#define __SFMT_H__

#include <stdint.h>
#include <stdio.h>
#include <NTL/vec_GF2.h>

NTL_CLIENT;

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 128
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

class SFMT {
public:
    enum MODE_E {BIT32, BIT64, BIT128};
private:
    enum MODE_E mode;
    uint32_t sfmt[N][4];
    unsigned int idx;
    void init_gen_rand(uint32_t seed);
public:
    void next_state32(void);
    void next_state64(void);
    void next_state128(void);
    void next_state(void);
    void add(const SFMT& src);
    vec_GF2& gen_rand(vec_GF2& vec, uint32_t len);
    uint32_t gen_rand32(void) {
	next_state32();
	return sfmt[idx / 4][idx % 4];
    }
    friend ostream& operator<<(ostream& os, const SFMT& sfmt);
    void reseed(uint32_t seed) {
	memset(sfmt, 0, sizeof(sfmt));
	idx = 0;
	init_gen_rand(seed);
	mode = BIT32;
    }
    void set_mode(MODE_E p_mode) {
	if (idx % 4 == 0) {
	    mode = p_mode;
	} else {
	    printf("ERROR: can't change mode\n");
	    exit(1);
	}
    }
    SFMT(void) {
	memset(sfmt, 0, sizeof(sfmt));
	idx = 0;
	mode = BIT32;
    }
    SFMT(uint32_t seed){
	memset(sfmt, 0, sizeof(sfmt));
	idx = 0;
	init_gen_rand(seed);
	mode = BIT32;
    }
};

void setup_param(unsigned int p1, unsigned int p2, unsigned int p3, 
		 unsigned int p4, unsigned int p5, unsigned int p6,
		 unsigned int p7, unsigned int p8, unsigned int p9,
		 unsigned int p10, unsigned int p11, unsigned int p12,
		 unsigned int p13);
unsigned int get_rnd_maxdegree(void);
unsigned int get_rnd_mexp(void);
void print_param(FILE *fp);
void print_param2(FILE *fp);
void read_random_param(FILE *f);

#endif
