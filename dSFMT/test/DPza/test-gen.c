/* 
 * generate check for C and param file.
 */
#include <stdio.h>
#include "dSFMT-params.h"

static void test_gen(void);

int main(int argc, char *argv[]) {
    printf("mexp = %d\n", DSFMT_MEXP);
    test_gen();
    return 0;
}

static void test_gen() {
    int i, j;

    dsfmt_gv_init_gen_rand(1);
    for (i = 0; i < 1000; i++) {
	printf("%1.15f ", dsfmt_gv_genrand_close1_open2());
	if (i % 4 == 3) {
	    printf("\n");
	}
    }
    for (i = 1; i < 100; i++) {
	dsfmt_gv_init_gen_rand(i + 1);
	printf("seed = %d\n", i);
	for (j = 0; j < 12; j++) {
	    printf("%1.15f ", dsfmt_gv_genrand_close1_open2());
	    if (j % 4 == 3) {
		printf("\n");
	    }
	}
    }
}
