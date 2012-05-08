/**
 * @file do-jump.c
 *
 * @brief do jump using jump polynomial.
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

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "SFMT-params.h"
#include "SFMTst.h"
#include "SFMT-jump.h"
#include "SFMT-hardware.h"

static const int max_len = 216091 / 16 + 100;
static void chartoarray(uint64_t array[],
			int array_size,
			char *jump_polynomial);
inline static void next_state(sfmt_t * sfmt);

static void chartoarray(uint64_t array[],
			int array_size,
			char *jump_polynomial)
{
    int len = strnlen(jump_polynomial, max_len);
    assert(len == (SFMT_MEXP) / 16 + 1);
    for (int i = 0; i < array_size; i++) {
        array[i] = 0;
    }

    char temp[17];
    temp[16] = '\0';
    int idx = 0;
    int j = 0;

    for (int i = 0; i < len; i++) {
	temp[j++] = jump_polynomial[i];
	if (j >= 16) {
	    j = 0;
	    array[idx++] = strtoull(temp, NULL, 16);
	} else {
	    temp[j] = '\0';
	}
    }
    if (j > 0) {
	array[idx] = strtoull(temp, NULL, 16);
    }
}

inline static void add(sfmt_t *dest, sfmt_t *src) {
    int dp = dest->idx / 4;
    int sp = src->idx / 4;
    int diff = (sp - dp + SFMT_N) % SFMT_N;
    int p;
    for (int i = 0; i < SFMT_N; i++) {
	p = (i + diff) % SFMT_N;
	for (int j = 0; j < 4; j++) {
	    dest->state[i].u[j] ^= src->state[p].u[j];
	}
	p++;
    }
}

inline static void next_state(sfmt_t * sfmt) {
    int idx = sfmt->idx;
    w128_t *r1, *r2;
    w128_t * pstate = sfmt->state;

    r1 = &pstate[(idx + SFMT_N - 2) % SFMT_N];
    r2 = &pstate[(idx + SFMT_N - 1) % SFMT_N];
    do_recursion(&pstate[idx], &pstate[idx], &pstate[idx + SFMT_POS1], r1, r2);
    r1 = r2;
    r2 = &pstate[idx];
    sfmt->idx = (sfmt->idx + 1) % SFMT_N;
}

void do_jump_by_string(char * jump_str, sfmt_t * sfmt) {
    int array_size = SFMT_MEXP / 16 + 1;
    uint64_t array[array_size];
    chartoarray(array, array_size, jump_str);
    do_jump_by_array(array, array_size, sfmt);
}

void do_jump_by_array(uint64_t *jump_array, int array_size, sfmt_t * sfmt) {
    sfmt_t work;
    int index = sfmt->idx;
    uint64_t bits;
    memset (&work, 0, sizeof(sfmt_t));

    for (int i = 0; i < array_size; i++) {
	bits = jump_array[i];
	for (int j = 0; j < 64; j++) {
	    if (bits & 1) {
		add(&work, sfmt);
	    }
	    next_state(sfmt);
	}
    }
    *sfmt = work;
    sfmt->idx = index % 4;
}
