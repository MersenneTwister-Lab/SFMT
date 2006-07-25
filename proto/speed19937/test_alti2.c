/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "sfmt.h"

static inline void lshift128(uint32_t out[4], const uint32_t in[4],
			     int shift) {
    uint64_t t1, t2, o1, o2;

    t1 = ((uint64_t)in[1] << 32) | ((uint64_t)in[0]);
    t2 = ((uint64_t)in[3] << 32) | ((uint64_t)in[2]);

    o1 = t1 << (shift * 8);
    o2 = t2 << (shift * 8);
    o2 |= t1 >> (64 - shift * 8);
    out[1] = (uint32_t)(o1 >> 32);
    out[0] = (uint32_t)o1;
    out[3] = (uint32_t)(o2 >> 32);
    out[2] = (uint32_t)o2;
}

int main(void) {
    uint32_t x[4];
    vector unsigned int a = (vector unsigned int)
	(0xa1a2a3a4,0xb1b2b3b4,0xc1c2c3c4,0xd1d2d3d4);
    unsigned int *ap = (unsigned int *)&a;
    vector unsigned int b;
    unsigned int *bp = (unsigned int *)&b;
    vector unsigned char perm = (vector unsigned char)
	(1, 2, 3, 23, 5, 6, 7, 0, 9, 10, 11, 4, 13, 14, 15, 8);

    int i;

    lshift128(x, ap, 1);
    b = vec_perm(a, perm, perm);
    for (i = 0; i < 4; i++) {
	printf("%08x ", x[i]);
    }
    printf("\n");
    printf("%08vlx\n", b);
    return 0;
}
