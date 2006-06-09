#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#include "ht-shortbase32.h"
#include "util.h"
#include "ht-st.h"

NTL_CLIENT;

mat_GF2 debug_mat0;
//mat_GF2 debug_mat;
//bool debug_dependent[128];
//uint32_t debug_count;
//int debug_flag = 0;

static uint32_t get_shortest(bool dependents[], int count[]);
static bool get_dependent_trans(bool dependent[], vec_GF2 array[]);
static bool dependent_rows(bool result[], mat_GF2& mat);
static void convert(mat_GF2& mat, vec_GF2 array[], uint32_t bit_len);

static ht_rand bases[128 + 1];

static int bit_len;
void set_up(uint32_t len) {
    if ((len <= 0) || (len > 32)) {
	printf("bitLength error mode 32\n");
	exit(1);
    }
    bit_len = len;
}
  
int get_shortest_base(ht_rand *sfmt) {
    vec_GF2 next[bit_len + 1];
    int count[bit_len + 1];
    bool dependents[bit_len + 1];
    int shortest;
    int i;
    bool dependent_found;
    int debug_count;

    for (i = 0; i < bit_len; i++) {
	set_special(&(bases[i]), i);
    }
    bases[bit_len] = *sfmt;
    bases[bit_len].special = false;
    for (;;) {
	for (i = 0; i <= bit_len; i++) {
	    count[i] = get_vector32(next[i], &bases[i], bit_len);
	}
	dependent_found = get_dependent_trans(dependents, next);
	if (!dependent_found) {
	    break;
	}
	shortest = get_shortest(dependents, count);
#if 0
	for (i = 0; i <= bit_len; i++) {
	    cout << dependents[i];
	}
	cout << endl;
	for (i = 0; i <= bit_len; i++) {
	    cout << next[i] << " " << count[i] << endl;
	}
	cout << "before add " << next[shortest] << " " << count[shortest] 
	     << " " << bases[shortest].special << endl;
#endif
	debug_count = count[shortest];
	if (debug_count > 0) {
	    debug_count = 1;
	}
	for (i = 0; i <= bit_len; i++) {
	    if (i == shortest) {
		continue;
	    }
	    if (dependents[i]) {
		add_rnd1(&(bases[shortest]), &(bases[i]),
			 count[i] - count[shortest]);
	    }
	}
#if 0
	count[0] = get_vector32(next[0], &bases[shortest], bit_len);
	cout << "count " << debug_count << " -> " << count[0] << endl;
#endif
    }
    shortest = INT_MAX;
    for (i = 0; i <= bit_len; i++) {
	//if (!is_zero(&bases[i])) {
	if (count[i] < shortest) {
	    shortest = count[i];
	}
	//}
    }
#if 0
    //cout << "next:" << endl;
    for (i = 0; i <= bit_len; i++) {
	//if (count[i] == shortest) {
	printf("%d:count(%d)\n", i, count[i]);
	//cout << next[i] << endl;
	//}
    }
#endif
    return shortest;
}

static uint32_t get_shortest(bool dependents[], int count[]) {
    int index = 0;
    int min = INT_MAX;
    int i;

    for (i = 0; i <= bit_len; i++) {
    //for (i = bit_len; i >= 0; i--) {
	if (dependents[i]) {
	    if (count[i] < min) {
		min = count[i];
		index = i;
	    }
	}
    }
    if (min == INT_MAX) {
	printf("can't find shortest\n");
	exit(1);
    }
    return index;
}

static bool get_dependent_trans(bool dependent[], vec_GF2 array[]) {
    mat_GF2 mat;
    uint32_t rank;

    convert(mat, array, bit_len);
    debug_mat0 = mat;
    rank = (uint32_t) gauss_plus(mat);
    return dependent_rows(dependent, mat);
}

static bool dependent_rows(bool result[], mat_GF2& mat) {
    int32_t rows;
    int32_t cols;
    int i, j, k, pos;
    bool found = false;

    rows = mat.NumRows();
    cols = mat.NumCols();
    for (i = 0; i < cols; i++) {
	result[i] = false;
    }
    pos = 0;
    for (i = 0; (i < cols - pos) && (i < rows); i++) {
	if (IsOne(mat.get(i, i + pos))) {
	    continue;
	}
	for (j = 0; j < i; j++) {
	    if (IsOne(mat.get(j, i + pos))) {
		for (k = 0; k < i + pos; k++) {
		    if (IsOne(mat.get(j, k))) {
			found = true;
			result[k] = true;
			break;
		    }
		}
	    }
	}
	if (found) {
	    result[i + pos] = true;
	    break;
	} else {
	    pos++;
	    i--;
	}
    }
    if (!found) {
#if 0
	cout << "not found debug_mat:" << endl;
	cout << debug_mat0 << endl;
	cout << "mat:" << endl;
	cout << mat << endl;
	exit(1);
#endif
    }
    //debug_mat = mat;
    return found;
}

static void convert(mat_GF2& mat, vec_GF2 array[], uint32_t size) {
    uint32_t i, j;

    mat.SetDims(size + 1, size + 1);
    for (i = 0; i < size + 1; i++) {
	for (j = 0; j < size; j++) {
	    mat.put(j, i, array[i].get(j));
	}
    }
    for (i = 0; i < size + 1; i++) {
	mat.put(size, i, 0);
    }
}
