#include <stdio.h>
#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include "sfmt19937.h"

int main(int argc, char* argv[]) {
    int i, j, cnt, seed;
    double x, y, pi;
    const int NUM = 10000;
    const int R_SIZE = 2 * NUM;
    uint64_t *array;

    if (argc >= 2) {
	seed = strtol(argv[1], NULL, 10);
    } else {
	seed = 12345;
    }
#ifdef __APPLE__
    array = malloc(sizeof(uint64_t) * R_SIZE);
    if (array == NULL) {
	printf("can't allocate memory.\n");
	return 1;
    }
#else
    if (posix_memalign((void **)&array, 16, sizeof(uint64_t) * R_SIZE) != 0) {
	printf("can't allocate memory.\n");
	return 1;
    }
#endif
    cnt = 0;
    j = 0;
    init_gen_rand(seed);
    fill_array64(array, R_SIZE);
    for (i = 0; i < NUM; i++) {
	x = to_res53(array[j++]);
	y = to_res53(array[j++]);
	if (x * x + y * y < 1.0) {
	    cnt++;
	}
    }
    free(array);
    pi = (double)cnt / NUM * 4;
    printf("%lf\n", pi);
    return 0;
}
