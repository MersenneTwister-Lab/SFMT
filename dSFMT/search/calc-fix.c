#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>

#define FIXSIZE 12

int SL1, SL2;
uint64_t MSK1, MSK2;
uint64_t LOW = 0x000FFFFFFFFFFFFFULL;
uint64_t HIGH = 0x3FF0000000000000ULL;

uint64_t get_fix(uint64_t c, uint64_t msk);
uint64_t get_fix1(uint64_t c, uint64_t msk);
uint64_t get_fix2(uint64_t c, uint64_t msk);
bool check(uint64_t a, uint64_t l, uint64_t msk);
bool check2(uint64_t a1, uint64_t a2, uint64_t l1, uint64_t l2);

void do_recursion(uint64_t a[2], uint64_t b[2],
		  uint64_t lung[2]);

int main(int argc, char* argv[]) {
    int i;
    uint64_t a1, a2, l1, l2;
    bool ok;

    if (argc < 4) {
	printf("usage: %s SL1 SL2 MSK1 MSK2\n");
	return 1;
    }
    SL1 = (int)strtol(argv[1], NULL, 10);
    SL2 = (int)strtol(argv[2], NULL, 10);
    MSK1 = (uint64_t)strtoll(argv[3], NULL, 16);
    MSK2 = (uint64_t)strtoll(argv[3], NULL, 16);

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
    printf("A[0] = %016"PRIx64"\n", a1);
    printf("A[1] = %016"PRIx64"\n", a2);
    printf("L[0] = %016"PRIx64"\n", l1);
    printf("L[1] = %016"PRIx64"\n", l2);
    if (check2(a1, a2, l1, l2)) {
	printf("check 2 ok\n");
    } else {
	printf("check 2 ng\n");
    }
    return 0;
}

uint64_t get_fix(uint64_t c, uint64_t msk) {
    if (SL1 < SL2) {
	return get_fix1(c, msk);
    } else {
	return get_fix2(c, msk);
    }
}

uint64_t get_fix1(uint64_t c, uint64_t msk) {
    int min, max;
    uint64_t i, a, b, r, one;

    one = 1;
    a = 0;
    for (i = one << (SL1 + 1); i < (one << 52); i <<= 1) {
	b = a | i;
	r = b;
	r ^= (c ^ b) << SL1;
	r ^= (b & msk) << SL2;
	if ((r & i) == 0) {
	    a |= i;
	}
    }
    return (a & LOW) | HIGH;
}

uint64_t get_fix2(uint64_t c, uint64_t msk) {
    int min, max;
    uint64_t i, a, b, r, one;

    one = 1;
    a = 0;
    for (i = one << (SL2 + 1); i < (one << 52); i <<= 1) {
	b = a | i;
	r = b;
	r ^= (c ^ b) << SL1;
	r ^= (b & msk) << SL2;
	if ((r & i) == 0) {
	    a |= i;
	}
    }
    return (a & LOW) | HIGH;
}

bool check(uint64_t a, uint64_t l, uint64_t msk) {
    uint64_t r, u;

    r = a;
    r ^= (l ^ a) << SL1;
    r ^= (a & msk) << SL2;
    return r == 0;
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
