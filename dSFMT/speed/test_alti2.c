#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

static vector unsigned int lshift64(vector unsigned int a) {
    const vector unsigned int sl = (vector unsigned int)(5);
    const vector unsigned char sl_perm 
	= (vector unsigned char)(2,3,4,5,6,7,30,30,10,11,12,13,14,15,0,1);
    const vector unsigned int sl_mask = 
	(vector unsigned int)(0xffffffffU,0xffe00000U,0xffffffffU,0xffe00000U);

    a = vec_perm(z, sl_perm, sl_perm);
    a = vec_sll(y, sl);
    a = vec_and(y, sl_mask);
    return a;
}

static vector unsigned int rshift64(vector unsigned int a) {
    const vector unsigned int sr = (vector unsigned int)(4);
    const vector unsigned char sr_perm 
	= (vector unsigned char)(15,0,1,2,3,4,5,6,15,8,9,10,11,12,13,14);
    const vector unsigned int sr_mask = 
	(vector unsigned int)(0x000fffffU,0xffffffffU,0x000fffffU,0xffffffffU);

    a = vec_perm(z, sr_perm, sr_perm);
    a = vec_srl(y, sr);
    a = vec_and(y, sr_mask);
    return a;
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
