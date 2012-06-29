#ifndef SFMTEXT_HPP
#define SFMTEXT_HPP
#include <string.h>
/**
 * @file SFMText.hpp
 *
 * @brief exteted SFMT class for calculating characteristic polynomial.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (The University of Tokyo)
 *
 * Copyright (C) 2012 Mutsuo Saito, Makoto Matsumoto,
 * Hiroshima University and The University of Tokyo.
 * All rights reserved.
 *
 * The 3-clause BSD License is applied to this software, see
 * LICENSE.txt
 */
#include <stdexcept>
#include <stdint.h>
#include <inttypes.h>

namespace sfmt {
    using namespace std;

    struct w128_t {
	uint32_t u[4];
    };

    class SFMText {
    public:
	/**
	 * Constructor from parameters.
	 */
	SFMText(int mexp, int sl1, int sl2, int sr1, int sr2, int pos1,
		uint32_t mask[4], uint32_t parity[4]) {
		this->mexp = mexp;
		this->sl1 = sl1;
		this->sl2 = sl2;
		this->sr1 = sr1;
		this->sr2 = sr2;
		this->pos1 = pos1;
		this->mask1 = mask[0];
		this->mask2 = mask[1];
		this->mask3 = mask[2];
		this->mask4 = mask[3];
		this->parity[0] = parity[0];
		this->parity[1] = parity[1];
		this->parity[2] = parity[2];
		this->parity[3] = parity[3];
		this->size = mexp / 128 + 1;
		this->maxdegree = size * 128;
		this->state = new w128_t[size];
		this->index = 0;
		this->idx = 0;
		this->bitpos = 0;
	}

	/**
	 * release internal state.
	 */
	~SFMText() {
	    delete[] state;
	}

	int get_mexp() {
	    return mexp;
	}

	int get_maxdegree() {
	    return maxdegree;
	}

	void init_basis() {
	    if (bitpos >= maxdegree) {
		throw new logic_error("exceed max degree");
	    }
	    int i, j, k;
	    uint32_t mask;
	    k = bitpos % 32;
	    j = (bitpos / 32) % 4;
	    i = bitpos / 128;
	    memset(state, 0, sizeof(w128_t) * size);
	    mask = 1 << k;
	    state[i].u[j] = mask;
	    bitpos++;
	}

	/**
	 * generate next 128 bits.
	 * @returns 128-bit number.
	 */
	w128_t next() {
	    do_recursion();
	    w128_t result = state[index];
	    index = (index + 1) % size;
	    return result;
	}

	int next(uint32_t mask[4]) {
	    w128_t num = next();
	    for (int i = 0; i < 4; i++) {
		if ((num.u[i] & mask[i]) != 0) {
		    return 1;
		}
	    }
	    return 0;
	}
#if 0
	uint32_t next_uint32() {
	    int p = idx / 4;
	    if (idx % 4 == 0) {
		do_recursion();
		index = (index + 1) % size;
	    }
	    uint32_t work = state[p].u[idx % 4];
	    idx = (idx + 1) % (size * 4);
	    return work;
	}
#endif
	void seeding(uint32_t seed) {
	    int i;
	    uint32_t * psfmt32 = &state[0].u[0];

	    psfmt32[0] = seed;
	    for (i = 1; i < size * 4; i++) {
		psfmt32[i] = UINT32_C(1812433253)
		    * (psfmt32[i - 1] ^ (psfmt32[i - 1] >> 30))
		    + i;
	    }
	    idx = 0;
	    period_certification();
	}
    private:
	int mexp;
	int maxdegree;
	int sl1;
	int sl2;
	int sr1;
	int sr2;
	int pos1;
	int size;
	int index;
	int bitpos;
	int idx;
	uint32_t mask1;
	uint32_t mask2;
	uint32_t mask3;
	uint32_t mask4;
	uint32_t parity[4];
	w128_t * state;

	void rshift128(w128_t *out, w128_t const *in, int shift) {
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

	void lshift128(w128_t *out, w128_t const *in, int shift) {
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

	void do_recursion() {
	    do_recursion(&state[index],
			 &state[index],
			 &state[(index + pos1) % size],
			 &state[(index + size - 2) % size],
			 &state[(index + size - 1) % size]);
	}

	void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *c,
			  w128_t *d) {
	    w128_t x;
	    w128_t y;

	    lshift128(&x, a, sl2);
	    rshift128(&y, c, sr2);
	    r->u[0] = a->u[0] ^ x.u[0] ^ ((b->u[0] >> sr1) & mask1) ^ y.u[0]
		^ (d->u[0] << sl1);
	    r->u[1] = a->u[1] ^ x.u[1] ^ ((b->u[1] >> sr1) & mask2) ^ y.u[1]
		^ (d->u[1] << sl1);
	    r->u[2] = a->u[2] ^ x.u[2] ^ ((b->u[2] >> sr1) & mask3) ^ y.u[2]
		^ (d->u[2] << sl1);
	    r->u[3] = a->u[3] ^ x.u[3] ^ ((b->u[3] >> sr1) & mask4) ^ y.u[3]
		^ (d->u[3] << sl1);
	}

	void period_certification(void) {
	    int inner = 0;
	    int i, j;
	    uint32_t work;
	    uint32_t * psfmt32 = &state[0].u[0];

	    for (i = 0; i < 4; i++)
		inner ^= psfmt32[i] & parity[i];
	    for (i = 16; i > 0; i >>= 1)
		inner ^= inner >> i;
	    inner &= 1;
	    /* check OK */
	    if (inner == 1) {
		return;
	    }
	    /* check NG, and modification */
	    for (i = 0; i < 4; i++) {
		work = 1;
		for (j = 0; j < 32; j++) {
		    if ((work & parity[i]) != 0) {
			psfmt32[i] ^= work;
			return;
		    }
		    work = work << 1;
		}
	    }
	}
    };
}
#endif
