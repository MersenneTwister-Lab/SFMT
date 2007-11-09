/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <limits.h>
#include <errno.h>
#include "util.h"
#include "dsfmtN-util.h"

extern "C" {
  #include "dsfmtN-st.h"
  #include "mt19937blk.h"
}

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>

NTL_CLIENT;

void search(unsigned int n);

static unsigned long long all_count = 0;
int limit;

void search(unsigned int n) {
    int j;
    unsigned int succ = 0;
    int bmOk;
    dsfmt_t dsfmt;
    GF2X minpoly;
    vec_GF2 vec;
    unsigned int maxdegree;
    int mexp;
    const int rndarray_cnt = 100000;
    int rndarray_idx = 0;
    static uint32_t rndarray[rndarray_cnt];
  
    maxdegree = get_rnd_maxdegree();
    mexp = get_rnd_mexp();
    vec.FixLength(2 * maxdegree);
    mt_fill(rndarray, rndarray_cnt);
    for (;;) {
	setup_param(rndarray, &rndarray_idx);
	init_gen_rand(&dsfmt, rndarray[rndarray_idx++]);
	if (rndarray_idx + 100 > rndarray_cnt) {
	    mt_fill(rndarray, rndarray_cnt);
	    rndarray_idx = 0;
	}
	bmOk = 1;
	//    for (j = 0; j < 32; j++) {
	for (j = 0; j < 1; j++) {
	    generating_polynomial104(&dsfmt, vec, j, maxdegree);
	    berlekampMassey(minpoly, maxdegree, vec);
	    if (deg(minpoly) == -1) {
		bmOk = 0;
		if (j == 1) {
		    printf("failure 1\n");
		    fflush(stdout);
		}
		break;
	    }
	    all_count++;
	    if (deg(minpoly) < mexp) {
		bmOk = 0;
		break;
	    }
	    if (!non_reducible(minpoly, mexp)) {
		bmOk = 0;
		if (j == 1) {
		    printf("failure 2 deg = %ld\n", deg(minpoly));
		    fflush(stdout);
		}
		break;
	    } else {
		//printBinary(stdout, minpoly);
		//fflush(stdout);
	    }
	}
	if (bmOk) {
	    printf("----------\n");
	    printf("succ = %u\n", ++succ);
	    printf("deg = %ld\n", deg(minpoly));
	    print_param(stdout);
	    printBinary(stdout, minpoly);
	    fflush(stdout);
	    if (succ >= n) {
		break;
	    }
	}
	if (all_count % 10000 == 0) {
	    printf("count = %llu\n", all_count);
	    fflush(stdout);
	    if ((all_count % 50000) == 0 && (n < 10)) {
		break;
	    }
	}
    }
    printf("count = %llu\n", all_count);
    fflush(stdout);
}


int main(int argc, char* argv[]){
    int n;
    unsigned long seed;

    if (argc != 2) {
	//limit = 32;
	n = 1;
    } else {
	//limit = atoi(argv[1]);
	n = atoi(argv[1]);
    }

    printf("MEXP = %d, MAXDEGREE = %d, N = %d\n",
	   get_rnd_mexp(), get_rnd_maxdegree(), N);
    seed = (long)time(NULL);
    printf("seed = %lu\n", seed);
    mt_init(seed);
    //printf("search limit degree = %d\n", limit);
    printf("now search %d times\n", n);
    fflush(stdout);
    search(n);
    return 0;
}
