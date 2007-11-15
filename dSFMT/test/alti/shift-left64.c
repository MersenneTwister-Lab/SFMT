/* Hearty Twister Search Code, Makoto Matsumoto 2005/5/6 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "param-shift.h"

#if 0
static void rshift64(uint32_t out[4], const uint32_t in[4], int shift) {
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in[3] << 32) | ((uint64_t)in[2]);
    tl = ((uint64_t)in[1] << 32) | ((uint64_t)in[0]);

    oh = th >> shift;
    ol = tl >> shift;
    out[1] = (uint32_t)(ol >> 32);
    out[0] = (uint32_t)ol;
    out[3] = (uint32_t)(oh >> 32);
    out[2] = (uint32_t)oh;
}
#else
static void rshift64(uint64_t out[2], const uint64_t in[2], int shift) {
    out[0] = in[0] >> shift;
    out[1] = in[1] >> shift;
}
#endif
#if 0
static void lshift64(uint32_t out[4], const uint32_t in[4], int shift) {
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in[3] << 32) | ((uint64_t)in[2]);
    tl = ((uint64_t)in[1] << 32) | ((uint64_t)in[0]);

    oh = th << shift;
    ol = tl << shift;
    out[1] = (uint32_t)(ol >> 32);
    out[0] = (uint32_t)ol;
    out[3] = (uint32_t)(oh >> 32);
    out[2] = (uint32_t)oh;
}
#else
static void lshift64(uint64_t out[2], const uint64_t in[2], int shift) {
    out[0] = in[0] << shift;
    out[1] = in[1] << shift;
}
#endif

vector unsigned int v_sl64(vector unsigned int v, int sl) {
    vector unsigned int r;
    vector unsigned char perm_sl = (vector unsigned char)(PERM_SL);
    vector unsigned int sl128 = (vector unsigned char)(SL128);
    vector unsigned int mask_sl = (vector unsigned int)(MASK_SL);
    
    r = vec_perm((vector unsigned char)v, perm_sl, perm_sl);
    r = vec_sll(r, sl128);
    r = vec_and(r, mask_sl);
    return r;
}

vector unsigned int v_sr64(vector unsigned int v, int sr) {
    vector unsigned int r;
    vector unsigned char perm_sr = (vector unsigned char)(PERM_SR);
    vector unsigned int sr128 = (vector unsigned char)(SR128);
    vector unsigned int mask_sr = (vector unsigned int)(MASK_SR);

    r = vec_perm((vector unsigned char)v, perm_sr, perm_sr);
    r = vec_srl(r, sr128);
    r = vec_and(r, mask_sr);
    return r;
}

static void pr(unsigned int x[4]) {
    int i;
    for (i = 0; i < 4; i++) {
	printf("%08x ", x[i]);
    }
    printf("\n");
    fflush(stdout);
 }

static int cmp(vector unsigned int v, uint32_t u[4]) {
    union {
	vector unsigned int v;
	uint32_t a[4];
    } uni;
    int i;

    uni.v = v;
    for (i = 0; i < 4; i++) {
	if (uni.a[i] != u[i]) {
	    return 1;
	}
    }
    return 0;
}

int main(void) {
    uint32_t x[4];
    vector unsigned int a = (vector unsigned int)
	(0xa1a2a3a5,0xb1b2b3b5,0xc1c2c3c5,0xd1d2d3d5);
    unsigned int *ap = (unsigned int *)&a;
    vector unsigned int r;
    int res;
    
    printf("START\n");
    pr(ap);
    printf("%08vlx\n", a);

    printf("SL = %d\n", SL);
    lshift64((uint64_t *)x, (uint64_t *)ap, SL);
    pr(x);
    r = v_sl64(a, SL);
    printf("%08vlx\n", r);
    res = cmp(r, x);

    printf("SR = %d\n", SR);
    rshift64((uint64_t *)x, (uint64_t *)ap, SR);
    pr(x);
    r = v_sr64(a, SR);
    printf("%08vlx\n", r);
    res += cmp(r, x);
    printf("res = %d\n", res);
    return res;
}
