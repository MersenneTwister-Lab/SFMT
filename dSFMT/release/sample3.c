#include <stdio.h>
#include <stdlib.h>
#include "dSFMT.c"

int main(int argc, char* argv[]) {
    int i, cnt, seed;
    double x, y, pi;
    const int NUM = 10000;

    if (argc >= 2) {
	seed = strtol(argv[1], NULL, 10);
    } else {
	seed = 12345;
    }
    cnt = 0;
    init_gen_rand(seed);
    for (i = 0; i < NUM; i++) {
	x = genrand_close_open();
	y = genrand_close_open();
	if (x * x + y * y < 1.0) {
	    cnt++;
	}
    }
    pi = (double)cnt / NUM * 4;
    printf("%lf\n", pi);
    return 0;
}
