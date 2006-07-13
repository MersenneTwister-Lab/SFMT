#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>

#include "util.h"
#include "sfmt-st.h"
#include "sfmt-util.h"

NTL_CLIENT;

void fill_state_random(sfmt_t *sfmt, FILE *frandom) {
    int i, j;
    int w;

    for (i = 0; i < N; i++) {
	for (j = 0; j < 4; j++) {
	    w = getw(frandom);
	    if (feof(frandom) || ferror(frandom)) {
		if (errno != 0) {
		    printf("test_shortest:%s\n", strerror(errno));
		} else {
		    printf("test_shortest:/dev/urandom reached to EOF!\n");
		}
		fclose(frandom);
		exit(1);
	    }
	    sfmt->sfmt[i][j] = w;
	}
    }
}

static bool generating_polynomial128_hi(sfmt_t *sfmt, vec_GF2& vec,
					uint32_t bitpos, uint32_t maxdegree)
{
    unsigned int i;
    uint64_t hi, low;
    uint64_t mask;
    uint64_t bit;

    mask = (uint64_t)1ULL << (63 - bitpos);
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	gen_rand128(sfmt, &hi, &low);
	bit = (hi & mask);
	if (bit) {
	    vec[i] = 1;
	} else {
	    vec[i] = 0;
	}
    }
    return true;
}

static bool generating_polynomial128_low(sfmt_t *sfmt, vec_GF2& vec,
					 uint32_t bitpos, uint32_t maxdegree)
{
    unsigned int i;
    uint64_t hi, low;
    uint64_t mask;
    uint64_t bit;

    mask = (uint64_t)1ULL << (63 - bitpos);
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	gen_rand128(sfmt, &hi, &low);
	bit = (low & mask);
	if (bit) {
	    vec[i] = 1;
	} else {
	    vec[i] = 0;
	}
    }
    return true;
}

bool generating_polynomial128(sfmt_t *sfmt, vec_GF2& vec, unsigned int bitpos, 
			   unsigned int maxdegree)
{
    if (bitpos < 64) {
	return generating_polynomial128_hi(sfmt, vec, bitpos, maxdegree);
    } else {
	return generating_polynomial128_low(sfmt, vec, bitpos - 64, maxdegree);
    }
}

void generating_polynomial32(sfmt_t *sfmt, vec_GF2& vec,
			     unsigned int bitpos, 
			     unsigned int maxdegree)
{
    unsigned int i;
    uint32_t mask;
    uint32_t bit;

    //mask = (uint32_t)1UL << (31 - bitpos);
    mask = (uint32_t)1UL << bitpos;
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	bit = gen_rand32(sfmt) & mask;
	vec[i] = (bit != 0);
    }
}

void make_zero_state(sfmt_t *sfmt, const GF2X& poly) {
    sfmt_t sfmtnew;
    uint64_t hi, low;
    int i;

    memset(&sfmtnew, 0, sizeof(sfmtnew));
    //printf("make zero state deg(poly) = %ld\n", deg(poly));
    //cout << poly << endl;
    for (i = 0; i <= deg(poly); i++) {
	if (coeff(poly, i) != 0) {
	    add_rnd(&sfmtnew, sfmt, 0);
	}
	gen_rand128(sfmt, &hi, &low);
    }
    *sfmt = sfmtnew;
}
