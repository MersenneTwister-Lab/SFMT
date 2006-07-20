#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static inline void rshift128(uint32_t out[4], const uint32_t in[4],
			     int shift) {
    uint64_t t1, t2, o1, o2;
    int i;

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

int main(void) {
    uint32_t a[4] = {0x12345678, 0x90123456, 0x78901234, 0xabcd9876};
    uint32_t b[4];
    int i;
    
    for (i = 0; i < 4; i++) {
	printf("%d:%08x\n", i, a[i]);
    }
    rshift128(b, a, 1);
    for (i = 0; i < 4; i++) {
	printf("%d:%08x\n", i, b[i]);
    }
    rshift128(b, a, 2);
    for (i = 0; i < 4; i++) {
	printf("%d:%08x\n", i, b[i]);
    }
    return 0;
}
