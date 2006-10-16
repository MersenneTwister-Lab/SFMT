#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>

#include "shortbase128.h"
#include "util.h"
#include "dsfmt-util.h"
#include <unistd.h>

extern "C" {
#include "dsfmt-st.h"
}

NTL_CLIENT;

int get_equiv_distrib(int bit, dsfmt_t *sfmt);
void make_zero_state(dsfmt_t *sfmt, const GF2X& poly);
void test_shortest(char *filename);

static int mexp;
static int maxdegree;
static FILE *frandom;

static bool bit_128 = false;
static bool bit_64 = true;
static bool msb = true;
static char* filename = NULL;

void option(int argc, char * argv[]) {
    int c;
    bool first = true;
    bool error = false;
    char *pgm = argv[0];
    for (;;) {
	c = getopt(argc, argv, "hrab:");
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
	case 'a':
	    bit_128 = true;
	    bit_64 = true;
	    break;
	case 'r':
	    msb = false;
	    break;
	case 'b':
	    if (first) {
		first = false;
		bit_128 = false;
		bit_64 = false;
	    }
	    if (strcmp("128", optarg) == 0) {
		bit_128 = true;
		break;
	    }
	    if (strcmp("64", optarg) == 0) {
		bit_64 = true;
		break;
	    }
	case 'h':
	default:
	    error = true;
	    break;
	}
    }
    if (error || filename == NULL) {
	printf("%s [-a | -b128 -b64] [-r] filename\n", pgm);
	    exit(0);
    }
}

#if 0
void fill_state_random(dsfmt_t *sfmt, FILE *frandom) {
    int i, j;
    int w, z;
    uint64_t x;

    for (i = 0; i <= N; i++) {
	for (j = 0; j < 2; j++) {
	    w = getw(frandom);
	    z = getw(frandom);
	    if (feof(frandom) || ferror(frandom)) {
		if (errno != 0) {
		    printf("test_shortest:%s\n", strerror(errno));
		} else {
		    printf("test_shortest:/dev/urandom reached to EOF!\n");
		}
		fclose(frandom);
		exit(1);
	    }
	    x = ((uint64_t)w << 32) | z;
	    sfmt->status[i][j] = x;
	}
    }
}
#else
void fill_state_random(dsfmt_t *sfmt, FILE *frandom) {
    int i, j;
    int w, z;
    uint64_t x;

    i = ((uint32_t)getw(frandom)) % (N + 1);
    j = ((uint32_t)getw(frandom)) % 2;
    w = getw(frandom);
    z = getw(frandom);
    if (feof(frandom) || ferror(frandom)) {
	if (errno != 0) {
	    printf("test_shortest:%s\n", strerror(errno));
	} else {
	    printf("test_shortest:/dev/urandom reached to EOF!\n");
	}
	fclose(frandom);
	exit(1);
    }
    x = ((uint64_t)w << 32) | z;
    sfmt->status[i][j] = x;
}

#endif


bool check_minpoly128_hi(dsfmt_t *sfmt, const GF2X& minpoly,
			 unsigned int bitpos) {
    uint32_t sum;
    uint64_t ar[2];
    uint64_t mask;
    int i;

    sum = 0;
    mask = (uint64_t)1UL << (63 - bitpos);
    for (int j = 0; j < 500; j++) {
	for (i = 0; i <= deg(minpoly); i++) {
	    gen_rand104sp(sfmt, ar, 0);
	    if (mask & ar[0] != 0) {
		sum ^= 1;
	    }
	}
	if (sum != 0) {
	    return false;
	}
    }
    return true;
}

bool check_minpoly128_low(dsfmt_t *sfmt, const GF2X& minpoly,
			  unsigned int bitpos) {
    uint32_t sum;
    uint64_t ar[2];
    uint64_t mask;
    int i;

    sum = 0;
    mask = (uint64_t)1UL << (63 - bitpos);
    for (int j = 0; j < 500; j++) {
	for (i = 0; i <= deg(minpoly); i++) {
	    gen_rand104sp(sfmt, ar, 0);
	    if (mask & ar[1] != 0) {
		sum ^= 1;
	    }
	}
	if (sum != 0) {
	    return false;
	}
    }
    return true;
}

bool check_minpoly128(dsfmt_t *sfmt, const GF2X& minpoly, unsigned int bitpos) {
    if (bitpos < 64) {
	return check_minpoly128_hi(sfmt, minpoly, bitpos);
    } else {
	return check_minpoly128_low(sfmt, minpoly, bitpos - 64);
    }
}

int get_equiv_distrib(int bit, dsfmt_t *sfmt) {
    static dsfmt_t sfmtnew;
    int shortest;

    //fprintf(stderr, "now start get_equiv %d\n", bit);
    sfmtnew = *sfmt;
    set_up(128, bit, 0, msb);
    shortest = get_shortest_base(&sfmtnew);
    return shortest;
}

int get_equiv_distrib64(int bit, dsfmt_t *sfmt) {
    static dsfmt_t sfmtnew;
    int dist, min;
    uint32_t mode;

    min = INT_MAX;
    for (mode = 0; mode < 2; mode++) {
	sfmtnew = *sfmt;
	set_up(64, bit, mode, msb);
	dist = get_shortest_base(&sfmtnew);
	//printf("%d\n", dist);
	if (dist < min) {
	    min = dist;
	}
    }
    return min;
}

void make_zero_state(dsfmt_t *sfmt, const GF2X& poly) {
    dsfmt_t sfmtnew;
    uint64_t ar[2];
    int i;

    memset(&sfmtnew, 0, sizeof(sfmtnew));
    for (i = 0; i <= deg(poly); i++) {
	if (coeff(poly, i) != 0) {
	    add_rnd(&sfmtnew, sfmt);
	}
	gen_rand104sp(sfmt, ar, 0);
    }
    *sfmt = sfmtnew;
}

void test_shortest(char *filename) {
    unsigned int bit;
    FILE *fp;
    GF2X poly;
    GF2X lcmpoly;
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    dsfmt_t sfmt;
    dsfmt_t sfmt_save;
    vec_GF2 vec;
    int shortest;
    int i;
    int dist_sum;
    int count;
    int old;
    int lcmcount;

    printf("filename:%s\n", filename);
    fp = fopen(filename, "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("main");
	fclose(fp);
	exit(1);
    }
    read_random_param(fp);
    init_gen_rand(&sfmt, 123);
    sfmt_save = sfmt;
    print_param(stdout);
    readFile(poly, fp);
    printf("deg poly = %ld\n", deg(poly));
    fclose(fp);
    printBinary(stdout, poly);
    vec.SetLength(2 * maxdegree);
    generating_polynomial104(&sfmt, vec, 0, maxdegree);
    berlekampMassey(lcmpoly, maxdegree, vec);
#if 0
    if (check_minpoly128(&sfmt, lcmpoly, 0)) {
	printf("check minpoly OK!\n");
	DivRem(tmp, rempoly, lcmpoly, poly);
	if (deg(rempoly) != -1) {
	    printf("rem != 0 deg rempoly = %ld: 0\n", deg(rempoly));
	}

    } else {
	printf("check minpoly NG!\n");
    }
#endif
    for (i = 1; i < 104; i++) {
	generating_polynomial104(&sfmt, vec, i, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
#if 0
	DivRem(tmp, rempoly, lcmpoly, poly);
	if (deg(rempoly) != -1) {
	    printf("rem != 0 deg rempoly = %ld: %d\n", deg(rempoly), i);
	}
#endif
    }
    lcmcount = 0;
    while (deg(lcmpoly) < (long)maxdegree) {
	if (lcmcount > 5000) {
	    printf("failure to get max LCM\n");
	    break;
	}
	fill_state_random(&sfmt, frandom);
	for (int j = 0; j < 104; j++) {
	    generating_polynomial104(&sfmt, vec, j, maxdegree);
	    if (IsZero(vec)) {
		break;
	    }
	    berlekampMassey(minpoly, maxdegree, vec);
	    LCM(tmp, lcmpoly, minpoly);
	    if (deg(tmp) > (long)maxdegree) {
		break;
	    }
	    lcmpoly = tmp;
	    lcmcount++;
	    if (deg(lcmpoly) >= (long)maxdegree) {
		break;
	    }
	}
    }
    if (deg(lcmpoly) > maxdegree) {
	printf("fail to get lcm, deg = %ld\n", deg(lcmpoly));
	exit(1);
    }
#if 0
    sfmt = sfmt_save;
    if (check_minpoly128(&sfmt, lcmpoly, 0)) {
	printf("check minpoly 2 OK!\n");
    } else {
	printf("check minpoly 2 NG!\n");
    }
#endif
    printf("deg lcm poly = %ld\n", deg(lcmpoly));
    printf("weight = %ld\n", weight(lcmpoly));
    printBinary(stdout, lcmpoly);
    DivRem(tmp, rempoly, lcmpoly, poly);
    printf("deg tmp = %ld\n", deg(tmp));
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld\n", deg(rempoly));
	exit(1);
    }
    sfmt = sfmt_save;
    make_zero_state(&sfmt, tmp);
    generating_polynomial104(&sfmt, vec, 0, maxdegree);
    berlekampMassey(minpoly, maxdegree, vec);
    if (deg(minpoly) != MEXP) {
	printf("deg zero state = %ld\n", deg(minpoly));
	return;
    }

    if (bit_128) {
	dist_sum = 0;
	count = 0;
	old = 0;
	printf("104 bit k-distribution\n");
	for (bit = 1; bit <= 104; bit++) {
	//for (bit = 1; bit <= 1; bit++) {
	    shortest = get_equiv_distrib(bit, &sfmt);
	    dist_sum += mexp / bit - shortest;
	    if (old == shortest) {
		count++;
	    } else {
		old = shortest;
	    }
	    //printf("k(%d) = %d, %d, %d\n", bit, shortest, dist_sum, count);
	    printf("k(%d) = %d\n", bit, shortest);
	    fflush(stdout);
	}
	printf("104bit D.D:%7d, DUP:%5d\n", dist_sum, count);
    }

    if (bit_64) {
    dist_sum = 0;
    count = 0;
    old = 0;
    printf("52 bit k-distribution\n");
    for (bit = 1; bit <= 52; bit++) {
	shortest = get_equiv_distrib64(bit, &sfmt);
	dist_sum += mexp / bit - shortest;
	if (old == shortest) {
	    count++;
	} else {
	    old = shortest;
	}
	//printf("k(%d) = %d, %d, %d\n", bit, shortest, dist_sum, count);
	printf("k(%d) = %d\n", bit, shortest);
	fflush(stdout);
    }
    printf("52bit D.D:%7d, DUP:%5d\n", dist_sum, count);
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
