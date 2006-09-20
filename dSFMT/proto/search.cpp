/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <limits.h>
#include <errno.h>
#include "util.h"
#include "dsfmt-util.h"

extern "C" {
  #include "dsfmt-st.h"
  #include "mt19937ar.h"
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
    unsigned int mexp;
  
    maxdegree = get_rnd_maxdegree();
    mexp = get_rnd_mexp();
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
		    genrand_int32(),
		    genrand_int32(),
		    genrand_int32(),
		    genrand_int32(),
		    genrand_int32());
	init_gen_rand(&dsfmt, genrand_int32()+3);
	bmOk = 1;
	//    for (j = 0; j < 32; j++) {
	for (j = 0; j < 2; j++) {
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
	}
    }
    printf("count = %llu\n", all_count);
    fflush(stdout);
}


int main(int argc, char* argv[]){
    int n;
    unsigned long seed;

    setup_param(1, 0, 21, 4, 3, 29, 2, 2, 2, 0, 0, 0, 0);

    if (argc != 2) {
	//limit = 32;
	n = 1;
    } else {
	//limit = atoi(argv[1]);
	n = atoi(argv[1]);
    }

    printf("MEXP = %d\n", get_rnd_mexp());
    seed = (long)time(NULL);
    printf("seed = %lu\n", seed);
    init_genrand(seed);
    //printf("search limit degree = %d\n", limit);
    printf("now search %d times\n", n);
    fflush(stdout);
    search(n);
    return 0;
}
