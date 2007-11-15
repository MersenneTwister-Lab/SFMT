/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "sfmt.h"

#define INLINE inline

INLINE static void rshift128(uint32_t out[4], const uint32_t in[4],
			     int shift) {
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in[3] << 32) | ((uint64_t)in[2]);
    tl = ((uint64_t)in[1] << 32) | ((uint64_t)in[0]);

    oh = th >> (shift * 8);
    ol = tl >> (shift * 8);
    ol |= th << (64 - shift * 8);
    out[1] = (uint32_t)(ol >> 32);
    out[0] = (uint32_t)ol;
    out[3] = (uint32_t)(oh >> 32);
    out[2] = (uint32_t)oh;
}

INLINE static void lshift128(uint32_t out[4], const uint32_t in[4],
			     int shift) {
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in[3] << 32) | ((uint64_t)in[2]);
    tl = ((uint64_t)in[1] << 32) | ((uint64_t)in[0]);

    oh = th << (shift * 8);
    ol = tl << (shift * 8);
    oh |= tl >> (64 - shift * 8);
    out[1] = (uint32_t)(ol >> 32);
    out[0] = (uint32_t)ol;
    out[3] = (uint32_t)(oh >> 32);
    out[2] = (uint32_t)oh;
}

static inline void old_rshift128(uint32_t out[4], const uint32_t in[4],
			     int shift) {
    uint64_t t1, t2, o1, o2;

    t1 = ((uint64_t)in[1] << 32) | ((uint64_t)in[0]);
    t2 = ((uint64_t)in[3] << 32) | ((uint64_t)in[2]);

    o1 = t1 >> (shift * 8);
    o1 |= t2 << (64 - shift * 8);
    o2 = t2 >> (shift * 8);
    out[1] = (uint32_t)(o1 >> 32);
    out[0] = (uint32_t)o1;
    out[3] = (uint32_t)(o2 >> 32);
    out[2] = (uint32_t)o2;
}

static inline void old_lshift128(uint32_t out[4], const uint32_t in[4],
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
    vector unsigned char rperm = (vector unsigned char)
	(7, 0, 1, 2, 11, 4, 5, 6, 3, 8, 9, 10, 17, 12, 13, 14);

    int i;

    lshift128(x, ap, 1);
    for (i = 0; i < 4; i++) {
	printf("%08x ", x[i]);
    }
    printf("\n");
    old_lshift128(x, ap, 1);
    for (i = 0; i < 4; i++) {
	printf("%08x ", x[i]);
    }
    printf("\n");


    rshift128(x, ap, 1);
    for (i = 0; i < 4; i++) {
	printf("%08x ", x[i]);
    }
    printf("\n");
    old_rshift128(x, ap, 1);
    for (i = 0; i < 4; i++) {
	printf("%08x ", x[i]);
    }
    printf("\n");
    return 0;
}
