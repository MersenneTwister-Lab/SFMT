/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "sfmt.h"

int main(void) {
    vector unsigned int a = (vector unsigned int)
	(0xffffffff,0xffffffff,0xffffffff,0xffffffff);
    unsigned int *ap = (unsigned int *)&a;
    vector unsigned int b = (vector unsigned int)
	(0xaaaaaaaa,0xbbbbbbbb,0xcccccccc,0xdddddddd);
    unsigned int *bp = (unsigned int *)&b;
    vector unsigned char perm = (vector unsigned char)
	(4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3);

    int i;

    a = vec_sro(a, (vector unsigned char)(4 << 3));
    //a = vec_sro(a, (vector unsigned char)(4));
    printf("%vlx\n", a);
    for (i = 0; i < 4; i++) {
	printf("%x ", ap[i]);
    }
    printf("\n");
    printf("--------\n");
    b = vec_perm(b, b, perm);
    printf("%vlx\n", b);
    for (i = 0; i < 4; i++) {
	printf("%x ", bp[i]);
    }
    printf("\n");
    return 0;
}
