#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "dsfmt-st.h"

#define NUM_RANDS 50000
#define TIC_MAG 1
#define TIC_COUNT 2000

void check(void);
void check(void) {
    int i, j, k;
    uint64_t ar[1000][2];
    uint64_t br[2];
    dsfmt_t sfmt;

    printf("check 104sp and 104spar\n");
    for (k = 0; k < 10; k++) {
	init_gen_rand(&sfmt, 1234 + k);
	gen_rand104spar(&sfmt, ar, 1000);
	init_gen_rand(&sfmt, 1234 + k);
	for (i = 0; i < 1000; i++) {
	    gen_rand104sp(&sfmt, br, 0);
	    for (j = 0; j < 2; j++) {
		if (br[j] != ar[i][j]) {
		    printf("diff %d:br %016llx ar%016llx\n",
			   i, br[j], ar[i][j]);
		    return;
		}
	    }
	}
    }
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
    printf("pcv1 %016llx\n", pcv[0]);
    printf("pcv2 %016llx\n", pcv[1]);
    read_random_param(fp);
    print_param(stdout);
    set_pcv(pcv);
    check();
    return 0;
}
