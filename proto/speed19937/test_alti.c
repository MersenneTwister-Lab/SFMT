/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "sfmt.h"

int main(void) {
    vector unsigned int a = (vector unsigned int)
	(0xffffffff,0xffffffff,0xffffffff,0xffffffff);
    unsigned int *ap = (unsigned int *)&a;
    int i;

    a = vec_sro(a, (vector unsigned char)(4 << 3));
    //a = vec_sro(a, (vector unsigned char)(4));
    printf("%vlx\n", a);
    for (i = 0; i < 4; i++) {
	printf("%x ", ap[i]);
    }
    printf("\n");
    return 0;
}
