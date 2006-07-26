#include "c99.h"
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <string.h>

#define QUOTE(str) QUOTE_HELPER(str)
#define QUOTE_HELPER(str) # str
//#include QUOTE(RANDOM)

#if defined(__ppc__)
static INLINE __attribute__((always_inline)) uint64_t get_clock(void) {
    register uint32_t tbu, tbl, tmp;

    __asm__ volatile(
        "0:\n\t"
        "mftbu %0\n\t"
        "mftb %1\n\t"
        "mftbu %2\n\t"
        "cmpw %0, %2\n\t"
        "bne- 0b"
        : "=r"(tbu), "=r"(tbl), "=r"(tmp)
        : /* nope */
        : "cc");

    return (((uint64_t)tbu)<<32) + tbl;
}
#define TIC_MAG 1
#define TIC_COUNT 1
#elif defined(__GNUC__)
static INLINE __attribute__((always_inline)) uint64_t get_clock() {
     uint64_t x;
     __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
     return x;
}
#define TIC_MAG 100
#define TIC_COUNT 1
#else
#if 1
#include <ia32intrin.h>
static __int64 get_clock() {
  return _rdtsc();
}
#define TIC_MAG 100
#define TIC_COUNT 1
#endif
#if 0
#include <time.h>
static unsigned long long get_clock() {
  return (unsigned long long)clock();
}
#define TIC_MAG 1
#define TIC_COUNT 1000
#endif
#endif

#define KAISU 10000
#if defined(__GNUC__) && defined(__ppc__)
//uint32_t dummy[KAISU/4+1] __attribute__((aligned(16)));
vector unsigned int dummy[KAISU/4+1];
#else
//__declspec(align(16)) uint32_t dummy[KAISU/4+1];
__m128i dummy[KAISU/4+1];
#endif

int main(int argc, char *argv[]) {
    uint32_t i, j;
    uint64_t clo;
    unsigned long long min = LONG_MAX;
    uint32_t block;
    uint32_t randoms;
    bool verbose = false;
    uint32_t *array = (uint32_t *)dummy;

    if ((argc >= 2) && (strncmp(argv[1],"-v",2) == 0)) {
	verbose = true;
    }
    block = get_onetime_rnds();
    randoms = (KAISU / block) * block;
    init_gen_rand(1234);
    if (verbose) {
	//printf("init states\n");
	//print_state(stdout);
	printf("generated randoms\n");
	fill_array_block(array, 1000 / block + 1);
	for (i = 0; i < 1000; i++) {
	    printf("%10lu ", array[i]);
	    if (i % 5 == 4) {
		printf("\n");
	    }
	}
    }
    for (i = 0; i < 100; i++) {
	clo = get_clock();
	for (j = 0; j < TIC_COUNT; j++) {
	    fill_array_block(array, randoms / block);
	}
	clo = get_clock() - clo;
	if (clo < min) {
	    min = clo;
	}
    }
    if (min != 0) {
      printf("%llu", min);
      printf(" tick and %.0f randoms = %04.2f randoms per %dtick\n",
	       (double)randoms * TIC_COUNT,
	       (double)randoms * TIC_MAG * TIC_COUNT / min, TIC_MAG);
    } else {
	printf("can't count time too fast\n");
    }
    min = LONG_MAX;
    for (i = 0; i < 100; i++) {
	clo = get_clock();
	for (j = 0; j < randoms * TIC_COUNT; j++) {
	    gen_rand();
	}
	clo = get_clock() - clo;
	if (clo < min) {
	    min = clo;
	}
    }
    if (min != 0) {
      printf("%llu", min);
      printf(" tick and %.0f randoms = %04.2f randoms per %dtick\n",
	       (double)randoms * TIC_COUNT,
	       (double)randoms * TIC_MAG * TIC_COUNT / min, TIC_MAG);
    } else {
	printf("can't count time too fast\n");
    }
    return 0;
}
