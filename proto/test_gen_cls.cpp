#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "sfmt-cls.h"

int main(int argc, char *argv[]) {
    uint32_t seed;
    int i;
    SFMT sfmt;

    if (argc == 1) {
	seed = 1234;
    } else {
	seed = strtol(argv[1], NULL, 10);
    }

    sfmt.reseed(seed);
    for (i = 0; i < 1000; i++) {
	printf("%10u ", sfmt.gen_rand32());
	if (i % 5 == 4) {
	    printf("\n");
	}
    }
    return 0;
}
