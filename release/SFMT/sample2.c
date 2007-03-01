#include <stdio.h>
#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include "SFMT.h"

int main(int argc, char* argv[]) {
    int i, j, cnt, seed;
    double x, y, pi;
    const int NUM = 10000;
    const int R_SIZE = 2 * NUM;
    int size;
    uint64_t *array;

    if (argc >= 2) {
	seed = strtol(argv[1], NULL, 10);
    } else {
	seed = 12345;
    }
    size = get_min_array_size64();
    if (size < R_SIZE) {
	size = R_SIZE;
    }
#if defined(__APPLE__)
    printf("malloc used\n");
    array = malloc(sizeof(double) * size);
    if (array == NULL) {
	printf("can't allocate memory.\n");
	return 1;
    }
#elif defined(_POSIX_C_SOURCE)
    printf("posix_memalign used\n");
    if (posix_memalign((void **)&array, 16, sizeof(double) * size) != 0) {
	printf("can't allocate memory.\n");
	return 1;
    }
#elif defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3))
    printf("memalign used\n");
    array = memalign(16, sizeof(double) * size);
    if (array == NULL) {
	printf("can't allocate memory.\n");
	return 1;
    }
#else /* in this case, gcc doesn't suppport SSE2 */
    printf("malloc used\n");
    array = malloc(sizeof(double) * size);
    if (array == NULL) {
	printf("can't allocate memory.\n");
	return 1;
    }
#endif
    cnt = 0;
    j = 0;
    init_gen_rand(seed);
    fill_array64(array, size);
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
