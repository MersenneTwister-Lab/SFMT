/* dSFMT Search Code, M.Saito 2006/9/14 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include "util.h"
#include "dsfmt.h"

extern "C" {
    #include "mt19937blk.h"
}

#if !defined(MEXP)
#define MEXP 19937
#endif

const int mexp = MEXP;
const int WORDSIZE = 104;
const int N = (MEXP - 128) / WORDSIZE + 1;
const int MAXDEGREE = WORDSIZE * N + 128;
const uint64_t LOW_MASK = 0x000FFFFFFFFFFFFFULL;

uint64_t DSFMT::high_const = 0x0000000000000000ULL;
int DSFMT::pos1;
int DSFMT::pos2;
int DSFMT::pos3;
int DSFMT::sl1;
int DSFMT::sl2;
int DSFMT::sl3;
int DSFMT::sr1;
int DSFMT::sr2;
int DSFMT::sr3;
uint64_t DSFMT::msk1;
uint64_t DSFMT::msk2;
uint64_t DSFMT::msk3;
uint64_t DSFMT::msk4;
uint64_t DSFMT::msk5;
uint64_t DSFMT::msk6;
uint64_t DSFMT::fix[2];
uint64_t DSFMT::pcv[2];

unsigned int DSFMT::get_rnd_maxdegree(void) {
    return MAXDEGREE;
};

unsigned int DSFMT::get_rnd_mexp(void) {
    return mexp;
};

void DSFMT::setup_param(uint32_t array[], int *index) {
    pos1 = array[(*index)++] % (N - 1) + 1;
    //sl1 = array[(*index)++] % (51 - 13) + 13;
    sl1 = (array[(*index)++] % 8) * 2 + 13;
    msk1 = array[(*index)++];
    msk1 |= array[(*index)++];
    msk1 |= array[(*index)++];
    msk1 <<= 32;
    msk1 |= array[(*index)++];
    msk1 |= array[(*index)++];
    msk1 |= array[(*index)++];
    msk1 &= LOW_MASK;
    //msk1 |= 0x0fffU;
    msk2 = array[(*index)++];
    msk2 |= array[(*index)++];
    msk2 |= array[(*index)++];
    msk2 <<= 32;
    msk2 |= array[(*index)++];
    msk2 |= array[(*index)++];
    msk2 |= array[(*index)++];
    msk2 &= LOW_MASK;
    //msk2 |= 0x0fffU;
}

void DSFMT::set_pcv(uint64_t in_pcv[2]) {
    pcv[0] = in_pcv[0];
    pcv[1] = in_pcv[1];
}

void DSFMT::set_fix(uint64_t in_fix[2]) {
    fix[0] = in_fix[0];
    fix[1] = in_fix[1];
}

void DSFMT::print_param(FILE *fp) {
    fprintf(fp, "pos1 = %d\n", pos1);
    fprintf(fp, "sl1 = %d\n", sl1);
    fprintf(fp, "msk1 = %016"PRIx64"\n", msk1);
    fprintf(fp, "msk2 = %016"PRIx64"\n", msk2);
    fprintf(fp, "fix1 = %016"PRIx64"\n", fix[0]);
    fprintf(fp, "fix2 = %016"PRIx64"\n", fix[1]);
    fprintf(fp, "pcv1 = %016"PRIx64"\n", pcv[0]);
    fprintf(fp, "pcv2 = %016"PRIx64"\n", pcv[1]);
    fflush(fp);
}

void DSFMT::read_random_param(FILE *f) {
    int c;
    char line[256];

    fgets(line, 256, f);
    fgets(line, 256, f);
    fgets(line, 256, f);
    pos1 = get_uint(line, 10);
    fgets(line, 256, f);
    sl1 = get_uint(line, 10);
    fgets(line, 256, f);
    msk1 = get_uint64(line, 16);
    fgets(line, 256, f);
    msk2 = get_uint64(line, 16);
    c = getc(f);
    if (isdigit(c)) {
	ungetc(c, f);
	return;
    }
    ungetc(c, f);
    fgets(line, 256, f);
    if (strncmp(line, "fix", 3) != 0) {
	return;
    }
    fix[0] = get_uint64(line, 16);
    fgets(line, 256, f);
    fix[1] = get_uint64(line, 16);
    fgets(line, 256, f);
    pcv[0] = get_uint64(line, 16);
    fgets(line, 256, f);
    pcv[1] = get_uint64(line, 16);
}

DSFMT::DSFMT() {
    status = new uint64_t[N + 1][2];
    idx = 0;
    init_gen_rand(1);
}

DSFMT::DSFMT(uint64_t seed) {
    status = new uint64_t[N + 1][2];
    idx = 0;
    init_gen_rand(seed);
}

DSFMT::DSFMT(const DSFMT& src) {
    int i;

    status = new uint64_t[N + 1][2];
    for (i = 0; i < N + 1; i++) {
	status[i][0] = src.status[i][0];
	status[i][1] = src.status[i][1];
    }
    idx = src.idx;
}

DSFMT& DSFMT::operator=(const DSFMT& src) {
    int i;

    for (i = 0; i < N + 1; i++) {
	status[i][0] = src.status[i][0];
	status[i][1] = src.status[i][1];
    }
    idx = src.idx;
    return *this;
}

bool DSFMT::operator==(const DSFMT& src) {
    int i, k;

    assert(idx % 2 == 0);
    assert(src.idx % 2 == 0);

    if (status[N][0] != src.status[N][0] ||
	status[N][1] != src.status[N][1]) {
	return false;
    }
    
    k = (src.idx / 2 - idx / 2 + N) % N;
    for (i = 0; i < N; i++) {
	if (status[i][0] != src.status[(k + i) % N][0] ||
	    status[i][1] != src.status[(k + i) % N][1]) {
	    return false;
	}
    }
    return true;
}

void DSFMT::d_p() {
    int i;

    printf("idx = %d\n", idx);
    for (i = 0; i < N + 1; i++) {
	printf("%016"PRIx64" %016"PRIx64"\n", status[i][0], status[i][1]);
    }
}

DSFMT::~DSFMT() {
    delete status;
}

void DSFMT::mask_status() {
    int i;
    uint64_t msk = 0x3FF0000000000000ULL;

    for (i = 0; i < N; i++) {
	status[i][0] = (status[i][0] & LOW_MASK) | msk;
	status[i][1] = (status[i][1] & LOW_MASK) | msk;
    }
}

int DSFMT::period_certification(bool no_fix) {
    int inner = 0;
    int i, j;
    uint64_t tmp[2];
    uint64_t work;

    if (no_fix) {
	tmp[0] = status[N][0] & pcv[0];
	tmp[1] = status[N][1] & pcv[1];
    } else {
	tmp[0] = (status[N][0] ^ fix[0]) & pcv[0];
	tmp[1] = (status[N][1] ^ fix[1]) & pcv[1];
    }
    for (i = 0; i < 2; i++) {
	work = tmp[i];
	for (j = 0; j < 64; j++) {
	    inner ^= work & 1;
	    work = work >> 1;
	}
    }
    /* check OK */
    if (inner == 1) {
	return 1;
    }
    /* check NG, and modification */
    if ((pcv[1] & 1) == 1) {
	status[N][1] ^= 1;
	return 0;
    }
    for (i = 1; i >= 0; i--) {
	work = 1;
	for (j = 0; j < 64; j++) {
	    if ((work & pcv[i]) != 0) {
		status[N][i] ^= work;
		return 0;
	    }
	    work = work << 1;
	}
    }
    return 0;
}

inline static void do_recursion(uint64_t a[2], uint64_t b[2],
				uint64_t lung[2]) {
    uint64_t t0, t1, L0, L1;

    t0 = a[0];
    t1 = a[1];
    L0 = lung[0];
    L1 = lung[1];
    lung[0] = (t0 << DSFMT::sl1) ^ (L1 >> 32) ^ (L1 << 32) ^ b[0];
    lung[1] = (t1 << DSFMT::sl1) ^ (L0 >> 32) ^ (L0 << 32) ^ b[1];
    a[0] = (lung[0] >> 12) ^ (lung[0] & DSFMT::msk1) ^ t0;
    a[1] = (lung[1] >> 12) ^ (lung[1] & DSFMT::msk2) ^ t1;
}

void DSFMT::set_const(){
    const uint64_t high = 0x3ff0000000000000ULL;

    memset(status, 0, sizeof(uint64_t) * 2 * (N + 1));
    status[N][0] = high;
    status[N][1] = high;
    status[N - 1][0] = high >> 12;
    status[N - 1][1] = high >> 12;
    idx = 0;
}
/*
 * これは直接呼び出さないでgenrandを呼び出している。
 */
void DSFMT::next_state() {
    uint32_t i;

    if (idx >= N * 2) {
	idx = 0;
    }
    i = idx / 2;
    do_recursion(status[i], status[(i + pos1) % N], status[N]);
}

/* これは初期状態を出力する */
uint64_t DSFMT::gen_rand104sp(uint64_t array[2], int mode)
{
    int i, p;

    if (idx >= N * 2) {
	idx = 0;
    }
    i = idx / 2;
    p = idx + 2;
    if (p >= N * 2) {
	p = 0;
    }
    p = p / 2;
    switch (mode) {
    case 0:
	array[0] = status[i][0] & LOW_MASK;
	array[1] = status[i][1] & LOW_MASK;
	break;
    case 1:
    default:
	array[0] = status[i][1] & LOW_MASK;
	array[1] = status[p][0] & LOW_MASK;
    }

    next_state();
    idx += 2;
    if (idx >= N * 2) {
	idx = 0;
    }
    return array[0];
}

/* output lung */
void DSFMT::gen_rand128spar(uint64_t array[][2], int size) {
    int i;
    int j;

    if (idx >= N * 2) {
	idx = 0;
    }
    i = idx / 2;
    for (j = 0; j < size; j++) {
	array[j][0] = status[N][0];
	array[j][1] = status[N][1];
	//next_state();
	do_recursion(status[i], status[(i + pos1) % N], status[N]);
	i = (i + 1) % N ;
    }
    idx = i * 2;
}

void DSFMT::gen_rand104spar(uint64_t array[][2], int size) {
    int i;
    int j;

    if (idx >= N * 2) {
	idx = 0;
    }
    i = idx / 2;
    for (j = 0; j < size; j++) {
	array[j][0] = status[i][0] & LOW_MASK;
	array[j][1] = status[i][1] & LOW_MASK;
	//next_state();
	do_recursion(status[i], status[(i + pos1) % N], status[N]);
	i = (i + 1) % N ;
    }
    idx = i * 2;
}

double DSFMT::gen_rand() {
    int i, j;
    union {
	uint64_t u;
	double d;
    } x;

    idx++;
    if (idx >= N * 2) {
	idx = 0;
    }
    if (idx % 2 == 0) {
	next_state();
    }
    i = idx / 2;
    j = idx % 2;
    x.u = status[i][j];
    return x.d;
}

void DSFMT::add(const DSFMT& src) {
    int i, k;

    assert(idx % 2 == 0);
    assert(src.idx % 2 == 0);
    
    k = (src.idx / 2 - idx / 2 + N) % N;
    for (i = 0; i < N; i++) {
	status[i][0] ^= src.status[(k + i) % N][0];
	status[i][1] ^= src.status[(k + i) % N][1];
    }
    status[N][0] ^= src.status[N][0];
    status[N][1] ^= src.status[N][1];
}

void DSFMT::get_lung(uint64_t lung[2]) {
    lung[0] = status[N][0];
    lung[1] = status[N][1];
}

void DSFMT::initial_mask(uint64_t high) {
    int i;
    uint64_t *psfmt;

    psfmt = &status[0][0];
    for (i = 0; i < N * 2; i++) {
        psfmt[i] = (psfmt[i] & LOW_MASK) | high;
    }
}

void DSFMT::init_gen_rand(uint64_t seed, uint64_t high)
{
    int i;
    uint64_t *psfmt;
    uint32_t *psfmt32;

    if (seed == 0) {
	psfmt = status[0];
	for (i = 0; i < N * 2; i++) {
	    psfmt[i] = high;
	}
	for (;i < (N + 1) * 2; i++) {
	    psfmt[i] = 0;
	}
	idx = 0;
	return;
    }
    psfmt32 = (uint32_t *)&status[0][0];
    psfmt32[idxof(0)] = seed;
    for (i = 1; i < (N + 1) * 4; i++) {
        psfmt32[idxof(i)] = 1812433253UL 
	    * (psfmt32[idxof(i - 1)] ^ (psfmt32[idxof(i - 1)] >> 30)) + i;
    }
    initial_mask(high);
    idx = N * 2;
}

void DSFMT::fill_rnd(uint64_t high) {
    const int size = (N + 1) * 4;
    uint32_t array[size];
    uint64_t u;
    int i, j;
    int idx = 0;

    mt_fill(array, size);
    for (i = 0; i < N; i++) {
	for (j = 0; j < 2; j++) {
	    u = array[idx++];
	    u = u << 32;
	    u = ((u | array[idx++]) & LOW_MASK) | high;
	    status[i][j] = u;
	}
    }
    for (j = 0; j < 2; j++) {
	u = array[idx++];
	u = u << 32;
	u = u | array[idx++];
	status[i][j] = u;
    }
    this->idx = 0;
}

void DSFMT::fill_rnd_all(int p) {
    int i, j, k;

    if (p < (int)get_rnd_maxdegree() - 128) {
	i = p / 104;
	j = (p % 104) / 52;
	k = p % 52;
    } else {
	i = N;
	j = (p - 104 * N) / 64;
	k = (p - 104 * N) % 64;
    }
    assert(0 <= i && i <= N);
    assert(0 <= j && j <= 1);
    assert(0 <= k && k < 64);
    memset(status, 0, sizeof(uint64_t) * 2 * (N + 1));
    idx = 0;
    status[i][j] = (uint64_t)1 << k;
}

#if defined(MAIN)
int main(int argc, char * argv[]) {
    int i;
    dsfmt_t dsfmt;
    union {
	uint64_t u;
	double d;
    } un;
    FILE *fp;

    LOW_MASK =   0x000FFFFFFFFFFFFFULL;
    high_const = 0x3ff0000000000000ULL;
    if (argc > 1) {
	fp = fopen(argv[1], "r");
	errno = 0;
	if ((fp == NULL) || errno) {
	    perror("main");
	    fclose(fp);
	    exit(1);
	}
	read_random_param(fp);
    }
    init_gen_rand(&dsfmt, 1234);
    printf("generated randoms [1, 2)\n");
    for (i = 0; i < 1000; i++) {
	if (i % 2 == 0) {
	    next_state(&dsfmt);
	}
	un.u = dsfmt.status[dsfmt.idx / 2][dsfmt.idx % 2];
	dsfmt.idx++;
	printf("%1.17f ", un.d);
	//printf("%016"PRIx64" ", un.u);
	if (i % 3 == 2) {
	    printf("\n");
	}
    }
    printf("\n");
    return 0;
}
#endif
