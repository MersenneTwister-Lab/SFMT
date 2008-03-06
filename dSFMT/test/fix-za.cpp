/* fix point search for sfmtDPza 
 * using NTL
 */
#include <iostream>
#include <stdio.h>
#include <inttypes.h>
#include <NTL/mat_GF2.h>

NTL_CLIENT;

void calc_fix(int sl, int sr, uint64_t mask1, uint64_t mask2);
void setup_mask(mat_GF2& m_mask, uint64_t mask1, uint64_t mask2);
void setup_perm(mat_GF2& m_perm);
void setup_sl(mat_GF2& m_sl, int sl);
void setup_sr(mat_GF2& m_sr, int sr);
void makeup(mat_GF2& M, mat_GF2& m1, mat_GF2& m2, mat_GF2& m3, mat_GF2& m4);
void get_kernel(mat_GF2& M);

int main(int argc, char *argv[]) {
    int sl;
    int sr;
    uint64_t mask1;
    uint64_t mask2;

    if (argc < 4) {
	printf("usage: %s SL1 SR1 MSK1 MSK2\n", argv[0]);
	return 1;
    }
    sl = (int)strtol(argv[1], NULL, 10);
    sr = (int)strtol(argv[2], NULL, 10);
    mask1 = (uint64_t)strtoll(argv[3], NULL, 16);
    mask2 = (uint64_t)strtoll(argv[4], NULL, 16);

    calc_fix(sl, sr, mask1, mask2);
    return 0;
}

void calc_fix(int sl, int sr, uint64_t mask1, uint64_t mask2) {
    mat_GF2 m_perm(INIT_SIZE, 128, 128);
    mat_GF2 m_sl(INIT_SIZE, 128, 128);
    mat_GF2 m_sr(INIT_SIZE, 128, 128);
    mat_GF2 m_mask(INIT_SIZE, 128, 128);
    mat_GF2 m_i(INIT_SIZE, 128, 128);
    mat_GF2 s2(INIT_SIZE, 128, 128);
    mat_GF2 tmp(INIT_SIZE, 128, 128);
    mat_GF2 m1(INIT_SIZE, 128, 128);
    mat_GF2 m2(INIT_SIZE, 128, 128);
    mat_GF2 M(INIT_SIZE, 256, 256);

    ident(m_i, 128);
    setup_mask(m_mask, mask1, mask2);
    setup_perm(m_perm);
    setup_sl(m_sl, sl);
    setup_sr(m_sr, sr);

    add(s2, m_sl, m_i);

    mul(m1, m_sr, s2);
    mul(tmp, m_mask, s2);
    add(m1, m1, tmp);
    add(m1, m1, m_i);

    mul(m2, m_sr, m_perm);
    mul(tmp, m_mask, m_perm);
    add(m2, m2, tmp);

    makeup(M, m1, m2, s2, m_perm);
    get_kernel(M);
}

void setup_mask(mat_GF2& m_mask, uint64_t mask1, uint64_t mask2) {
    int i;

    clear(m_mask);
    for (i = 0; i < 64; i++) {
	m_mask[i][i] = (mask1 >> i) & 1;
    }
    for (i = 0; i < 64; i++) {
	m_mask[i + 64][i + 64] = (mask2 >> i) & 1;
    }
}

void setup_perm(mat_GF2& m_perm) {
    int i;
    int j;
    
    clear(m_perm);
    for (i = 0; i < 4; i++) {
	for (j = 0; j < 32; j++) {
	    m_perm[i * 32 + j][(3 - i) * 32 + j] = 1;
	}
    }
}

/* 下位ビットがマトリクスでも小さいインデクス */
void setup_sr(mat_GF2& m_sr, int sr) {
    int i;

    for (i = 0; i + sr < 64; i++) {
	m_sr[i][i + sr] = 1;
    }
    for (i = 0; i + sr < 64; i++) {
	m_sr[i + 64][i + 64 + sr] = 1;
    }
}

void setup_sl(mat_GF2& m_sl, int sl) {
    int i;

    for (i = sl; i < 64; i++) {
	m_sl[i][i - sl] = 1;
    }
    for (i = sl; i < 64; i++) {
	m_sl[i + 64][i + 64 - sl] = 1;
    }
}

void makeup(mat_GF2& M, mat_GF2& m1, mat_GF2& m2, mat_GF2& m3, mat_GF2& m4) {
    int i;
    int j;

    for (i = 0; i < 128; i++) {
	for (j = 0; j < 128; j++) {
	    M[i][j] = m1[i][j];
	}
    }
    for (i = 0; i < 128; i++) {
	for (j = 0; j < 128; j++) {
	    M[i][j + 128] = m2[i][j];
	}
    }
    for (i = 0; i < 128; i++) {
	for (j = 0; j < 128; j++) {
	    M[i + 128][j] = m3[i][j];
	}
    }
    for (i = 0; i < 128; i++) {
	for (j = 0; j < 128; j++) {
	    M[i + 128][j + 128] = m4[i][j];
	}
    }
}

void get_kernel(mat_GF2& M) {
    mat_GF2 i;
    mat_GF2 t;
    mat_GF2 x;
    vec_GF2 v;

    ident(i, 256);
    add(i, i, M);
    if (!IsZero(determinant(i))) {
	printf("M doesn't have eigenvalue 1\n");
	return;
    }
    transpose(t, i);
    kernel(x, t);
    t.SetDims(24, 24);
    clear(t);
    for (int j = 0; j < x.NumRows() && j < 24; j++) {
	for (int k = 0; k < 12; k++) {
	    t[j][k] = x[j][k + 52];
	    t[j][k + 12] = x[j][k + 52 + 64];
	}
    }
    x = t;
    gauss(x);
    printf("kernel \n");
    for (int j = 0; j < x.NumRows(); j++) {
	for (int k = 0; k < 12; k++) {
	    if (IsOne(x[j][k])) {
		printf("1");
	    } else {
		printf("0");
	    }
	}
	printf(" ");
	for (int k = 0; k < 12; k++) {
	    if (IsOne(x[j][12 + k])) {
		printf("1");
	    } else {
		printf("0");
	    }
	}
	printf("\n");
    }
}
