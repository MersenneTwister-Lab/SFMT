/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "sfmt-st.h"

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 128
#define N (MEXP / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N)

class SFMT {
private:
    static uint32_t POS1;
    static uint32_t SL1;
    static uint32_t SL2;
    static uint32_t SR1;
    uint32_t sfmt[N][4];
    uint32_t idx;
    void next_state(void);
public:
    static unsigned int get_rnd_maxdegree(void);
    static unsigned int get_rnd_mexp(void);
    static void setup_param(unsigned int p1, unsigned int p2, unsigned int p3, 
			    unsigned int p4, unsigned int p5, unsigned int p6);
    static ostream& operator<<(ostream& os);
    static void print_param2(FILE *fp);
    uint32_t gen_rand(void) {
	uint32_t r;
	
	if (idx >= N * 4) {
	    gen_rand_all();
	    idx = 0;
	}
	r = sfmt[idx / 4][idx % 4];
	idx++;
	return r;
    }
    void add(SFMT& src) {
	int i;
	for (i = 0; i < N; i++) {
	    sfmt[i] ^= src.sfmt[(N + i + src.idx - idx) % N];
	}
    }
    istream& operator>>(istream& is, const SFMT& sfmt) {
	string line;
	while (is.good()) {
	    is >> line;
	    if 
    }
    SFMT(uint32_t seed) {
	int i;
	
	sfmt[0][0] = seed;
	for (i = 1; i < N * 4; i++) {
	    sfmt[i/4][i%4] = 1812433253UL 
		* (sfmt[(i - 1) / 4][(i - 1) % 4]
		   ^ (sfmt[(i - 1) / 4][(i - 1) % 4] >> 30)) 
		+ i;
	}
	idx = N * 4;
    }
};

uint32_t SFMT::POS1 = 1;
uint32_t SFMT::SL1 = 11;
uint32_t SFMT::SL2 = 7;
uint32_t SFMT::SR1 = 17;

static unsigned int get_rnd_maxdegree(void) {
    return MAXDEGREE;
}
static unsigned int get_rnd_mexp(void) {
    return MEXP;
}
static void setup_param(unsigned int p1, unsigned int p2, unsigned int p3, 
			unsigned int p4, unsigned int p5, unsigned int p6) {
    POS1 = p1 % (N-2) + 1;
    SL1 = p2 % (32 - 1) + 1;
    SL2 = p3 % (32 - 1) + 1;
    SR1 = p4 % (32 - 1) + 1;
}
    ostream& operator<<(ostream& os, SFMT& sfmt) {
	fprintf(fp, "POS1 = %u\n", POS1);
	fprintf(fp, "SL1 = %u\n", SL1);
	fprintf(fp, "SL2 = %u\n", SL2);
	fprintf(fp, "SR1 = %u\n", SR1);
	fflush(fp);
    }
static void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SR1] = [%u,%u,%u,%u]\n", 
	    POS1, SL1, SL2, SR1);
    fflush(fp);
}

void SFMT::next_state(void) {
    if (++idx >= N) {
	idx = 0;
    }
    sfmt[idx][0] = (sfmt[idx][0] << SL1) ^ sfmt[idx][0]
	^ (sfmt[(idx + POS1) % N][0] >> SR1) ^ sfmt[(idx + POS1) % N][1]
	^ (sfmt[(idx + N - 1) % N][0] << SL2); 
    sfmt[idx][1] = (sfmt[idx][1] << SL1) ^ sfmt[idx][1]
	^ (sfmt[(idx + POS1) % N][1] >> SR1) ^ sfmt[(idx + POS1) % N][2]
	^ (sfmt[(idx + N - 1) % N][1] << SL2) ^ sfmt[(idx + N -1) % N][0];
    sfmt[idx][2] = (sfmt[idx][2] << SL1) ^ sfmt[idx][2]
	^ (sfmt[(idx + POS1) % N][2] >> SR1) ^ sfmt[(idx + POS1) % N][3]
	^ (sfmt[(idx + N - 1) % N][2] << SL2) ^ sfmt[(idx + N -1) % N][1];
    sfmt[idx][3] = (sfmt[idx][3] << SL1) ^ sfmt[idx][3]
	^ (sfmt[(idx + POS1) % N][3] >> SR1)
	^ (sfmt[(idx + N - 1) % N][3] << SL2) ^ sfmt[(idx + N -1) % N][2];
}


