/**
 * @file sample2.c
 *
 * @brief sample jump for one sequence using 6 generators.
 *
 * The jump polynomial used in this program was calculated like this:
<pre>
$ ./calc-jump 10000 characteristic.607.txt
jump polynomial:
6e31cdd80d4be73c65312170814807aa0b1c82a32128fa24db888ef4f867786b46a1a04b8c5c2fd7
344253605bdaddf72a6f5104ce98000ccd4d80bbe19cf4d69b3d3cbdc90c3dbbc02d95bbc10fb886
</pre>
*/
/*
 * In this sample, jump polynomial is fixed, then SFMT_MEXP should be fixed.
 */
#define SFMT_MEXP 607
#include <stdio.h>
#include "SFMT.h"
#include "SFMT-jump.h"

/*
 * jump step is SFMT internal state step,
 * for 32-bit integer, 4 * jump step numbers are skipped.
 */
static const int step = 4 * 2000;
static const char * jump2000 = "3f0571527f177195bbff0dea5f07e3e4ea6240528b70c"
    "a81f989a85eedc0e376737e009289a656b6cbf9552f9a7389a71b5ff2757e3b13e9bc7b7"
    "61bd9d30e2c89f69ac8638db6d9a4540c9999f2f458";
static const char * jump10000 = "6e31cdd80d4be73c65312170814807aa0b1c82a32128"
    "fa24db888ef4f867786b46a1a04b8c5c2fd7344253605bdaddf72a6f5104ce98000ccd4d"
    "80bbe19cf4d69b3d3cbdc90c3dbbc02d95bbc10fb886";

int main(int argc, char * argv[])
{
    uint32_t seed = 1234;
    sfmt_t sfmt[6];
    sfmt_t serial;
    uint32_t array[6 * step];

    /* set up initial position of six sfmt instances
     * sfmt[0]: position 0
     * sfmt[1]: position 2000
     * sfmt[2]: position 4000
     * sfmt[3]: position 6000
     * sfmt[4]: position 8000
     * sfmt[5]: position 10000
     */
    sfmt_init_gen_rand(&sfmt[0], seed);
    for (int i = 1; i < 6; i++) {
	sfmt[i] = sfmt[i - 1];
	SFMT_jump(&sfmt[i], jump2000);
    }
    /* initialize serial generator */
    sfmt_init_gen_rand(&serial, seed);
    /* generate and compare with serial generation */
    for (int i = 0; i < 10; i++) {
	/* simulating parallel generation */
	for (int j = 0; j < step; j++) {
	    array[j + 0 * step] = sfmt_genrand_uint32(&sfmt[0]);
	    array[j + 1 * step] = sfmt_genrand_uint32(&sfmt[1]);
	    array[j + 2 * step] = sfmt_genrand_uint32(&sfmt[2]);
	    array[j + 3 * step] = sfmt_genrand_uint32(&sfmt[3]);
	    array[j + 4 * step] = sfmt_genrand_uint32(&sfmt[4]);
	    array[j + 5 * step] = sfmt_genrand_uint32(&sfmt[5]);
	}
	/* checking */
	for (int j = 0; j < 6 * step; j++) {
	    if (sfmt_genrand_uint32(&serial) != array[j]) {
		printf("something wrong! i=%d, j=%d\n", i, j);
		return -1;
	    }
	}
	/* simulating parallel jump */
	SFMT_jump(&sfmt[0], jump10000);
	SFMT_jump(&sfmt[1], jump10000);
	SFMT_jump(&sfmt[2], jump10000);
	SFMT_jump(&sfmt[3], jump10000);
	SFMT_jump(&sfmt[4], jump10000);
	SFMT_jump(&sfmt[5], jump10000);
    }
    printf("OK\n");
    return 0;
}
