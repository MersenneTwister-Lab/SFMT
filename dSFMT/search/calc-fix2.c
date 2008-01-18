#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define FIXSIZE 12
#define LIST_SIZE 100

static int SL1, SL2;
static uint64_t MSK1, MSK2;
//static uint64_t LOW = 0x000FFFFFFFFFFFFFULL;
static uint64_t HIGH = 0x3FF0000000000000ULL;
static bool VERBOSE = false;

struct {
    uint64_t l[LIST_SIZE];
    uint64_t a[LIST_SIZE];
    int index;
} LIST[2];
static int INDEX = 0;

void calc_fix(int sl1, int sl2, uint64_t msk1, uint64_t msk2);
static void setup_l(char l[64], int sl);
static void setup_a(char a[64], int sl1, int sl2);
static void setup_c(char c[64], uint64_t msk, int sl2);
static void print_array(char *mess, char *a);
static void calc_rec(char l[], char a[], char c[], int x, int y);
static bool calc(char l[], char a[], char c[], int x, int y);
static char and(char a, char b);
static char idxof(char a[], int idx);
static char plus(char a, char b, char c);
static void output(char l[64], char a[64]);
static void do_recursion(uint64_t a[2], uint64_t b[2], uint64_t lung[2]);
static bool check2(uint64_t a1, uint64_t a2, uint64_t l1, uint64_t l2);

#ifdef MAIN
int main(int argc, char* argv[]) {
    int sl1, sl2;
    uint64_t msk1, msk2;

    if (argc < 4) {
	printf("usage: %s SL1 SL2 MSK1 MSK2\n", argv[0]);
	return 1;
    }
    sl1 = (int)strtol(argv[1], NULL, 10);
    sl2 = (int)strtol(argv[2], NULL, 10);
    if (strlen(argv[3]) == 16) {
	msk1 = (uint64_t)strtoll(argv[3]+1, NULL, 16);
    } else {
	msk1 = (uint64_t)strtoll(argv[3], NULL, 16);
    }
    if (strlen(argv[4]) == 16) {
	msk2 = (uint64_t)strtoll(argv[4]+1, NULL, 16);
    } else {
	msk2 = (uint64_t)strtoll(argv[4], NULL, 16);
    }
    VERBOSE = true;
    calc_fix(sl1, sl2, msk1, msk2);
    return 0;
}
#endif

void calc_fix(int sl1, int sl2, uint64_t msk1, uint64_t msk2) {
    char a[64];
    char l[64];
    char c[64];
    int i, j;
    bool b;

    SL1 = sl1;
    SL2 = sl2;
    MSK1 = msk1;
    MSK2 = msk2;

    INDEX = 0;
    LIST[0].index = 0;
    LIST[1].index = 0;
    setup_l(l, SL1);
    setup_c(c, MSK1, SL2);
    setup_a(a, SL1, SL2);
    calc(l, a, c, SL1, SL2);
    print_array("l = ", l);
    print_array("a = ", a);
    calc_rec(l, a, c, SL1, SL2);

    INDEX = 1;
    setup_l(l, SL1);
    setup_c(c, MSK2, SL2);
    setup_a(a, SL1, SL2);
    calc(l, a, c, SL1, SL2);
    print_array("l = ", l);
    print_array("a = ", a);
    calc_rec(l, a, c, SL1, SL2);

    printf("index1 = %d, index2  = %d\n", LIST[0].index, LIST[1].index);
    printf("fix point\n");
    fflush(stdout);
    for (i = 0; i < LIST[0].index; i++) {
	for (j = 0; j < LIST[1].index; j++) {
	    printf("A[0] = %016llx\n", LIST[0].a[i]);
	    printf("A[1] = %016llx\n", LIST[1].a[j]);
	    printf("L[0] = %016llx\n", LIST[1].l[j]);
	    printf("L[1] = %016llx\n", LIST[0].l[i]);
	    b = check2(LIST[0].a[i], LIST[1].a[j],
		       LIST[1].l[j], LIST[0].l[i]);
	    if (b) {
		printf("fix point check OK!\n");
	    } else {
		printf("fix point check NG!\n");
	    }
	    if (!VERBOSE) {
		return;
	    }
	}
    }
}

static void setup_l(char l[64], int sl){
    int i;

    for (i = 0; i < 12; i++) {
	l[i] = 3;
    }
    for (;i < 64; i++) {
	l[i] = 0;
    }
    for (i = 63; i >= sl; i--) {
	l[i] = l[i-sl];
    }
    for (; i >= 0; i--) {
	l[i] = 0;
    }
}
static void setup_c(char c[64], uint64_t msk, int sl2) {
    int i;

    msk = msk << sl2;
    for (i = 0; i < 64; i++) {
	c[i] = (msk >> i) & 1;
    }
}

static void setup_a(char a[64], int sl1, int sl2){
    int i, min;

    if (sl1 < sl2) {
	min = sl1;
    } else {
	min = sl2;
    }
    for (i = 0; i < min; i++) {
	a[i] = 0;
    }
    for (; i < 52; i++) {
	a[i] = 3;
    }
    for (; i < 64; i++) {
	a[i] = (HIGH >> i) & 1;
	//a[i] = ((HIGH >> i) & 1) * 3;
    }
}

static void print_array(char *mess, char *a) {
    int i;

    if (!VERBOSE) {
	return;
    }
    printf("%s", mess);
    for (i = 63; i >= 0; i--) {
	printf("%d", a[i]);
    }
    printf("\n");
}

static void calc_rec(char l[], char a[], char c[], int x, int y) {
    int i, cnt;
    char ll[64], aa[64];

    if (calc(l, a, c, x, y) != true) {
	return;
    }
    cnt = 0;
    for (i = 0; i < 64; i++) {
	if (l[i] == 3) {
	    cnt++;
	    memcpy(ll, l, 64);
	    memcpy(aa, a, 64);
	    ll[i] = 0;
	    calc_rec(ll, aa, c, x, y);
	    memcpy(ll, l, 64);
	    memcpy(aa, a, 64);
	    ll[i] = 1;
	    calc_rec(ll, aa, c, x, y);
	}
    }
    if (cnt == 0) {
	output(l, a);
    }
}

static bool calc(char l[], char a[], char c[], int x, int y) {
    int i;
    char s;
    bool change;

    change = true;
    while(change) {
	//print_array("debug a=", a);
	//print_array("debug l=", l);
	change = false;
	for (i = 0; i < 52; i++) {
	    s = plus(l[i], idxof(a, i-x), and(idxof(a, i-y), c[i]));
	    if (a[i] == 3) {
		if (s != 3) {
		    change = true;
		    a[i] = s;
		}
	    } else if (s != 3 && a[i] != s) {
		//printf("miss[a]: s=%d, a[%d]=%d\n", s, i, a[i]);
		return false;
	    }
	}
	for (i = 51; i >= 0; i--) {
	    s = plus(idxof(l, i+x), idxof(a, i+x),
		     and(idxof(a, i-y+x), idxof(c, i+x)));
	    if (a[i] == 3) {
		if (s != 3) {
		    change = true;
		    a[i] = s;
		}
	    } else if (s != 3 && a[i] != s) {
		//printf("miss[A]: s=%d, a[%d]=%d\n", s, i, a[i]);
		return false;
	    }
	}
	for (i = 0; i < 64; i++) {
	    s = plus(idxof(a, i-x), a[i], and(idxof(a, i-y), c[i]));
	    if (l[i] == 3) {
		if (s != 3) {
		    change = true;
		    l[i] = s;
		}
	    } else if (s != 3 && l[i] != s) {
		//printf("miss[L]: s=%d, l[%d]=%d\n", s, i, l[i]);
		return false;
	    }
	}
    }
    return true;
}

static char idxof(char a[], int idx) {
    if (idx < 0) {
	return 0;
    } else if (idx >= 64) {
	return 0;
    } else {
	return a[idx];
    }
}

static char and(char a, char b) {
    if (b == 0) {
	return 0;
    } else {
	return a;
    }
}

static char plus(char a, char b, char c) {
    if ((a | b | c) & 2) {
	return 3;
    } else {
	return a ^ b ^ c;
    }
}

static void output(char l[64], char a[64]) {
    uint64_t ll, aa;
    int i;

    ll = 0;
    aa = 0;
    for (i = 0; i < 64; i++) {
	ll |= (uint64_t)l[i] << i;
	aa |= (uint64_t)a[i] << i;
    }
    ll = ll >> SL1;
    for (i = 0; i < LIST[INDEX].index; i++) {
	if (LIST[INDEX].l[i] == ll && LIST[INDEX].a[i] == aa) {
	    return;
	}
    }
    if (LIST[INDEX].index < LIST_SIZE) {
	LIST[INDEX].l[LIST[INDEX].index] = ll;
	LIST[INDEX].a[LIST[INDEX].index] = aa;
	LIST[INDEX].index++;
	if (VERBOSE) {
	    printf("%llx, %llx\n", ll, aa);
	}
	print_array("l = ", l);
	print_array("a = ", a);
    }
}

static void do_recursion(uint64_t a[2], uint64_t b[2], uint64_t lung[2]) {
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

static bool check2(uint64_t a1, uint64_t a2, uint64_t l1, uint64_t l2) {
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