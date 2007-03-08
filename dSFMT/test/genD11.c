#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "dsfmt-st.h"

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
	printf("%016llx ", sfmt.status[i][0]);
	printf("%016llx\n", sfmt.status[i][1]);
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
    FILE *fp;
    uint64_t pcv[2];

    if (argc < 4) {
	printf("usage:\n%s filename pcv1 pcv2\n", argv[0]);
	return 1;
    }
    fp = fopen(argv[1], "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("main");
	fclose(fp);
	exit(1);
    }
    pcv[0] = strtoll(argv[2], NULL, 16);
    pcv[1] = strtoll(argv[3], NULL, 16);
    //printf("pcv1 %016llx\n", pcv[0]);
    //printf("pcv2 %016llx\n", pcv[1]);
    read_random_param(fp);
    //print_param(stdout);
    set_pcv(pcv);
    check_co();
    return 0;
}
