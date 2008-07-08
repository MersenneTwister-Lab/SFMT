/* fix point search for sfmtDPza 
 * 線形代数を使った不動点の求め方
 */
#include <iostream>
#include <stdio.h>
#include <inttypes.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>

NTL_CLIENT;

void calc_fix(int sl, int sr, uint64_t mask1, uint64_t mask2);
void setup_mask(mat_GF2& m_mask, uint64_t mask1, uint64_t mask2);
void setup_perm(mat_GF2& m_perm);
void setup_sl(mat_GF2& m_sl, int sl);
void setup_sr(mat_GF2& m_sr, int sr);
void setup_c(vec_GF2& C);
void makeup(mat_GF2& M, mat_GF2& m1, mat_GF2& m2, mat_GF2& m3, mat_GF2& m4);
void makeup_kakudai(mat_GF2& K, mat_GF2& M, vec_GF2& C);
void hakidashi(mat_GF2& M);

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

/* 固定された部分を除いて、a は 104bit L は 128bit 
 * y2 = Slx + Py + x + c1
 * x' = Sry2 + My2 + x
 * y' = y2
*/
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
    mat_GF2 M(INIT_SIZE, 104 + 128, 104 + 128);
    mat_GF2 K, K2;
    mat_GF2 Id;
    vec_GF2 C;
    long r1, r2;

    ident(m_i, 128);
    setup_mask(m_mask, mask1, mask2);
    setup_perm(m_perm);
    setup_sl(m_sl, sl);
    setup_sr(m_sr, sr);
    setup_c(C);

    add(s2, m_sl, m_i);

    mul(m1, m_sr, s2);
    mul(tmp, m_mask, s2);
    add(m1, m1, tmp);
    add(m1, m1, m_i);

    mul(m2, m_sr, m_perm);
    mul(tmp, m_mask, m_perm);
    add(m2, m2, tmp);

    makeup(M, m1, m2, s2, m_perm);
    ident(Id, M.NumRows());
    add(M, M, Id);
    makeup_kakudai(K, M, C);
    K2 = K;
    r1 = gauss(M);
    r2 = gauss(K);
    if (r1 < r2) {
	printf("解なし %ld < %ld\n", r1, r2);
    } else {
	hakidashi(K2);
    }
}

/* 下位ビットがマトリクスでも小さいインデクス */
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

/* 下位ビットがマトリクスでも小さいインデクス */
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

/* 下位ビットがマトリクスでも小さいインデクス */
void setup_sl(mat_GF2& m_sl, int sl) {
    int i;

    for (i = sl; i < 64; i++) {
	m_sl[i][i - sl] = 1;
    }
    for (i = sl; i < 64; i++) {
	m_sl[i + 64][i + 64 - sl] = 1;
    }
}

/*
 * M = | m1 m2 |
 *     | m3 m4 |
 */
void makeup(mat_GF2& M, mat_GF2& m1, mat_GF2& m2, mat_GF2& m3, mat_GF2& m4) {
    int i;
    int j;

    for (i = 0; i < 104; i++) {
	for (j = 0; j < 52; j++) {
	    M[i][j] = m1[i][j];
	}
	for (j = 0; j < 52; j++) {
	    M[i][j + 52] = m1[i][j + 64];
	}
    }
    for (i = 0; i < 52; i++) {
	for (j = 0; j < 128; j++) {
	    M[i][j + 104] = m2[i][j];
	}
    }
    for (i = 0; i < 52; i++) {
	for (j = 0; j < 128; j++) {
	    M[i+52][j + 104] = m2[i+64][j];
	}
    }

    for (i = 0; i < 64; i++) {
	for (j = 0; j < 52; j++) {
	    M[i + 104][j] = m3[i][j];
	}
	for (j = 0; j < 52; j++) {
	    M[i + 104 + 52][j] = m3[i+64][j];
	}
    }
    for (i = 0; i < 128; i++) {
	for (j = 0; j < 128; j++) {
	    M[i + 104][j + 104] = m4[i][j];
	}
    }
}

void setup_c(vec_GF2& C) {
    int i;
    //uint64_t c64 = 0xbff0000000000000LLU;
    uint64_t c64 = 0x3ff0000000000000LLU;

    C.SetLength(128 + 104);
    clear(C);
    for (i = 0; i < 64; i++) {
	C[i + 104] = (c64 >> i) & 1;
	C[i + 104 + 64] = (c64 >> i) & 1;
    }
}

void makeup_kakudai(mat_GF2& K, mat_GF2& M, vec_GF2& C) {
    int i, j;

    K.SetDims(M.NumRows(), M.NumCols() + 1);
    for (i = 0; i < K.NumRows(); i++) {
	for (j = 0; j < M.NumCols(); j++) {
	    K[i][j] = M[i][j];
	}
	K[i][M.NumCols()] = C[i];
    }
}

void hakidashi(mat_GF2& t) {
    int i, j;
    bool found;

    /* 掃き出し */
    for (i = 0; i < t.NumRows(); i++) {
	if (IsZero(t[i][i])) {
	    found = false;
	    for (j = i + 1; j < t.NumRows(); j++) {
		if (IsOne(t[j][i])) {
		    t[i] += t[j];
		    found = true;
		    break;
		}
	    }
	    if (!found) {
		continue;
	    }
	}
	for (j = 0; j < t.NumRows(); j++) {
	    if (j != i && IsOne(t[j][i])) {
		t[j] += t[i];
	    }
	}
    }
#if 0
    /* 全体のプリント */
    for (i = 0; i < t.NumRows(); i++) {
	for (j = 0; j < t.NumCols() - 1; j++) {
	    if (IsOne(t[i][j])) {
		printf("1");
	    } else {
		printf("0");
	    }
	}
	if (IsOne(t[i][t.NumCols() -1])) {
	    printf(" 1\n");
	} else {
	    printf(" 0\n");
	}
    }
#endif
    /* 解？のプリント */
    uint64_t a1 = 0;
    uint64_t a2 = 0;
    uint64_t l1 = 0;
    uint64_t l2 = 0;
    for (i = 0; i < 52; i++) {
	if (IsOne(t[i][t.NumCols() -1])) {
	    a1 = a1 | ((uint64_t)1 << i);
	}
    }
    for (i = 0; i < 52; i++) {
	if (IsOne(t[i + 52][t.NumCols() -1])) {
	    a2 = a2 | ((uint64_t)11 << i);
	}
    }
    for (i = 0; i < 64; i++) {
	if (IsOne(t[i + 104][t.NumCols() -1])) {
	    l1 = l1 | ((uint64_t)11 << i);
	}
    }
    for (i = 0; i < 64; i++) {
	if (IsOne(t[i + 104 + 64][t.NumCols() -1])) {
	    l2 = l2 | ((uint64_t)11 << i);
	}
    }
    printf("a1 = %016llx\n", a1);
    printf("a2 = %016llx\n", a2);
    printf("l1 = %016llx\n", l1);
    printf("l2 = %016llx\n", l2);
}
