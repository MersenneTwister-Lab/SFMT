/**
 * @file sample1.c
 *
 * @brief sample jump using pre-computed jump polynomial.
 *
 * The jump polynomial used in this program was calculated like this:
<pre>
$ ./calc-jump 100000000000000000000 characteristic.1279.txt
jump polynomial:
9d5b86a5d43ab13fe8df9176e597400e28a93d7a57e6a2c567af960cfe9918db3c5c6e61b00d546dd4296944888d4aea89f4ec6a89270b8a74e6e5dacd493bae9e64be470fef3db499c632fd0523ecb12e8156a9ce026cbe7cca3e18bf1867c71d876ef460dc1d7c44194aea55f8009d81712eaed8c58b7b1ef0e55e2a0f368ee2dd43d7ced024e565fc5b0f9e0c6561a9827fbe132b68d0e203a41ef3750573</pre>
*/
/*
 * In this sample, jump polynomial is fixed, then SFMT_MEXP should be fixed.
 */
#define SFMT_MEXP 1279
#include <stdio.h>
#include "SFMT.h"
#include "SFMT-jump.h"

/*
 * jump step is SFMT internal state step,
 * for 32-bit integer, 4 * jump step numbers are skipped.
 */
static const char * jump10_20 =
    "9d5b86a5d43ab13fe8df9176e597400e28a93d7a57e6a2c567af960cfe9918d"
    "b3c5c6e61b00d546dd4296944888d4aea89f4ec6a89270b8a74e6e5dacd493b"
    "ae9e64be470fef3db499c632fd0523ecb12e8156a9ce026cbe7cca3e18bf186"
    "7c71d876ef460dc1d7c44194aea55f8009d81712eaed8c58b7b1ef0e55e2a0f"
    "368ee2dd43d7ced024e565fc5b0f9e0c6561a9827fbe132b68d0e203a41ef37"
    "50573";
int main(int argc, char * argv[])
{
    const int size = 5;
    uint32_t seed = 4321;
    sfmt_t sfmt[size];

    sfmt_init_gen_rand(&sfmt[0], seed);
    for (int i = 1; i < size; i++) {
	sfmt[i] = sfmt[i - 1];
	SFMT_jump(&sfmt[i], jump10_20);
    }
    /* generate and compare with serial generation */
    for (int i = 0; i < 10; i++) {
	/* simulating parallel generation */
	for (int j = 0; j < size; j++) {
	    printf("%10"PRIu32" ", sfmt_genrand_uint32(&sfmt[j]));
	}
	printf("\n");
    }
    return 0;
}
