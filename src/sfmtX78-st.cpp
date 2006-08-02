/* SFMT Search Code, M.Saito 2006/2/28 */
/* sfmt7x7 */
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>
#include "sfmt-st.h"

#define OUT_INITIAL 1
static inline void do_recursion(uint32_t a[4], uint32_t b[4], uint32_t c[4], 
				uint32_t d[4]);
static uint32_t gen_rand128sp(sfmt_t *sfmt, uint32_t arrary[4], uint32_t mode);
static void next_state(sfmt_t *sfmt);
static bool is_zero(sfmt_t *sfmt);
static void get_vector32(vec_GF2& vec, sfmt_t *sfmt, int state_mode,
			 int bit_len);
static void get_vector64(vec_GF2& vec, sfmt_t *sfmt, int state_mode,
			 int bit_len);
static void get_vector128(vec_GF2& vec, sfmt_t *sfmt, int state_mode,
			  int bit_len);

static unsigned int POS1 = 1;
static unsigned int SL1 = 11;
static unsigned int SL2 = 11;
static unsigned int SR1 = 7;
static unsigned int SR2 = 7;
static unsigned int MSK1 = 7;
static unsigned int MSK2 = 7;
static unsigned int MSK3 = 7;
static unsigned int MSK4 = 7;

unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

void print_version(FILE *fp) {
    fprintf(fp, "version:SFMT88q1\n");
}

void setup_param(unsigned int p1, unsigned int p2, unsigned int p3, 
		 unsigned int p4, unsigned int p5, unsigned int p6,
		 unsigned int p7, unsigned int p8, unsigned int p9,
		 unsigned int p10, unsigned int p11, unsigned int p12,
		 unsigned int p13) {
    POS1 = p1 % (N-3) + 1;
    SL1 = p2 % (32 - 1) + 1;
    SL2 = (p13 % 4) * 2 + 1; 
    SR1 = p3 % (32 - 1) + 1;
    SR2 = (p12 % 4) * 2 + 1; 
    MSK1 = p4 | p5 | p6;
    MSK2 = p6 | p7 | p8;
    MSK3 = p8 | p9 | p10;
    MSK4 = p10 | p11 | p12;
}

void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    fprintf(fp, "SL2 = %u\n", SL2);
    fprintf(fp, "SL3 = %u\n", SL3);
    fprintf(fp, "SR1 = %u\n", SR1);
    fprintf(fp, "MSK1 = %08x\n", MSK1);
    fprintf(fp, "MSK2 = %08x\n", MSK2);
    fprintf(fp, "MSK3 = %08x\n", MSK3);
    fprintf(fp, "MSK4 = %08x\n", MSK4);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SR1, SR2, MSK1, MSK2, MSK3, MSK4] = "
	    "[%u,%u,%u,%u,%u,%u,%u,%u,%u]\n", 
	    POS1, SL1, SL2, SR1, SR2, MSK1, MSK2, MSK3, MSK4);
    fflush(fp);
}

static inline void rshift128(uint32_t out[4], const uint32_t in[4],
			     int shift) {
    uint64_t t1, t2, o1, o2;

    t1 = ((uint64_t)in[1] << 32) | ((uint64_t)in[0]);
    t2 = ((uint64_t)in[3] << 32) | ((uint64_t)in[2]);

    o1 = t1 >> (shift * 8);
    o1 |= t2 << (64 - shift * 8);
    o2 = t2 >> (shift * 8);
    out[1] = (uint32_t)(o1 >> 32);
    out[0] = (uint32_t)o1;
    out[3] = (uint32_t)(o2 >> 32);
    out[2] = (uint32_t)o2;
}

static inline void lshift128(uint32_t out[4], const uint32_t in[4],
			     int shift) {
    uint64_t t1, t2, o1, o2;

    t1 = ((uint64_t)in[1] << 32) | ((uint64_t)in[0]);
    t2 = ((uint64_t)in[3] << 32) | ((uint64_t)in[2]);

    o1 = t1 << (shift * 8);
    o2 = t2 << (shift * 8);
    o2 |= t1 >> (64 - shift * 8);
    out[1] = (uint32_t)(o1 >> 32);
    out[0] = (uint32_t)o1;
    out[3] = (uint32_t)(o2 >> 32);
    out[2] = (uint32_t)o2;
}

static inline void do_recursion(uint32_t a[4], uint32_t b[4],
				uint32_t c[4], uint32_t d[4]) {
    uint32_t x[4];
    uint32_t y[4];

    lshift128(x, a, SL2);
    rshift128(y, c, SR2);
    a[0] = a[0] ^ x[0] ^ ((b[0] >> SR1) & MSK1) ^ y[0] ^ (d[0] << SL1);
    a[1] = a[1] ^ x[1] ^ ((b[1] >> SR1) & MSK2) ^ y[1] ^ (d[1] << SL1);
    a[2] = a[2] ^ x[2] ^ ((b[2] >> SR1) & MSK3) ^ y[2] ^ (d[2] << SL1);
    a[3] = a[3] ^ x[3] ^ ((b[3] >> SR1) & MSK4) ^ y[3] ^ (d[3] << SL1);
}

/*
 * これは直接呼び出さないでgenrandを呼び出している。
 */
static void next_state(sfmt_t *sfmt) {
    uint32_t i;

    //sfmt->idx += 4;
    if (sfmt->idx >= N * 4) {
	sfmt->idx = 0;
    }
    i = sfmt->idx / 4;
    do_recursion(sfmt->sfmt[i], sfmt->sfmt[(i + POS1) % N],
		 sfmt->sfmt[(i + N - 2) % N], sfmt->sfmt[(i + N - 1) % N]);
}

/*------------------------------------
 * Output initial state 
 ------------------------------------*/
#ifdef OUT_INITIAL
uint64_t gen_rand128(sfmt_t *sfmt, uint64_t *hi, uint64_t *low)
{
    uint32_t i;

    i = sfmt->idx / 4;
    *low = (uint64_t)sfmt->sfmt[i][0] | ((uint64_t)sfmt->sfmt[i][1] << 32);
    *hi = (uint64_t)sfmt->sfmt[i][2] | ((uint64_t)sfmt->sfmt[i][3] << 32);
    next_state(sfmt);
    sfmt->idx += 4;
    sfmt->idx = (sfmt->idx / 4) * 4;
    if (sfmt->idx >= N * 4) {
	sfmt->idx = 0;
    }
    return *hi;
}

uint64_t gen_rand64(sfmt_t *sfmt)
{
    uint64_t r;
    uint32_t i;

    assert(sfmt->idx % 2 == 0);
 
    i = sfmt->idx / 4;
    r = (uint64_t)sfmt->sfmt[i][sfmt->idx % 4] 
	| ((uint64_t)sfmt->sfmt[i][sfmt->idx % 4 + 1] << 32);
    if (sfmt->idx % 4 == 2) {
	next_state(sfmt);
    }
    sfmt->idx += 2;
    if (sfmt->idx >= N * 4) {
	sfmt->idx = 0;
    }

    return r;
}

uint32_t gen_rand32(sfmt_t *sfmt)
{
    uint32_t r;

    r = sfmt->sfmt[sfmt->idx / 4][sfmt->idx % 4];
    if (sfmt->idx % 4 == 3) {
	next_state(sfmt);
    }
    sfmt->idx++;
    if (sfmt->idx >= N * 4) {
	sfmt->idx = 0;
    }
    return r;
}

/*------------------------------------
 * Do not output initial state 
 ------------------------------------*/
#else  

uint64_t gen_rand128(sfmt_t *sfmt, uint64_t *hi, uint64_t *low)
{
    uint32_t i;

    assert(sfmt->idx % 4 == 0);

    next_state(sfmt);
    i = sfmt->idx / 4;
    *low = (uint64_t)sfmt->sfmt[i][0] | ((uint64_t)sfmt->sfmt[i][1] << 32);
    *hi = (uint64_t)sfmt->sfmt[i][2] | ((uint64_t)sfmt->sfmt[i][3] << 32);
    sfmt->idx += 4;
    if (sfmt->idx >= N * 4) {
	sfmt->idx = 0;
    }
    return *hi;
}

uint64_t gen_rand64(sfmt_t *sfmt)
{
    uint64_t r;
    uint32_t i;

    assert(sfmt->idx % 2 == 0);
 
    if (sfmt->idx % 4 == 0) {
	next_state(sfmt);
    }
    i = sfmt->idx / 4;
    r = (uint64_t)sfmt->sfmt[i][sfmt->idx % 4] 
	| ((uint64_t)sfmt->sfmt[i][sfmt->idx % 4 + 1] << 32);
    sfmt->idx += 2;
    if (sfmt->idx >= N * 4) {
	sfmt->idx = 0;
    }
    return r;
}

uint32_t gen_rand32(sfmt_t *sfmt)
{
    uint32_t r;

    if (sfmt->idx % 4 == 0) {
	next_state(sfmt);
    }
    r = sfmt->sfmt[sfmt->idx / 4][sfmt->idx % 4];
    sfmt->idx++;
    if (sfmt->idx >= N * 4) {
	sfmt->idx = 0;
    }
    return r;
}
#endif

/*------------------------------------
 * For distribution calculation
 ------------------------------------*/
/* これは初期状態を出力する */
static uint32_t gen_rand128sp(sfmt_t *sfmt, uint32_t array[4], uint32_t mode)
{
    uint32_t i, p;

    i = sfmt->idx / 4;
    p = sfmt->idx + 4;
    if (p >= N * 4) {
	p = 0;
    }
    p = p / 4;
    switch (mode) {
    case 0:
	array[0] = sfmt->sfmt[i][0];
	array[1] = sfmt->sfmt[i][1];
	array[2] = sfmt->sfmt[i][2];
	array[3] = sfmt->sfmt[i][3];
	break;
    case 1:
	array[0] = sfmt->sfmt[i][1];
	array[1] = sfmt->sfmt[i][2];
	array[2] = sfmt->sfmt[i][3];
	array[3] = sfmt->sfmt[p][0];
	break;
    case 2:
	array[0] = sfmt->sfmt[i][2];
	array[1] = sfmt->sfmt[i][3];
	array[2] = sfmt->sfmt[p][0];
	array[3] = sfmt->sfmt[p][1];
	break;
    case 3:
    default:
	array[0] = sfmt->sfmt[i][3];
	array[1] = sfmt->sfmt[p][0];
	array[2] = sfmt->sfmt[p][1];
	array[3] = sfmt->sfmt[p][2];
    }

    next_state(sfmt);
    sfmt->idx += 4;
    if (sfmt->idx >= N * 4) {
	sfmt->idx = 0;
    }
    return array[0];
}


void set_special(sfmt_t *sfmt, int special) {
    memset(sfmt, 0, sizeof(sfmt_t));
    sfmt->idx = 0;
    sfmt->special = true;
    sfmt->special_bit = special;
}

static bool is_zero(sfmt_t *sfmt) {
    int i, j;

    for (i = 0; i < N; i++) {
	for (j = 0; j < 4; j++) {
	    if (sfmt->sfmt[i][j] != 0) {
		return false;
	    }
	}
    }
    return true;
}

/*
 * state_mode: 0, 1, 2, 3
 */
static void get_vector32(vec_GF2& vec, sfmt_t *sfmt, int state_mode,
			 int bit_len) {
    uint32_t array[4];
    uint32_t mask;
    int i, j, k;

    vec.SetLength(0);
    vec.SetLength(bit_len);
    gen_rand128sp(sfmt, array, state_mode);
    k = 0;
    for (i = 0; i < 4; i++) {
	mask = (uint32_t)0x80000000UL;
	for (j = 0; j < bit_len / 4; j++) {
	    if (array[i] & mask) {
		vec.put(k, 1);
	    } else {
		vec.put(k, 0);
	    }
	    mask = mask >> 1;
	    k++;
	}
    }
#if 0
    cout << "get_vector32:";
    cout << vec << endl;
#endif
}

/*
 * state_mode : 0, 2
 */
static void get_vector64(vec_GF2& vec, sfmt_t *sfmt, int state_mode,
			 int bit_len) {
    uint32_t array[4];
    uint32_t mask;
    int i, j, k;

    assert(state_mode == 0 || state_mode == 2);

    vec.SetLength(0);
    vec.SetLength(bit_len);
    gen_rand128sp(sfmt, array, 0);
    k = 0;
    for (i = 0; (i < 2) && (k < bit_len / 2); i++) {
	mask = (uint32_t)0x80000000UL;
	for (j = 0; (j < 32) && (k < bit_len / 2); j++) {
	    if (array[i] & mask) {
		vec.put(k, 1);
	    } else {
		vec.put(k, 0);
	    }
	    mask = mask >> 1;
	    k++;
	}
    }
    for (i = 2; (i < 4) && (k < bit_len); i++) {
	mask = (uint32_t)0x80000000UL;
	for (j = 0; (j < 32) && (k < bit_len); j++) {
	    if (array[i] & mask) {
		vec.put(k, 1);
	    } else {
		vec.put(k, 0);
	    }
	    mask = mask >> 1;
	    k++;
	}
    }
}

/*
 * state_mode : 0
 */
static void get_vector128(vec_GF2& vec, sfmt_t *sfmt, int state_mode,
			  int bit_len) {
    uint32_t array[4];
    uint32_t mask;
    int i, j, k;

    assert(state_mode == 0);

    vec.SetLength(0);
    vec.SetLength(bit_len);
    gen_rand128sp(sfmt, array, 0);
    k = 0;
    for (i = 0; (i < 4) && (k < bit_len); i++) {
	mask = (uint32_t)0x80000000UL;
	for (j = 0; (j < 32) && (k < bit_len); j++) {
	    if (array[i] & mask) {
		vec.put(k, 1);
	    } else {
		vec.put(k, 0);
	    }
	    mask = mask >> 1;
	    k++;
	}
    }
}

int get_vector(vec_GF2& vec, sfmt_t *sfmt, int state_mode, int weight_mode,
	       int bit_len, int max_weight_mode) {
    sfmt_t tmp_sfmt;
    int i, count;
    vec_GF2 tmp_vec;
    vec_GF2 prev_vec;

    vec.SetLength(0);
    vec.SetLength(bit_len);
    if (sfmt->special) {
	if (sfmt->special_bit < bit_len * weight_mode / max_weight_mode) {
	    vec.put(sfmt->special_bit, 1);
	} else {
	    vec.put(sfmt->special_bit, 0);
	}
	return 0;
    }
    if (is_zero(sfmt)) {
	return INT_MAX;
    }
    tmp_sfmt = *sfmt;
    tmp_vec.SetLength(0);
    tmp_vec.SetLength(bit_len);
    for (count = 0; IsZero(vec); count++) {
	if (count > 2 * MEXP) {
	    return INT_MAX;
	}
	prev_vec = tmp_vec;
	switch (max_weight_mode) {
	case 1:
	    get_vector128(tmp_vec, &tmp_sfmt, state_mode, bit_len);
	    break;
	case 2:
	    get_vector64(tmp_vec, &tmp_sfmt, state_mode, bit_len);
	    break;
	case 4:
	default:
	    get_vector32(tmp_vec, &tmp_sfmt, state_mode, bit_len);
	    break;
	}
	/* calculate weighted norm */
	for (i = 0; i < (bit_len * weight_mode) / max_weight_mode ; i++) {
	    vec.put(i, tmp_vec.get(i));
	}
	for (; i < bit_len; i++) {
	    vec.put(i, prev_vec.get(i));
	}
    }
    return count;
}

void init_gen_rand(sfmt_t *sfmt, uint32_t seed)
{
    int i;

    sfmt->sfmt[0][0] = seed;
    for (i = 1; i < N * 4; i++) {
	sfmt->sfmt[i / 4][i % 4] = (uint32_t)1812433253UL 
	    * (sfmt->sfmt[(i - 1) / 4][(i - 1) % 4]
	       ^ (sfmt->sfmt[(i - 1) / 4][(i - 1) % 4] >> 30)) + i;
    }
    sfmt->idx = 0;
}

void add_rnd(sfmt_t *a, sfmt_t *b, int n) {
    int i, j;
    int ap;
    int cp;
    sfmt_t c;

    assert(a->idx % 4 == 0);
    assert(b->idx % 4 == 0);

    if (b->special) {
	return;
    }
    c = *b;
    for (i = 0; i < n; i++) {
	next_state(&c);
	c.idx += 4;
	if (c.idx >= N * 4) {
	    c.idx = 0;
	}
    }
    if (a->special) {
	*a = c;
	return;
    }
    ap = a->idx / 4;
    cp = c.idx / 4;
    for (i = 0; i < N; i++) {
	for (j = 0; j < 4; j++) {
	    a->sfmt[ap][j] ^= c.sfmt[cp][j];
	}
	ap = (ap + 1) % N;
	cp = (cp + 1) % N;
    }
}

uint32_t get_lung(sfmt_t *sfmt) {
    return sfmt->sfmt[0][3];
}

static unsigned int get_uint(char *line, int radix);
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
    MSK1 = get_uint(line, 16);
    fgets(line, 256, f);
    MSK2 = get_uint(line, 16);
    fgets(line, 256, f);
    MSK3 = get_uint(line, 16);
    fgets(line, 256, f);
    MSK4 = get_uint(line, 16);
}

