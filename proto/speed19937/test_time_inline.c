#include "c99.h"
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <string.h>

#define QUOTE(str) QUOTE_HELPER(str)
#define QUOTE_HELPER(str) # str
#include QUOTE(RANDOM)

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
#elif defined(__GNUC__)
static INLINE __attribute__((always_inline)) uint64_t get_clock() {
     uint64_t x;
     __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
     return x;
}
#define TIC_MAG 100
#else
#include <ia32intrin.h>
static __int64 get_clock() {
  return _rdtsc();
}
#define TIC_MAG 100
#endif

int main(int argc, char *argv[]) {
    uint32_t i, j;
    uint64_t clo;
    uint64_t min = UINT64_MAX;
    //double sum = 0;
    uint32_t randoms;
    bool verbose = false;

    if ((argc >= 2) && (strncmp(argv[1],"-v",2) == 0)) {
	verbose = true;
    }
    randoms = get_onetime_rnds() * 1000;
    init_gen_rand(1234);
    if (verbose) {
	printf("init states\n");
	print_state(stdout);
	printf("generated randoms\n");
	for (i = 0; i < 1000; i++) {
	    printf("%10u ", gen_rand());
	    if (i % 5 == 4) {
		printf("\n");
	    }
	}
    }
    for (i = 0; i < 100; i++) {
	clo = get_clock();
	for (j = 0; j < 1000; j++) {
	    gen_rand_all();
	}
	clo = get_clock() - clo;
	if (clo < min) {
	    min = clo;
	}
	//sum += clo;
    }
    //sum /= 1000;
    printf("%llu", min);
    printf(" tick and %u randoms = %04.2f randoms per %dtick\n",
	   randoms, (double)randoms * TIC_MAG / min, TIC_MAG);
    //printf("%f tick and %u randoms = %04.2f randoms per tick\n",
    //   sum, randoms, (double)randoms / sum);
    min = UINT64_MAX;
    //sum = 0;
    for (i = 0; i < 100; i++) {
	clo = get_clock();
	for (j = 0; j < randoms; j++) {
	    gen_rand();
	}
	clo = get_clock() - clo;
	if (clo < min) {
	    min = clo;
	}
	//sum += clo;
    }
    //sum /= 1000;
    printf("%llu", min);
    printf(" tick and %u randoms = %04.2f randoms per %dtick\n",
	   randoms, (double)randoms * TIC_MAG / min, TIC_MAG);
    //printf("%f tick and %u randoms = %04.2f randoms per tick\n",
    //   sum, randoms, (double)randoms / sum);
    return 0;
}
