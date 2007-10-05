#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/GF2XFactoring.h>

#include "shortbase128.h"
#include "util.h"
#include "dsfmtL-util.h"
#include <unistd.h>

extern "C" {
#include "dsfmtL-st.h"
#include "mt19937blk.h"
}

NTL_CLIENT;

int get_equiv_distrib(int bit, dsfmt_t *sfmt);
void make_zero_state(dsfmt_t *sfmt, const GF2X& poly);
void test_shortest(char *filename);

static int mexp;
static int maxdegree;
static FILE *frandom;
static GF2X cha;

static bool use_base = false;
static char* filename = NULL;

void option(int argc, char * argv[]) {
    int c;
    bool error = false;
    char *pgm = argv[0];
    for (;;) {
	c = getopt(argc, argv, "hb");
	if (error) {
	    break;
	}
	if (c == -1) {
	    if (optind >= 0) {
		filename = argv[optind];
	    }
	    break;
	}
	switch (c) {
	case 'b':
	    use_base = true;
	    break;
	case 'h':
	default:
	    error = true;
	    break;
	}
    }
    if (error || filename == NULL) {
	printf("%s [-b] filename\n", pgm);
	    exit(0);
    }
}

int fill_state_base(dsfmt_t *sfmt) {
    static int bit_pos = 0;
    int i, j, k;
    uint64_t mask;

#if 0
    if (bit_pos == 0) {
	printf("doing fill_state_base ...\n");
    }
#endif
    if (bit_pos >= maxdegree) {
	return 0;
    } else if (bit_pos >= maxdegree - 128) {
	k = bit_pos % 64;
	j = (bit_pos / 64) % 2;
	i = N;
	memset(sfmt, 0, sizeof(dsfmt_t));
	mask = (uint64_t)1 << k;
	sfmt->status[i][j] = mask;
    } else {
	k = bit_pos % 52;
	j = (bit_pos / 52) % 2;
	i = bit_pos / (52 * 2);
	memset(sfmt, 0, sizeof(dsfmt_t));
	mask = (uint64_t)1 << k;
	sfmt->status[i][j] = mask;
    }
    bit_pos++;
    return 1;
}

void fill_rnd(dsfmt_t *sfmt) {
    const int a_max = 100000;
    static int idx = a_max;
    static uint32_t array[a_max];
    uint64_t u;
    int i, j;

    if (idx + (N + 1) * 4 >= a_max) {
	mt_fill(array, a_max);
	idx = 0;
    }
    for (i = 0; i < N; i++) {
	for (j = 0; j < 2; j++) {
	    u = array[idx++];
	    u = u << 32;
	    u = (u | array[idx++]) & 0x000FFFFFFFFFFFFFULL;
	    sfmt->status[i][j] = u;
	}
    }
    for (j = 0; j < 2; j++) {
	u = array[idx++];
	u = u << 32;
	u = u | array[idx++];
	sfmt->status[i][j] = u;
    }
}

int fill_state_random(dsfmt_t *sfmt, FILE *frandom) {
    static int count = 0;

    if (count > 5000 && count > MEXP * 2) {
	if (use_base) {
	    return fill_state_base(sfmt);
	}
	return 0;
    }
    fill_rnd(sfmt);
    count++;
    return 1;
}

void update(GF2X& min) {
#if 0
    printf("weight.min %d \n", weight(min));
#endif
    if (deg(cha) != maxdegree) {
	cha = min;
    } else if (weight(min) < weight(cha)) {
	cha = min;
    }
}

void kumiawase(GF2X& min, vec_GF2X& can, GF2X lcm, int zan) {
    int i, j, n;
    GF2X tmpX;
    vec_GF2X lis;

#if 0
    printf("deg.min = %d, zan = %d, can.length = %d\n", deg(min), 
	   zan, can.length());
#endif
    if (zan == 0) {
	tmpX = min * lcm;
	update(tmpX);
	return;
    } else if (zan < 0) {
	return;
    } else if (can.length() == 0) {
	return;
    }
    lis.SetMaxLength(can.length());
    lis.SetLength(can.length() - 1);
    for (i = 0; i < can.length() -1; i++) {
	lis[i] = can[i+1];
    }
    n = deg(can[0]);
    for (i = 0; i <= zan / n; i++) {
	tmpX = min;
	for (j = 0; j < i; j++) {
	    tmpX *= can[0];
	}
	kumiawase(tmpX, lis, lcm, zan - n * i);
    }
}

void calc_factor(GF2X& qpoly, GF2X& lcm, int diff) {
    vec_pair_GF2X_long factors;
    vec_GF2X can;
    GF2X min;
    int i;
    int cnt = 0;

    can.SetMaxLength(500);
    CanZass(factors, qpoly);
    for (i = 0; i < factors.length(); i++) {
	if (deg(factors[i].a) <= diff) {
	    can[cnt++] = factors[i].a;
	    can.SetLength(cnt);
	}
	printf("mul = %d, ", (int)factors[i].b);
	printBinary(stdout, factors[i].a);
    }
    if (cnt == 1) {
	for (i = 0; i < diff / deg(can[0]); i++) {
	    lcm *= can[0];
	}
	if (deg(lcm) == maxdegree) {
	    printf("weight = %ld\n", weight(lcm));
	    printBinary(stdout, lcm);
	}
    } else {
	min = 1;
	cha = lcm;
	kumiawase(min, can, lcm, diff);
	if (deg(cha) == maxdegree) {
	    printf("weight = %ld\n", weight(cha));
	    printBinary(stdout, cha);
	    if (deg(cha) > deg(lcm)) {
		printf("weight is larger than lcm\n");
	    }
	} else {
	    printf("can't get maxdegree\n");
	    printf("degree is %ld of %d\n", deg(cha), maxdegree);
	}
    }
}

void test_shortest(char *filename) {
    FILE *fp;
    GF2X poly;
    GF2X lcmpoly;
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    dsfmt_t sfmt;
    dsfmt_t sfmt_save;
    vec_GF2 vec;
    char line[200];
    int i;

    printf("filename:%s\n", filename);
    fp = fopen(filename, "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("main");
	fclose(fp);
	exit(1);
    }
    mt_init(1234);
    read_random_param(fp);
    init_gen_rand(&sfmt, 123);
    sfmt_save = sfmt;
    print_param(stdout);
    fgets(line, 200, fp);
    readFile(poly, fp);
    printf("deg poly = %ld\n", deg(poly));
    fclose(fp);
    printBinary(stdout, poly);
    vec.SetLength(2 * maxdegree);
    generating_polynomial104(&sfmt, vec, 0, maxdegree);
    berlekampMassey(lcmpoly, maxdegree, vec);
#if 1
    DivRem(tmp, rempoly, lcmpoly, poly);
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld: 0\n", deg(rempoly));
    } else {
	printf("divide OK\n");
    }
#endif
    for (i = 1; i < 104; i++) {
	generating_polynomial104(&sfmt, vec, i, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
    }
    for (i = 0;;i++) {
	if (fill_state_random(&sfmt, frandom) == 0) {
	    break;
	}
	generating_polynomial104(&sfmt, vec, i % 104, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	LCM(tmp, lcmpoly, minpoly);
	if (deg(tmp) >= (long)maxdegree) {
	    break;
	}
	lcmpoly = tmp;
	if (deg(lcmpoly) >= (long)maxdegree) {
	    break;
	}
    }
    if (deg(lcmpoly) != maxdegree) {
	printf("fail to get lcm, deg = %ld of %d\n", deg(lcmpoly), maxdegree);
	DivRem(tmp, rempoly, lcmpoly, poly);
	if (deg(rempoly) != -1) {
	    printf("rem != 0 deg rempoly = %ld\n", deg(rempoly));
	    exit(1);
	}
	calc_factor(tmp, lcmpoly, maxdegree - deg(lcmpoly));
	exit(1);
    }
    printf("deg lcm poly = %ld\n", deg(lcmpoly));
    printf("weight = %ld\n", weight(lcmpoly));
    printBinary(stdout, lcmpoly);
    DivRem(tmp, rempoly, lcmpoly, poly);
    printf("deg tmp = %ld\n", deg(tmp));
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld\n", deg(rempoly));
	exit(1);
    }
}

int main(int argc, char *argv[]) {
    option(argc, argv);
    mexp = get_rnd_mexp();
    maxdegree = get_rnd_maxdegree();
    printf("mexp = %d, maxdegree = %d\n", mexp, maxdegree);
    frandom = fopen("/dev/urandom", "r");
    if (errno) {
	perror("main");
	exit(1);
    }
    test_shortest(filename);
    fclose(frandom);
    return 0;
}
