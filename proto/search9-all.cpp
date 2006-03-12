/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <limits.h>
#include <errno.h>
#include "util.h"
#include "shortbase9.h"

extern "C" {
#include "sfmt9-st.h"
#include "mt19937ar.h"
}

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>

NTL_CLIENT;

int non_reducible(GF2X& fpoly, int degree);
void search(unsigned int n);
int test_shortest(GF2X& poly);

static unsigned long long all_count = 0;
static unsigned int maxdegree;
static unsigned int mexp;
static FILE *frandom;

void generating_polynomial(sfmt9_t *sfmt, vec_GF2& vec, unsigned int bitpos, 
			   unsigned int maxdegree)
{
    unsigned int i;
    uint32_t mask = 1UL << bitpos;

    i = 0;
    clear(vec);
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	if ((gen_rand(sfmt) & mask) == mask){
	    vec[i] = 1;
	}
    }
}

void search(unsigned int n) {
    int j;
    unsigned int succ = 0;
    int bmOk;
    GF2X minpoly;
    vec_GF2 vec;
    sfmt9_t sfmt;
  
    vec.FixLength(2 * maxdegree);

    for (;;) {
	setup_param(genrand_int32(),
		    genrand_int32(),
		    genrand_int32(),
		    genrand_int32(),
		    genrand_int32(),
		    genrand_int32(),
		    genrand_int32(),
		    genrand_int32(),
		    genrand_int32());
	init_gen_rand(&sfmt, genrand_int32()+3);
	//print_param2(stdout);
	bmOk = 1;
	//    for (j = 0; j < 32; j++) {
	for (j = 0; j < 1; j++) {
	    generating_polynomial(&sfmt, vec, j, maxdegree);
	    berlekampMassey(minpoly, maxdegree, vec);
	    if (deg(minpoly) == -1) {
		bmOk = 0;
		break;
	    }
	    all_count++;
	    if (!non_reducible(minpoly, mexp)) {
		bmOk = 0;
		break;
	    }
	}
	if (bmOk) {
	    printf("----------\n");
	    printf("succ = %u\n", ++succ);
	    printf("deg = %ld\n", deg(minpoly));
	    print_param(stdout);
	    printBinary(stdout, minpoly);
	    test_shortest(minpoly);
	    fflush(stdout);
	    if (succ >= n) {
		break;
	    }
	}
	if (all_count % 10000 == 0) {
	    printf("count = %llu\n", all_count);
	    fflush(stdout);
	}
    }
    printf("count = %llu\n", all_count);
    fflush(stdout);
}

int get_equiv_distrib(int bit, sfmt9_t *sfmt) {
    static sfmt9_t sfmtnew;
    int shortest;

    //fprintf(stderr, "now start get_equiv %d\n", bit);
    sfmtnew = *sfmt;
    set_bit_len(bit);
    shortest = get_shortest_base(bit, &sfmtnew);
    return shortest;
}

void make_zero_state(sfmt9_t *sfmt, GF2X& poly) {
    static sfmt9_t sfmtnew;
    int i;

    memset(&sfmtnew, 0, sizeof(sfmtnew));
    for (i = 0; i <= deg(poly); i++) {
	if (coeff(poly, i) != 0) {
	    add_state(&sfmtnew, sfmt);
	}
	next_state(sfmt);
    }
    *sfmt = sfmtnew;
}

int test_shortest(GF2X& poly) {
    unsigned int bit;
    GF2X lcmpoly;
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    sfmt9_t sfmt;
    sfmt9_t sfmt_save;
    uint32_t shortest;
    int i;
    int dist_sum;
    int count;
    uint32_t old;
    vec_GF2 vec;
    int lcmcount;

    vec.SetLength(2 * maxdegree);
    init_gen_rand(&sfmt, 12345678901ULL);
    sfmt_save = sfmt;
    generating_polynomial(&sfmt, vec, 0, maxdegree);
    berlekampMassey(lcmpoly, maxdegree, vec);
    for (i = 1; i < 32; i++) {
	generating_polynomial(&sfmt, vec, i, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
    }
    lcmcount = 0;
    while (deg(lcmpoly) < (long)maxdegree) {
	if (lcmcount > 1000) {
	    printf("failure\n");
	    return -1;
	}
	errno = 0;
	fread(sfmt.sfmt, sizeof(uint32_t), N, frandom);
	if (errno) {
	    perror("set_bit");
	    fclose(frandom);
	    exit(1);
	}
	for (int j = 0; j < 32; j++) {
	    generating_polynomial(&sfmt, vec, j, maxdegree);
	    berlekampMassey(minpoly, maxdegree, vec);
	    LCM(tmp, lcmpoly, minpoly);
	    lcmpoly = tmp;
	    lcmcount++;
	}
    }
    printf("lcm:\n");
    printBinary(stdout, lcmpoly);
    printf("weight = %ld\n", weight(lcmpoly));
    DivRem(tmp, rempoly, lcmpoly, poly);
    printf("deg tmp = %ld\n", deg(tmp));
    if (deg(rempoly) != -1) {
	printf("rem != 0 deg rempoly = %ld\n", deg(rempoly));
	return -1;
    }
    sfmt = sfmt_save;
    make_zero_state(&sfmt, tmp);
    dist_sum = 0;
    count = 0;
    old = 0;
    for (bit = 1; bit <= 32; bit++) {
	shortest = get_equiv_distrib(bit, &sfmt);
	if (shortest > mexp) {
	    printf("k(%d) = %d\n", bit, shortest);
	    printf("distribution greater than mexp!\n");
	    return -1;
	}
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
    printf("D.D:%7d, DUP:%5d\n", dist_sum, count);
    fflush(stdout);
    return 0;
}

int main(int argc, char* argv[]){
    int n;
    unsigned long seed;

    setup_param(1, 0, 21, 4, 3, 29, 2, 2, 2);

    if (argc != 2) {
	n = 1;
    } else {
	n = atoi(argv[1]);
    }

    maxdegree = get_rnd_maxdegree();
    mexp = get_rnd_mexp();
    printf("MEXP = %d\n", mexp);
    seed = (long)time(NULL);
    printf("seed = %lu\n", seed);
    init_genrand(seed);
    printf("now search %d times\n", n);
    fflush(stdout);
    frandom = fopen("/dev/random", "r");
    if ((frandom == NULL) || errno) {
	perror("main");
	fclose(frandom);
	exit(1);
    }
    search(n);
    fclose(frandom);
    return 0;
}
