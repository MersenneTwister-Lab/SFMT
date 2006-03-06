#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "sfmt-st.h"

int main(int argc, char *argv[]) {
    sfmt_t sfmt;
    uint32_t seed;
    int i;

    if (argc == 1) {
	seed = 1234;
    } else {
	seed = strtol(argv[1], NULL, 10);
    }

    init_gen_rand(&sfmt, seed);
    for (i = 0; i < 1000; i++) {
	printf("%10u ", gen_rand32(&sfmt));
	if (i % 5 == 4) {
	    printf("\n");
	}
    }
    return 0;
}
