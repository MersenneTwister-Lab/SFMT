#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include "dsfmt-ref-st.h"

#define NUM_RANDS 50000
#define TIC_MAG 1
#define TIC_COUNT 2000

void check_co(void);
void check_co(void) {
    int i;
    union W64_T {
	uint64_t u;
	double d;
    };
    union W64_T r;
    dsfmt_t sfmt;

    printf("%s\n", get_idstring());
    printf("generated randoms [1,2)\n");
    init_gen_rand(&sfmt, 1234);
#if 0
    printf("after initialize\n");
    for (i = 0; i < 6; i++) {
	printf("%016llx ", sfmt.sfmt[i].u[0]);
	printf("%016llx\n", sfmt.sfmt[i].u[1]);
    }
#endif
    for (i = 0; i < 1000; i++) {
	r.d = genrand_close1_open2(&sfmt);
	printf("%1.20lf ", r.d);
	if (i % 3 == 2) {
	    printf("\n");
	}
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    //print_param(stdout);
    check_co();
    return 0;
}
