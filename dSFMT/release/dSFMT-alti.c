/** 
 * @file dSFMT-alti.c 
 * @brief double precision SIMD-oriented Fast Mersenne Twister (dSFMT)
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */

/**
 * @param dist posion of array. may equal to a. chaneged.
 * @param a position of array.
 * @param b position of array.
 * @param reg may keep in register. changed.
 * @param lung may keep in register. changed.
 */
inline static vector unsigned int vec_recursion(vector unsigned int a,
						vector unsigned int b,
						vector unsigned int reg,
						vector unsigned int lung) {
    vector unsigned int r, s, t, u, v, w, x, y, z;
    const vector unsigned char sl1 = (vector unsigned char)(ALTI_SL1);
    const vector unsigned char sl1_perm = SL1_PERM;
    const vector unsigned int sl1_msk = SL1_MSK;
    const vector unsigned char sl2_perm = SL2_PERM;
    const vector unsigned char sr1 = (vector unsigned char)(ALTI_SR1);
    const vector unsigned int sr1_msk = SR1_MSK;
    const vector unsigned char sr2_perm = SR2_PERM;
    const vector unsigned char perm = ALTI_PERM;
    const vector unsigned int low_mask = ALTI_LOW_MSK;
    const vector unsigned int high_const = ALTI_HIGH_CONST;

    x = vec_perm(a, (vector unsigned int)sl2_perm, sl2_perm);
    y = vec_srl(b, sr1);
    y = vec_and(y, sr1_msk);
    z = vec_perm(reg, (vector unsigned int)sl1_perm, sl1_perm);
    z = vec_sll(z, sl1);
    z = vec_and(z, sl1_msk);
    w = vec_perm(reg, (vector unsigned int)sr2_perm, sr2_perm);
    v = vec_perm(lung, (vector unsigned int)perm, perm);
    s = vec_xor(a, x);
    t = vec_xor(y, z);
    u = vec_xor(w, v);
    r = vec_xor(s, t);
    r = vec_xor(r, u);
    r = vec_and(r, low_mask);
    r = vec_or(r, high_const);
    return r;
}

inline static void gen_rand_all(void) {
    int i;
    vector unsigned int r, lung;

    lung = sfmt[N].s;
    r = sfmt[N - 1].s;
    for (i = 0; i < N - POS1; i++) {
	r = vec_recursion(sfmt[i].s, sfmt[i + POS1].s, r, lung);
	sfmt[i].s = r;
	lung = vec_xor(lung, r);
    }
    for (; i < N; i++) {
	r = vec_recursion(sfmt[i].s, sfmt[i + POS1 - N].s, r, lung);
	sfmt[i].s = r;
	lung = vec_xor(lung, r);
    }
    sfmt[N].s = lung;
}

inline static void gen_rand_array(w128_t array[], int size) {
    int i, j;
    vector unsigned int r, lung;

    /* read from sfmt */
    lung = sfmt[N].s;
    r = sfmt[N - 1].s;
    for (i = 0; i < N - POS1; i++) {
	r = vec_recursion(sfmt[i].s, sfmt[i + POS1].s, r, lung);
	array[i].s = r;
	lung = vec_xor(lung, r);
    }
    for (; i < N; i++) {
	r = vec_recursion(sfmt[i].s, array[i + POS1 - N].s, r, lung);
	array[i].s = r;
	lung = vec_xor(lung, r);
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = vec_recursion(array[i - N].s, array[i + POS1 - N].s, r, lung);
	array[i].s = r;
	lung = vec_xor(lung, r);
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j].s = array[j + size - N].s;
    }
    for (; i < size; i++) {
	r = vec_recursion(array[i - N].s, array[i + POS1 - N].s, r, lung);
	array[i].s = r;
	lung = vec_xor(lung, r);
	sfmt[j++].s = r;
    }
    sfmt[N].s = lung;
}
