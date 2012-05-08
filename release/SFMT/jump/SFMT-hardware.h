#ifndef SFMT_HARDWARE_H
#define SFMT_HARDWARE_H

inline static void do_recursion(w128_t * r, w128_t * a, w128_t * b,
				w128_t * c, w128_t * d);
#if defined(HAVE_SSE2)
/**
 * This function represents the recursion formula.
 * @param a a 128-bit part of the interal state array
 * @param b a 128-bit part of the interal state array
 * @param c a 128-bit part of the interal state array
 * @param d a 128-bit part of the interal state array
 * @param mask 128-bit mask
 * @return output
 */
inline static void do_recursion(w128_t * r, w128_t * a, w128_t * b,
				w128_t * c, w128_t * d) {
    __m128i v, x, y, z;
    w128_t w;

    x = _mm_load_si128(&a->si);
    y = _mm_srli_epi32(b->si, SR1);
    z = _mm_srli_si128(c->si, SR2);
    v = _mm_slli_epi32(d->si, SL1);
    z = _mm_xor_si128(z, x);
    z = _mm_xor_si128(z, v);
    x = _mm_slli_si128(x, SL2);
    y = _mm_and_si128(y, sse2_param_mask.si);
    z = _mm_xor_si128(z, x);
    z = _mm_xor_si128(z, y);
    r->si = z;
}

#else /* C99 */
inline static void rshift128(w128_t *out,  w128_t const *in, int shift);
inline static void lshift128(w128_t *out,  w128_t const *in, int shift);

inline static void rshift128(w128_t *out, w128_t const *in, int shift) {
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in->u[3] << 32) | ((uint64_t)in->u[2]);
    tl = ((uint64_t)in->u[1] << 32) | ((uint64_t)in->u[0]);

    oh = th >> (shift * 8);
    ol = tl >> (shift * 8);
    ol |= th << (64 - shift * 8);
    out->u[1] = (uint32_t)(ol >> 32);
    out->u[0] = (uint32_t)ol;
    out->u[3] = (uint32_t)(oh >> 32);
    out->u[2] = (uint32_t)oh;
}

inline static void lshift128(w128_t *out, w128_t const *in, int shift) {
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in->u[3] << 32) | ((uint64_t)in->u[2]);
    tl = ((uint64_t)in->u[1] << 32) | ((uint64_t)in->u[0]);

    oh = th << (shift * 8);
    ol = tl << (shift * 8);
    oh |= tl >> (64 - shift * 8);
    out->u[1] = (uint32_t)(ol >> 32);
    out->u[0] = (uint32_t)ol;
    out->u[3] = (uint32_t)(oh >> 32);
    out->u[2] = (uint32_t)oh;
}

inline static void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *c,
				w128_t *d) {
    w128_t x;
    w128_t y;

    lshift128(&x, a, SFMT_SL2);
    rshift128(&y, c, SFMT_SR2);
    r->u[0] = a->u[0] ^ x.u[0] ^ ((b->u[0] >> SFMT_SR1) & SFMT_MSK1)
	^ y.u[0] ^ (d->u[0] << SFMT_SL1);
    r->u[1] = a->u[1] ^ x.u[1] ^ ((b->u[1] >> SFMT_SR1) & SFMT_MSK2)
	^ y.u[1] ^ (d->u[1] << SFMT_SL1);
    r->u[2] = a->u[2] ^ x.u[2] ^ ((b->u[2] >> SFMT_SR1) & SFMT_MSK3)
	^ y.u[2] ^ (d->u[2] << SFMT_SL1);
    r->u[3] = a->u[3] ^ x.u[3] ^ ((b->u[3] >> SFMT_SR1) & SFMT_MSK4)
	^ y.u[3] ^ (d->u[3] << SFMT_SL1);
}

#endif

#endif
