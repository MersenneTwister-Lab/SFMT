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
#include "SFMT.h"

static const int max_len = 216091 / 16 + 100;

static void chartoarray(uint64_t array[],
			int array_size,
			char *jump_polynomial)
{
    int len = strnlen(jump_polynomial, max_len);

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

void do_jump
