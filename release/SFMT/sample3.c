#include <stdio.h>
#include <stdlib.h>
#include "SFMT.h"

int main(int argc, char* argv[]) {
    int i, cnt, seed;
    double x, y, pi;
    const int NUM = 10000;
    sfmt_t sfmt;

    if (argc >= 2) {
	seed = strtol(argv[1], NULL, 10);
    } else {
	seed = 12345;
    }
    cnt = 0;
    sfmt_init_gen_rand(&sfmt, seed);
    for (i = 0; i < NUM; i++) {
	x = sfmt_genrand_res53(&sfmt);
	y = sfmt_genrand_res53(&sfmt);
	if (x * x + y * y < 1.0) {
	    cnt++;
	}
    }
    pi = (double)cnt / NUM * 4;
    printf("%lf\n", pi);
    return 0;
}
