/* dSFMT Search Code, M.Saito 2006/9/14 */
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include "dsfmt-st.h"

#define LOW_MASK  ((uint64_t)0x000FFFFFFFFFFFFFULL)
//#define HIGH_CONST ((uint64_t)0x3FF0000000000000ULL)
//#define HIGH_CONST ((uint64_t)0x0000000000000ULL)

static unsigned int POS1 = 1;
static unsigned int SL1 = 11;
static unsigned int SL2 = 11;
static unsigned int SR1 = 7;
static unsigned int SR2 = 7;
static uint64_t MSK1 = 7;
static uint64_t MSK2 = 7;

static unsigned int get_uint(char *line, int radix);
static uint64_t get_uint64(char *line, int radix);
static uint64_t pcv[2];
static uint64_t HIGH_CONST = UINT64_C(0x3FF0000000000000);
static char idstring[512];

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

unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

char *get_idstring(void) {
    sprintf(idstring, "dSFMT-%d:%d-%d-%d-%d-%d:%016llx-%016llx",
	    MEXP, POS1, SL1, SL2, SR1, SR2, MSK1, MSK2);
    return idstring;
}

void set_high_const(void) {
    HIGH_CONST = UINT64_C(0x3FF0000000000000);
}
void reset_high_const(void) {
    HIGH_CONST = 0;
}

void get_fixed(uint64_t fix[2]) {
    fix[0] = (((HIGH_CONST >> SR1) & MSK2) ^ (HIGH_CONST >> SR2)) | HIGH_CONST;
    fix[1] = (((HIGH_CONST >> SR1) & MSK1) ^ (HIGH_CONST >> SR2)) | HIGH_CONST;
    fix[0] = fix[0] ^ (HIGH_CONST >> (64 - 8 * SL2));
 }

void set_pcv(uint64_t v[2]) {
    pcv[0] = v[0];
    pcv[1] = v[1];
}

void setup_param(uint32_t array[], int *index) {
    POS1 = array[(*index)++] % (N-2) + 1;
    SL1 = array[(*index)++] % (52 - 1) + 1;
    SL2 = (array[(*index)++] % 4) * 2 + 1; 
    SR1 = array[(*index)++] % (8 - 1) + 1;
    SR2 = (array[(*index)++] % (6 - 1) + 1) * 8;
    MSK1 = array[(*index)++];
    MSK1 |= array[(*index)++];
    MSK1 |= array[(*index)++];
    MSK1 <<= 32;
    MSK1 |= array[(*index)++];
    MSK1 |= array[(*index)++];
    MSK1 |= array[(*index)++];
    MSK2 = array[(*index)++];
    MSK2 |= array[(*index)++];
    MSK2 |= array[(*index)++];
    MSK2 <<= 32;
    MSK2 |= array[(*index)++];
    MSK2 |= array[(*index)++];
    MSK2 |= array[(*index)++];
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

inline static void rshift128(uint64_t out[2], const uint64_t in[2],
			     int shift) {
    out[1] = in[1] >> (shift * 8);
    out[0] = in[0] >> (shift * 8);
    out[0] |= in[1] << (64 - shift * 8);
}

inline static void lshift128(uint64_t out[2], const uint64_t in[2],
			     int shift) {
    out[1] = in[1] << (shift * 8);
    out[0] = in[0] << (shift * 8);
    out[1] |= in[0] >> (64 - shift * 8);
}

inline static void do_recursion(uint64_t a[2], uint64_t b[2],
				uint64_t c[2], uint64_t lung[2]) {
    uint64_t x[2];
    uint64_t r0, r1;

    lshift128(x, a, SL2);
    r0 = a[0] ^ x[0] ^ ((b[0] >> SR1) & MSK1) ^ (c[0] << SL1) ^ (c[0] >> SR2);
    r1 = a[1] ^ x[1] ^ ((b[1] >> SR1) & MSK2) ^ (c[1] << SL1) ^ (c[1] >> SR2);
    r0 = (r0 ^ lung[1]) & LOW_MASK;
    r1 = (r1 ^ lung[0]) & LOW_MASK;
    a[0] = r0 | HIGH_CONST;
    a[1] = r1 | HIGH_CONST;
    lung[0] ^= r0;
    lung[1] ^= r1;
}

/*
 * これは直接呼び出さないでgenrandを呼び出している。
 */
inline static void next_state(dsfmt_t *dsfmt) {
    uint32_t i;

    if (dsfmt->idx >= N * 2) {
	dsfmt->idx = 0;
    }
    i = dsfmt->idx / 2;
    do_recursion(dsfmt->status[i], dsfmt->status[(i + POS1) % N],
		 dsfmt->status[(i + N - 1) % N],
		 dsfmt->status[N]);
}

/* これは初期状態を出力する */
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
	array[0] = dsfmt->status[i][0] & LOW_MASK;
	array[1] = dsfmt->status[i][1] & LOW_MASK;
	break;
    case 1:
    default:
	array[0] = dsfmt->status[i][1] & LOW_MASK;
	array[1] = dsfmt->status[p][0] & LOW_MASK;
    }

    next_state(dsfmt);
    dsfmt->idx += 2;
    if (dsfmt->idx >= N * 2) {
	dsfmt->idx = 0;
    }
    return array[0];
}

double genrand_close1_open2(dsfmt_t *dsfmt) {
    double r;
    double *psfmt64;

    if ((dsfmt->idx >= N * 2) || (dsfmt->idx % 2 == 0)) {
	next_state(dsfmt);
    }
    psfmt64 = (double *)&dsfmt->status[0][0];
    r = psfmt64[dsfmt->idx++];
    return r;
}

void gen_rand104spar(dsfmt_t *dsfmt, uint64_t array[][2], int size) {
    uint32_t i;
    int j;

    if (dsfmt->idx >= N * 2) {
	dsfmt->idx = 0;
    }
    for (j = 0; j < size; j++) {
	i = dsfmt->idx / 2;
	array[j][0] = dsfmt->status[i][0] & LOW_MASK;
	array[j][1] = dsfmt->status[i][1] & LOW_MASK;

	next_state(dsfmt);
	dsfmt->idx += 2;
	if (dsfmt->idx >= N * 2) {
	    dsfmt->idx = 0;
	}
    }
}

#if 0
void init_gen_rand(dsfmt_t *dsfmt, uint64_t seed)
{
    int i;
    uint64_t *psfmt;

    psfmt = dsfmt->status[0];
    psfmt[0] = (seed & LOW_MASK) | HIGH_CONST;
    for (i = 1; i <= N * 2; i++) {
	psfmt[i] = 6364136223846793005ULL 
	    * (psfmt[i - 1] ^ (psfmt[i - 1] >> 62)) + i;
	psfmt[i] = (psfmt[i] & LOW_MASK) | HIGH_CONST;
    }
    dsfmt->idx = 0;
}
#else
/**
 * This function initializes the internal state array with a 32-bit
 * integer seed.
 * @param seed a 32-bit integer used as the seed.
 */
void init_gen_rand(dsfmt_t *dsfmt, uint32_t seed) {
    int i;
    uint32_t *psfmt;

    memset(dsfmt, 0, sizeof(dsfmt_t));
    psfmt = (uint32_t *)&dsfmt->status[0][0];
    psfmt[idxof(0)] = seed;
    for (i = 1; i < (N + 1) * 4; i++) {
	psfmt[idxof(i)] = 1812433253UL * (psfmt[idxof(i - 1)] 
					  ^ (psfmt[idxof(i - 1)] >> 30)) + i;
    }
    initial_mask(dsfmt);
    period_certification(dsfmt);
    dsfmt->idx = N * 2;
}
/**
 * This function initializes the internal state array to fit the IEEE
 * 754 format.
 */
void initial_mask(dsfmt_t *dsfmt) {
    int i;
    uint64_t *psfmt;

    psfmt = &dsfmt->status[0][0];
    for (i = 0; i < (N + 1) * 2; i++) {
        psfmt[i] = (psfmt[i] & LOW_MASK) | HIGH_CONST;
    }
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

    get_fixed(fix);
    //printf("fix lung %016"PRIx64"\n", fix[0]);
    //printf("fix lung %016"PRIx64"\n", fix[1]);
    //printf("old lung %016"PRIx64"\n", dsfmt->sfmt[N].u[0]);
    //printf("old lung %016"PRIx64"\n", dsfmt->sfmt[N].u[1]);
    new[0] = dsfmt->status[N][0] ^ fix[0];
    new[1] = dsfmt->status[N][1] ^ fix[1];
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
	work = 1;
	for (j = 0; j < 52 - SR1; j++) {
	    if ((work & pcv[i]) != 0) {
		dsfmt->status[N][i] ^= work;
		//printf("mod lung %016"PRIx64"\n", dsfmt->sfmt[N].u[0]);
		//printf("mod lung %016"PRIx64"\n", dsfmt->sfmt[N].u[1]);
		return 0;
	    }
	    work = work << 1;
	}
    }
    return 0;
}
#endif
void add_rnd(dsfmt_t *dist, dsfmt_t *src) {
    int i, k;

    assert(dist->idx % 2 == 0);
    assert(src->idx % 2 == 0);
    
    k = (src->idx / 2 - dist->idx / 2 + N) % N;
    for (i = 0; i < N; i++) {
	dist->status[i][0] ^= src->status[(k + i) % N][0];
	dist->status[i][1] ^= src->status[(k + i) % N][1];
    }
    dist->status[N][0] ^= src->status[N][0];
    dist->status[N][1] ^= src->status[N][1];
}

void get_lung(dsfmt_t *dsfmt, uint64_t lung[2]) {
    lung[0] = dsfmt->status[N][0];
    lung[1] = dsfmt->status[N][1];
}

static unsigned int get_uint(char *line, int radix) {
    unsigned int result;

    for (;(*line) && (*line != '=');line++);
    if (!*line) {
	fprintf(stderr, "WARN:can't get = in get_uint\n");
	return 0;
    }
    line++;
    errno = 0;
    result = (unsigned int)strtoll(line, NULL, radix);
    if (errno) {
	fprintf(stderr, "WARN:format error:%s", line);
	perror("get_unit");
    }
    return result;
}

static uint64_t get_uint64(char *line, int radix) {
    int i;
    unsigned int x;
    uint64_t result;

    assert(radix == 16);

    for (;(*line) && (*line != '=');line++);
    if (!*line) {
	fprintf(stderr, "WARN:can't get = in get_uint\n");
	return 0;
    }
    line++;
    for (;(*line) && (*line <= ' ');line++);
    result = 0;
    for (i = 0;(*line) && (i < 16);i++,line++) {
	x = *line;
	if (('0' <= x) && (x <= '9')) {
	    x = x - '0';
	} else if (('A' <= x) && (x <= 'F')) {
	    x = x + 10 - 'A';
	} else if (('a' <= x) && (x <= 'f')) {
	    x = x + 10 - 'a';
	} else {
	    printf("format error %s\n", line);
	    return 0;
	}
	result = result * 16 + x;
    }
    return result;
}

void read_random_param(FILE *f) {
    char line[256];

    fgets(line, 256, f);
    fgets(line, 256, f);
    fgets(line, 256, f);
    POS1 = get_uint(line, 10);
    fgets(line, 256, f);
    SL1 = get_uint(line, 10);
    fgets(line, 256, f);
    SL2 = get_uint(line, 10);
    fgets(line, 256, f);
    SR1 = get_uint(line, 10);
    fgets(line, 256, f);
    SR2 = get_uint(line, 10);
    fgets(line, 256, f);
    MSK1 = get_uint64(line, 16);
    fgets(line, 256, f);
    MSK2 = get_uint64(line, 16);
}

