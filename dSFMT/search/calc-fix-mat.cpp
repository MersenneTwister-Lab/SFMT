#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <NTL/mat_GF2.h>

NTL_CLIENT;

#define FIXSIZE 12

uint64_t LOW = 0x000FFFFFFFFFFFFFULL;
uint64_t HIGH = 0x3FF0000000000000ULL;
int SL1, SL2;
uint64_t MSK1, MSK2;

bool check(uint64_t a, uint64_t l, uint64_t msk);
bool check2(uint64_t a1, uint64_t a2, uint64_t l1, uint64_t l2);
void set_mat(mat_GF2& mat, int sl1, int sl2, uint64_t msk);
void do_recursion(uint64_t a[2], uint64_t b[2],
		  uint64_t lung[2]);

int main(int argc, char* argv[]) {
    int i;
    uint64_t a1, a2, l1, l2;
    bool ok;
    mat_GF2 mat;
    mat_GF2 inv_m;
    GF2 d;

    if (argc < 4) {
	printf("usage: %s SL1 SL2 MSK1 MSK2\n", argv[0]);
	return 1;
    }
    SL1 = (int)strtol(argv[1], NULL, 10);
    SL2 = (int)strtol(argv[2], NULL, 10);
    MSK1 = (uint64_t)strtoll(argv[3], NULL, 16);
    MSK2 = (uint64_t)strtoll(argv[4], NULL, 16);

    mat.SetDims(52, 52);
    set_mat(mat, SL1, SL2, MSK1);
    //cout << "mat" << mat << endl;
    inv(d, inv_m, mat);
    cout << "inv d:" << d << ", mat:" << inv_m << endl;
    return 0;

#if 0
    ok = false;
    for (i = 1; i < 4096; i++) {
	a1 = get_fix(i, MSK1);
	l2 = (a1 >> SL1) & 0x0fff;
	if (check(a1, l2, MSK1)) {
	    ok = true;
	    break;
	}
    }
    if (!ok) {
	printf("can't find fix point in step 1\n");
    }
    ok = false;
    for (i = 1; i < 4096; i++) {
	a2 = get_fix(i, MSK2);
	l1 = (a2 >> SL1) & 0x0fff;
	if (check(a2, l1, MSK2)) {
	    ok = true;
	    break;
	}
    }
    if (!ok) {
	printf("can't find fix point in step 1\n");
    }
    printf("A[0] = %016llx\n", a1);
    printf("A[1] = %016llx\n", a2);
    printf("L[0] = %016llx\n", l1);
    printf("L[1] = %016llx\n", l2);
    if (check2(a1, a2, l1, l2)) {
	printf("check 2 ok\n");
    } else {
	printf("check 2 ng\n");
    }
    return 0;
#endif
}

void set_mat(mat_GF2& mat, int sl1, int sl2, uint64_t msk) {
    int i, j;

    sl1 -= 12;
    sl2 -= 12;
    printf("msk = %016llx\n", msk);
    msk &= ~ 0xfffLLU;
    printf("msk = %016llx\n", msk);
    for (i = 12; i < 52; i++) {
	mat.put(i, i - 12, 1);
    }
    for (i = 0; i < 52 - sl1; i++) {
	mat.put(i, i + sl1, 1);
    }
    for (i = 0; i < 52 - sl2; i++) {
	mat.put(i, i + sl2, (msk >> (52 - (i + sl2))) & 1);
    }
}

void do_recursion(uint64_t a[2], uint64_t b[2], uint64_t lung[2]) {
    uint64_t s0, s1, t0, t1;

    t0 = (lung[1] ^ a[0]) << SL1;
    t1 = (lung[0] ^ a[1]) << SL1;
    s0 = b[0] ^ ((b[0] & MSK1) << SL2); 
    s1 = b[1] ^ ((b[1] & MSK2) << SL2); 
    lung[0] = lung[0] ^ s0 ^ t0; 
    lung[1] = lung[1] ^ s1 ^ t1;
    a[0] = a[0] ^ (lung[0] >> FIXSIZE);
    a[1] = a[1] ^ (lung[1] >> FIXSIZE);
}

bool check2(uint64_t a1, uint64_t a2, uint64_t l1, uint64_t l2) {
    uint64_t a[2], b[2], lung[2];

    a[0] = a1;
    a[1] = a2;
    b[0] = a1;
    b[1] = a2;
    lung[0] = l1;
    lung[1] = l2;
    do_recursion(a, b, lung);
    return a[0] == a1 && a[1] == a2 && lung[0] == l1 && lung[1] == l2;
}
