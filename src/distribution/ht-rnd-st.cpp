/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ht-st.h"
#include <ntl/vec_GF2.h>

NTL_CLIENT;

static void private_next_state1(ht_rand *rand);
static void private_next_state(ht_rand *rand);
static unsigned int gmm;
static unsigned int gs0;
static unsigned int gs2;
static unsigned int gs3;
static unsigned int grot1;
static unsigned int grot2;

unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

unsigned int get_rnd_nn(void) {
    return NN;
}

void setup_param(unsigned int p_gmm, unsigned int p_gs0, unsigned int p_gs2, 
		 unsigned int p_gs3, unsigned int p_grot1, 
		 unsigned int p_grot2, unsigned int dmy1, unsigned int dmy2,
		 unsigned int dmy3) {
    gmm = p_gmm % (N - 1) + 1;
    gs0 = p_gs0 % (32 - 1) + 1;
    gs2 = p_gs2 % (32 - 1) + 1;
    gs3 = p_gs3 % (32 - 1) + 1;
    grot1 = p_grot1 % (32 - 1) + 1;
    grot2 = p_grot2 % (32 - 1) + 1;
}

void print_param(FILE *fp) {
    fprintf(fp, "gmm = %u\n", gmm);
    fprintf(fp, "gs2 = %u\n", gs2);
    fprintf(fp, "gs3 = %u\n", gs3);
    fprintf(fp, "grot1 = %u\n", grot1);
    fprintf(fp, "grot2 = %u\n", grot2);
    fflush(fp);
}

void print_param2(FILE *fp) {
    fprintf(fp, "[GMM, GS2, GS3, GROT1, GROT2] = "
            "[%u,%u,%u,%u,%u]＼n", 
            gmm, gs2, gs3, grot1, grot2);
    fflush(fp);
}

static void private_next_state1(ht_rand *rand) {
    uint32_t u;

    u = rand->gx[N];
    u ^= (rand->gx[rand->index % N] >> grot1) 
	^ (rand->gx[rand->index % N] << grot2);
    u ^= rand->gx[(rand->index + gmm) % N];
    u ^= u << gs2;
    rand->gx[rand->index % N] ^=  u ^ (u << gs3);
    rand->gx[N] = u;
}

static void private_next_state(ht_rand *rand) {
    uint32_t u;
    uint32_t i;

    u = rand->gx[N];
    for (i = rand->index; i < rand->index + 4; i++) {
	u ^= (rand->gx[i % N] >> grot1) 
	    ^ (rand->gx[i % N] << grot2);
	u ^= rand->gx[(i + gmm) % N];
	u ^= u << gs2;
	rand->gx[i % N] ^=  u ^ (u << gs3);
    }
    rand->gx[N] = u;
}

/* これは初期状態を出力する */
uint32_t private_gen_rand128sp(ht_rand *rand, uint32_t array[4], uint32_t mode)
{
    uint32_t i;

    i = rand->index + mode;
    array[0] = rand->gx[i];
    array[1] = rand->gx[(i + 1) % N];
    array[2] = rand->gx[(i + 2) % N];
    array[3] = rand->gx[(i + 3) % N];

    private_next_state(rand);
    rand->index = (rand->index + 4) % N;
    return array[0];
}

void set_special(ht_rand *rand, int special) {
    memset(rand, 0, sizeof(ht_rand));
    rand->index = 0;
    rand->special = true;
    rand->special_bit = special;
}

bool is_zero(ht_rand *rand) {
    int i;

    for (i = 0; i < NN; i++) {
	if (rand->gx[i] != 0) {
	    return false;
	}
    }
    return true;
}

int get_vector32(vec_GF2& vec, ht_rand *rand, int state_mode, int weight_mode,
		int bit_len) {
    uint32_t array[4];
    uint32_t mask;
    ht_rand tmp_rand;
    int i, j, count;
    vec_GF2 tmp_vec;
    vec_GF2 prev_vec;

    vec.SetLength(0);
    vec.SetLength(bit_len);
    if (rand->special) {
	if (rand->special_bit < bit_len * weight_mode / 4) {
	    vec.put(rand->special_bit, 1);
	}
	return 0;
    }
    if (is_zero(rand)) {
	return INT_MAX;
    }
    tmp_rand = *rand;
    tmp_vec.SetLength(0);
    tmp_vec.SetLength(bit_len);
    for (count = 0; IsZero(vec); count++) {
	if (count > 2 * MEXP) {
	    return INT_MAX;
	}
	private_gen_rand128sp(&tmp_rand, array, state_mode);
	prev_vec = tmp_vec;
	tmp_vec.SetLength(0);
	tmp_vec.SetLength(bit_len);
	for (i = 0; i < 4; i++) {
	    mask = 0x80000000UL;
	    for (j = 0; j < bit_len; j += 4) {
		if (array[i] & mask) {
		    tmp_vec.put(i + j, 1);
		} else {
		    tmp_vec.put(i + j, 0);
		}
		mask = mask >> 1;
	    }
	}
	/* weight 付きノルムの計算 */
	for (i = 0; i < bit_len * weight_mode / 4 ; i++) {
	    vec.put(i, tmp_vec.get(i));
	}
	for (; i < bit_len; i++) {
	    vec.put(i, prev_vec.get(i));
	}
    }
    return count;
}

int get_vector32(vec_GF2& vec, ht_rand *rand, int bit_len) {
    uint32_t mask;
    uint32_t r;
    ht_rand tmp_rand;
    int i, count;

    vec.SetLength(0);
    vec.SetLength(bit_len);
    if (rand->special) {
	vec.put(rand->special_bit, 1);
	return 0;
    }
    if (is_zero(rand)) {
	return INT_MAX;
    }
    tmp_rand = *rand;
    for (count = 0; IsZero(vec); count++) {
	if (count > 2 * MEXP) {
	    return INT_MAX;
	}
	r = gen_rand32(&tmp_rand);
	mask = 0x80000000UL;
	for (i = 0; i < bit_len; i++) {
	    if (r & mask) {
		vec.put(i, 1);
	    } else {
		vec.put(i, 0);
	    }
	    mask = mask >> 1;
	}
    }
    return count;
}

uint32_t gen_rand32(ht_rand *rand)
{
    uint32_t r;

    r = rand->gx[rand->index];
    private_next_state1(rand);
    rand->index = (rand->index + 1) % N;
    return r;
}

void init_gen_rand(ht_rand *rand, int seed)
{
    int i;
    for (i=0; i < NN; i++) {
	rand->gx[i] = (uint32_t)(seed + i);
	seed = seed * 31415UL;
    }
    rand->index = 0;
    rand->special_bit = 0;
    rand->special = false;
#if 0
    for (i = 0; i < N; i++) {
	private_next_state1(rand);
    }
#endif
}

void add_rnd(ht_rand *a, ht_rand *b, int n) {
    int i;
    int ap;
    int cp;
    ht_rand c;

    if (b->special) {
	return;
    }
    c = *b;
    for (i = 0; i < n; i++) {
	private_next_state(&c);
	c.index = (c.index + 4) % N;
    }
    if (a->special) {
	*a = c;
	return;
    }
    ap = a->index;
    cp = c.index;
    for (i = 0; i < N; i++) {
	a->gx[ap++] ^= c.gx[cp++];
	ap %= N;
	cp %= N;
    }
    a->gx[N] ^= c.gx[N];
}

void add_rnd1(ht_rand *a, ht_rand *b, int n) {
    int i;
    int ap;
    int cp;
    ht_rand c;

    if (b->special) {
	return;
    }
    c = *b;
    for (i = 0; i < n; i++) {
	private_next_state1(&c);
	c.index = (c.index + 1) % N;
    }
    if (a->special) {
	*a = c;
	return;
    }
    ap = a->index;
    cp = c.index;
    for (i = 0; i < N; i++) {
	a->gx[ap++] ^= c.gx[cp++];
	ap %= N;
	cp %= N;
    }
    a->gx[N] ^= c.gx[N];
}

unsigned int get_uint(char *line);
unsigned int get_uint(char *line) {
    unsigned int result;

    for (;(*line) && (*line != '=');line++);
    if (!*line) {
	fprintf(stderr, "WARN:can't get = in get_uint\n");
	return 0;
    }
    line++;
    errno = 0;
    result = (unsigned int)strtol(line, NULL, 10);
    if (errno) {
	fprintf(stderr, "WARN:format error:%s", line);
    }
    return result;
}

void read_random_param(FILE *f) {
    char line[256];

    fgets(line, 256, f);
    fgets(line, 256, f);
    fgets(line, 256, f);
    gmm = get_uint(line);
    fgets(line, 256, f);
    gs2 = get_uint(line);
    fgets(line, 256, f);
    gs3 = get_uint(line);
    fgets(line, 256, f);
    grot1 = get_uint(line);
    fgets(line, 256, f);
    grot2 = get_uint(line);
}

void print_ht_random(FILE *fp, ht_rand *ht) {
    int i, j;

    fprintf(fp, "index = %u ", ht->index);
    fprintf(fp, "gfsr:\n");
    for (i = 0; i < NN; i++) {
	for (j = 31; j >= 0; j--) {
	    if ((ht->gx[i] & (1 << j)) != 0) {
		fprintf(fp, "%c", '1');
	    } else {
		fprintf(fp, "%c", '0');
	    }
	}
	fprintf(fp, "\n");
    }
}
