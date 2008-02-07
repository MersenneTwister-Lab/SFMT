#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

static void lshift128(uint64_t out[2], const uint64_t in[2], int shift) {
    out[0] = in[0] << (shift * 8);
    out[1] = in[1] << (shift * 8);
    out[1] |= in[0] >> (64 - shift * 8);
}

int main(void) {
    uint64_t x[2];
    vector unsigned int a = (vector unsigned int)
	(0xa1a2a3a4,0xb1b2b3b4,0xc1c2c3c4,0xd1d2d3d4);
    uint64_t *ap = (uint64_t *)&a;
    vector unsigned int b;
    uint64_t *bp = (uint64_t *)&b;
    vector unsigned char sl2_perm = (vector unsigned char)
	(5, 6, 7, 27, 27, 27, 27, 27, 13, 14, 15, 0, 1, 2, 3, 4);
    int i;

    lshift128(x, ap, 5);
    b = vec_perm(a, sl2_perm, sl2_perm);
    for (i = 0; i < 2; i++) {
	printf("%016llx ", x[i]);
    }
    printf("\n");
    printf("%08vlx\n", b);
    for (i = 0; i < 2; i++) {
	printf("%016llx ", bp[i]);
    }
    printf("\n");
    for (i = 0; i < 2; i++) {
	printf("%016llx ", ap[i]);
    }
    printf("\n");

    return 0;
}
