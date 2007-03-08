/** 
 * @file dSFMT-ref.c 
 * @brief double precision SIMD-oriented Fast Mersenne Twister (dSFMT)
 * based on IEEE 754 format.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <inttypes.h>

#include "dsfmt-ref-st.h"
#if MEXP == 607
  #include "dSFMT-params607.h"
#elif MEXP == 1279
  #include "dSFMT-params1279.h"
#elif MEXP == 2281
  #include "dSFMT-params2281.h"
#elif MEXP == 4423
  #include "dSFMT-params4423.h"
#elif MEXP == 11213
  #include "dSFMT-params11213.h"
#elif MEXP == 19937
  #include "dSFMT-params19937.h"
#elif MEXP == 44497
  #include "dSFMT-params44497.h"
#elif MEXP == 86243
  #include "dSFMT-params86243.h"
#elif MEXP == 132049
  #include "dSFMT-params132049.h"
#else
#ifdef __GNUC__
  #error "MEXP is not valid."
  #undef MEXP
#else
  #undef MEXP
#endif
#endif

#undef HIGH_CONST
#undef HIGH_CONST32
static uint64_t HIGH_CONST = UINT64_C(0x3FF0000000000000);
static uint64_t HIGH_CONST32 = 0x3ff00000U;

#if defined(__ppc__)
/**
 * This function simulate a 64-bit index of LITTLE ENDIAN 
 * in BIG ENDIAN machine.
 */
inline static int idxof(int i) {
    return i ^ 1;
}
#else
inline static int idxof(int i) {
    return i;
}
#endif

double genrand_close1_open2(dsfmt_t *dsfmt);

int get_min_array_size(void);

/*--------------------------------------
  FILE GLOBAL VARIABLES
  internal state, index counter and flag 
  --------------------------------------*/
/** a period certification vector which certificate the period of 2^{MEXP}-1. */
static uint64_t pcv[2] = {PCV1, PCV2};

void set_high_const(void) {
    HIGH_CONST = UINT64_C(0x3FF0000000000000);
    HIGH_CONST32 = 0x3ff00000U;
}
void reset_high_const(void) {
    HIGH_CONST = 0;
    HIGH_CONST32 = 0;
}

unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    fprintf(fp, "SL2 = %u\n", SL2);
    fprintf(fp, "SR1 = %u\n", SR1);
    fprintf(fp, "SR2 = %u\n", SR2);
    fprintf(fp, "MSK1 = %016"PRIx64"\n", MSK1);
    fprintf(fp, "MSK2 = %016"PRIx64"\n", MSK2);
    fflush(fp);
}

/*----------------
  STATIC FUNCTIONS
  ----------------*/
static void lshift128(w128_t *out, const w128_t *in, int shift);
static void gen_rand_all(dsfmt_t *dsfmt);
//static void initial_mask(dsfmt_t *dsfmt);

/**
 * This function simulates SIMD 128-bit left shift by the standard C.
 * The 128-bit integer given in \b in is shifted by (shift * 8) bits.
 * This function simulates the LITTLE ENDIAN SIMD.
 * @param out the output of this function
 * @param in the 128-bit data to be shifted
 * @param shift the shift value
 */
inline static void lshift128(w128_t *out, const w128_t *in, int shift) {
    out->u[0] = in->u[0] << (shift * 8);
    out->u[1] = in->u[1] << (shift * 8);
    out->u[1] |= in->u[0] >> (64 - shift * 8);
}

/**
 * This function represents the recursion formula.
 * @param r output
 * @param a a 128-bit part of the internal state array
 * @param b a 128-bit part of the internal state array
 * @param c a 128-bit part of the internal state array
 * @param lung a 128-bit part of the internal state array
 */
inline static void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *c,
				w128_t *lung) {
    w128_t x;
    uint64_t r0, r1;

    lshift128(&x, a, SL2);
    r0 = a->u[0] ^ x.u[0] ^ ((b->u[0] >> SR1) & MSK1) ^ (c->u[0] << SL1)
	^ (c->u[0] >> SR2) ^ lung->u[1];
    r1 = a->u[1] ^ x.u[1] ^ ((b->u[1] >> SR1) & MSK2) ^ (c->u[1] << SL1)
	^ (c->u[1] >> SR2) ^ lung->u[0];
    r0 = r0 & LOW_MASK;
    r1 = r1 & LOW_MASK;
    r->u[0] = r0 | HIGH_CONST;
    r->u[1] = r1 | HIGH_CONST;
    lung->u[0] ^= r0;
    lung->u[1] ^= r1;
}

/**
 * This function fills the internal state array with double precision
 * floating point pseudorandom numbers of the IEEE 754 format.
 */
inline static void gen_rand_all(dsfmt_t *dsfmt) {
    int i;
    w128_t lung;

    lung = dsfmt->sfmt[N];
    do_recursion(&dsfmt->sfmt[0], &dsfmt->sfmt[0], &dsfmt->sfmt[POS1],
		 &dsfmt->sfmt[N -1], &lung);
    for (i = 1; i < N - POS1; i++) {
	do_recursion(&dsfmt->sfmt[i], &dsfmt->sfmt[i], &dsfmt->sfmt[i + POS1],
		     &dsfmt->sfmt[i - 1], &lung);
    }
    for (; i < N; i++) {
	do_recursion(&dsfmt->sfmt[i], &dsfmt->sfmt[i],
		     &dsfmt->sfmt[i + POS1 - N], &dsfmt->sfmt[i - 1], &lung);
    }
    dsfmt->sfmt[N] = lung;
}

/**
 * This function initializes the internal state array to fit the IEEE
 * 754 format.
 */
void initial_mask(dsfmt_t *dsfmt) {
    int i;
    uint64_t *psfmt;

    psfmt = &dsfmt->sfmt[0].u[0];
    for (i = 0; i < (N + 1) * 2; i++) {
        psfmt[i] = (psfmt[i] & LOW_MASK) | HIGH_CONST;
    }
}

void get_fixed(uint64_t fix[2]) {
    fix[0] = (((HIGH_CONST >> SR1) & MSK2) ^ (HIGH_CONST >> SR2)) | HIGH_CONST;
    fix[1] = (((HIGH_CONST >> SR1) & MSK1) ^ (HIGH_CONST >> SR2)) | HIGH_CONST;
    fix[0] = fix[0] ^ (HIGH_CONST >> (64 - 8 * SL2));
 }

/**
 * This function certificate the period of 2^{MEXP}-1.
 */
int period_certification(dsfmt_t *dsfmt) {
    int inner = 0;
    int i, j;
    uint64_t new[2];
    uint64_t work;
    uint64_t fix[2];

    fix[0] = (((HIGH_CONST >> SR1) & MSK2) ^ (HIGH_CONST >> SR2)) | HIGH_CONST;
    fix[1] = (((HIGH_CONST >> SR1) & MSK1) ^ (HIGH_CONST >> SR2)) | HIGH_CONST;
    fix[0] = fix[0] ^ (HIGH_CONST >> (64 - 8 * SL2));
    //printf("fix lung %016"PRIx64"\n", fix[0]);
    //printf("fix lung %016"PRIx64"\n", fix[1]);
    //printf("old lung %016"PRIx64"\n", dsfmt->sfmt[N].u[0]);
    //printf("old lung %016"PRIx64"\n", dsfmt->sfmt[N].u[1]);
    new[0] = dsfmt->sfmt[N].u[0] ^ fix[0];
    new[1] = dsfmt->sfmt[N].u[1] ^ fix[1];
    //printf("new lung %016"PRIx64"\n", new[0]);
    //printf("new lung %016"PRIx64"\n", new[1]);
    for (i = 0; i < 2; i++) {
	work = new[i] & pcv[i];
	for (j = 0; j < 52; j++) {
	    inner ^= work & 1;
	    work = work >> 1;
	}
    }
    /* check OK */
    if (inner == 1) {
	return 1;
    }
    /* check NG, and modification */
    for (i = 0; i < 2; i++) {
	work = (uint64_t)1 << SR1;
	for (j = 0; j < 52 - SR1; j++) {
	    if ((work & pcv[i]) != 0) {
		dsfmt->sfmt[N].u[i] ^= work;
		//printf("mod lung %016"PRIx64"\n", dsfmt->sfmt[N].u[0]);
		//printf("mod lung %016"PRIx64"\n", dsfmt->sfmt[N].u[1]);
		return 0;
	    }
	    work = work << 1;
	}
    }
    return 0;
}
/*----------------
  PUBLIC FUNCTIONS
  ----------------*/
/**
 * This function returns the identification string.  The string shows
 * the Mersenne exponent, and all parameters of this generator.
 * @return id string.
 */
char *get_idstring(void) {
    return IDSTR;
}

/**
 * This function returns the minimum size of array used for \b
 * fill_array functions.
 * @return minimum size of array used for fill_array functions.
 */
int get_min_array_size(void) {
    return N64;
}

/**
 * This function generates and returns double precision pseudorandom
 * number which distributes uniformly in the range [1, 2).
 * init_gen_rand() or init_by_array() must be called before this
 * function.
 * @return double precision floating point pseudorandom number
 */
inline double genrand_close1_open2(dsfmt_t *dsfmt) {
    double r;
    double *psfmt64;

    assert(dsfmt->initialized);

    psfmt64 = &dsfmt->sfmt[0].d[0];
    if (dsfmt->idx >= N * 2) {
	gen_rand_all(dsfmt);
	dsfmt->idx = 0;
    }
    r = psfmt64[dsfmt->idx++];
    return r;
}

/**
 * This function initializes the internal state array with a 32-bit
 * integer seed.
 * @param seed a 32-bit integer used as the seed.
 */
void init_gen_rand(dsfmt_t *dsfmt, uint32_t seed) {
    int i;
    uint32_t *psfmt;

    psfmt = (uint32_t *)&dsfmt->sfmt[0].u[0];
    psfmt[idxof(0)] = seed;
    for (i = 1; i < (N + 1) * 4; i++) {
	psfmt[idxof(i)] = 1812433253UL * (psfmt[idxof(i - 1)] 
					  ^ (psfmt[idxof(i - 1)] >> 30)) + i;
    }
    initial_mask(dsfmt);
    period_certification(dsfmt);
    dsfmt->idx = N64;
    dsfmt->initialized = 1;
}

void add_rnd(dsfmt_t *dist, dsfmt_t *src) {
    int i, k;

    assert(dist->idx % 2 == 0);
    assert(src->idx % 2 == 0);
    
    k = (src->idx / 2 - dist->idx / 2 + N) % N;
    for (i = 0; i < N; i++) {
	dist->sfmt[i].u[0] ^= src->sfmt[(k + i) % N].u[0];
	dist->sfmt[i].u[1] ^= src->sfmt[(k + i) % N].u[1];
    }
    dist->sfmt[N].u[0] ^= src->sfmt[N].u[0];
    dist->sfmt[N].u[1] ^= src->sfmt[N].u[1];
}

static void next_state(dsfmt_t *dsfmt) {
    uint32_t i;

    if (dsfmt->idx >= N * 2) {
	dsfmt->idx = 0;
    }
    i = dsfmt->idx / 2;
    do_recursion(&dsfmt->sfmt[i], &dsfmt->sfmt[i], &dsfmt->sfmt[(i + POS1) % N],
		 &dsfmt->sfmt[(i + N - 1) % N], &dsfmt->sfmt[N]);
}

uint64_t gen_rand104sp(dsfmt_t *dsfmt, uint64_t array[2], int mode)
{
    uint32_t i, p;

    if (dsfmt->idx >= N * 2) {
	dsfmt->idx = 0;
    }
    i = dsfmt->idx / 2;
    p = dsfmt->idx + 2;
    if (p >= N * 2) {
	p = 0;
    }
    p = p / 2;
    switch (mode) {
    case 0:
	array[0] = dsfmt->sfmt[i].u[0] & LOW_MASK;
	array[1] = dsfmt->sfmt[i].u[1] & LOW_MASK;
	break;
    case 1:
    default:
	array[0] = dsfmt->sfmt[i].u[1] & LOW_MASK;
	array[1] = dsfmt->sfmt[p].u[0] & LOW_MASK;
    }

    next_state(dsfmt);
    dsfmt->idx += 2;
    if (dsfmt->idx >= N * 2) {
	dsfmt->idx = 0;
    }
    return array[0];
}

#ifdef MAIN
int main(void) {
    int i;
    double d;
    dsfmt_t dsfmt;

    printf("%s\n", get_idstring());
    printf("generated randoms [1, 2)\n");
    init_gen_rand(&dsfmt, 1234);
    for (i = 0; i < 1000; i++) {
	d = genrand_close1_open2(&dsfmt);
	printf("%1.20lf ", d);
	if (i % 3 == 2) {
	    printf("\n");
	}
    }
    printf("\n");
    return 0;
}
#endif
