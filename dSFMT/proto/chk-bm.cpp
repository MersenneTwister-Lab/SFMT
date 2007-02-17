#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>

extern "C" {
#include "dsfmt-afst.h"
}
#include "shortbase128.h"
#include "util.h"
#include "dsfmt-util.h"
#include <unistd.h>


NTL_CLIENT;

static int mexp;
static int maxdegree;
static FILE *frandom;

static char* filename = NULL;


void set_bit(dsfmt_t *sfmt, int bit_pos, int mode) {
  int i, j, k;
  uint64_t mask;
  uint64_t high;

  if (mode == 0) {
      high = 0;
  } else {
      high = 0xBFF0000000000000ULL;
  }

  k = bit_pos % 52;
  j = (bit_pos / 52) % 2;
  i = bit_pos / (52 * 2);
  memset(sfmt, 0, sizeof(dsfmt_t));
  mask = 1UL << k;
  sfmt->status[i][j] = mask;
  for (i = 0; i < N + 1; i++) {
      for (j = 0; j < 2; j++) {
	  sfmt->status[i][j] |= high;
      }
  }
}

void fill_state_random(dsfmt_t *sfmt, FILE *frandom, int k) {
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

void test_bm(GF2X& lcmpoly, char *filename, int mode) {
    FILE *fp;
    GF2X poly;
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    dsfmt_t sfmt;
    dsfmt_t sfmt_save;
    vec_GF2 vec;
    int i, max;

    max = 128 * (N + 1);
    printf("filename:%s\n", filename);
    fp = fopen(filename, "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("main");
	fclose(fp);
	exit(1);
    }
    read_random_param(fp);
    init_gen_rand(&sfmt, 123, mode);
    sfmt_save = sfmt;
    //print_param(stdout);
    readFile(poly, fp);
    //printf("deg poly = %ld\n", deg(poly));
    fclose(fp);
    //printBinary(stdout, poly);
    vec.SetLength(2 * (max));
    generating_polynomial104(&sfmt, vec, 0, max);
    berlekampMassey(lcmpoly, max, vec);
    for (i = 1; i < 104; i++) {
	generating_polynomial104(&sfmt, vec, i, max);
	berlekampMassey(minpoly, max, vec);
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
    }
    //while (deg(lcmpoly) < (long)maxdegree) {
    for (i = 1; i < maxdegree; i++) {
	//fill_state_random(&sfmt, frandom);
	set_bit(&sfmt, i, mode);
	for (int j = 0; j < 104; j++) {
	    generating_polynomial104(&sfmt, vec, j, max);
	    if (IsZero(vec)) {
		break;
	    }
	    berlekampMassey(minpoly, max, vec);
	    LCM(tmp, lcmpoly, minpoly);
	    lcmpoly = tmp;
	}
    }
    //printf("deg lcm poly = %ld\n", deg(lcmpoly));
    //printf("weight = %ld\n", weight(lcmpoly));
    //printBinary(stdout, lcmpoly);
    DivRem(tmp, rempoly, lcmpoly, poly);
    //printf("deg tmp = %ld\n", deg(tmp));
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld\n", deg(rempoly));
	exit(1);
    }
}

int main(int argc, char *argv[]) {
    GF2X lcm1;
    GF2X lcm2;
    GF2X tmp;
    GF2X rem;

    if (argc >= 2) {
	filename = argv[1];
    } else {
	printf("%s filename\n", argv[0]);
	return 1;
    }
    mexp = get_rnd_mexp();
    maxdegree = get_rnd_maxdegree();
    printf("mexp = %d, maxdegree = %d\n", mexp, maxdegree);
    frandom = fopen("/dev/urandom", "r");
    if (errno) {
	perror("main");
	exit(1);
    }
    test_bm(lcm1, filename, 0);
    test_bm(lcm2, filename, 1);
    fclose(frandom);
    printf("deg linear = %ld, deg affin = %ld\n", deg(lcm1), deg(lcm2));
    if (lcm1 == lcm2) {
	printf("lcm1 and lcm2 is same.\n");
    } else if (deg(lcm1) >= deg(lcm2)) {
	DivRem(tmp, rem, lcm1, lcm2);
	if (!IsZero(rem)) {
	    printf("rem is not zero! deg = %ld\n", deg(rem));
	}
	printf("linear / affil = \n");
	printBinary(stdout, tmp);
    } else {
	DivRem(tmp, rem, lcm2, lcm1);
	if (!IsZero(rem)) {
	    printf("rem is not zero! deg = %ld\n", deg(rem));
	}
	printf("affin / linear = \n");
	printBinary(stdout, tmp);
    }
    return 0;
}
