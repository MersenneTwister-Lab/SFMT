#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#include "sfmt-st.h"
#include "shortbase.h"
#include "util.h"

NTL_CLIENT;

mat_GF2 debug_mat;
mat_GF2 debug_mat2;
static unsigned int bit_len;

void dprintnext(in_status *st) {
    unsigned int i;

    for (i = 0; i < bit_len; i++) {
	//if ((st->next & (0x80000000U >> i)) != 0) {
	if ((st->next & NTH_BIT(i)) != 0) {
	    fprintf(stderr, "1");
	} else {
	    fprintf(stderr, "0");
	}
    }
}

void dprintbase(char *file, int line, int num, in_status *st) {
    fprintf(stderr, "%s:%d SB[", file, line);
    fprintf(stderr, "special:%c ", st->special ? 'S' : 'N');
    fprintf(stderr, "next:");
    dprintnext(st);
    fprintf(stderr, " count:%u ", st->count);
    fprintf(stderr, "bitlen:%u]\n", bit_len);
}

void set_bit_len(unsigned int len) {
    if ((len <= 0) || (len > 32)) {
	printf("bitLength error\n");
	exit(1);
    }
    bit_len = len;
    mask = mask_tab[bit_len - 1];
}

void set_special(in_status *st, unsigned int special_bit) {
    /*st->random = NULL;*/
    st->special = true;
    st->zero = false;
    st->count = 0;
    //st->next = 0x80000000U >> special_bit;
    st->next = NTH_BIT(special_bit);
}

void set_normal(in_status *st, ht_rand *ht) {
    int zero_count = 0;
    //DPRINTHT("ht:", ht);
    st->random = *ht;
    st->special = false;
    st->zero = false;
    st->count = 0;
    st->next = gen_rand(&(st->random)) & mask;
    st->count++;
    while (st->next == 0) {
	zero_count++;
	if (zero_count > MAXDEGREE) {
	    st->zero = true;
	    break;
	}
	st->next = gen_rand(&(st->random)) & mask;
	st->count++;
    }
    //DPRINTHT("ht:", &(st->random));
}

void add_status(in_status *dist, in_status *src) {
    unsigned int c;
    unsigned int n;

    c = dist->count;
    n = dist->next;
    if (dist->special && (!src->special)) {
	//DPRINT("before copy", NULL);
	//DPRINTHT("dist", &(dist->random));
	//DPRINTHT("src ", &(src->random));
	*dist = *src;
	//DPRINT("after copy", NULL);
	//DPRINTHT("dist", &(dist->random));
    } else if ((! dist->special) && (! src->special)) {
	//DPRINT("before add", NULL);
	//DPRINTHT("dist", &(dist->random));
	//DPRINTHT("src ", &(src->random));
	add(&(dist->random), &(src->random));
	//DPRINT("after add", NULL);
	//DPRINTHT("dist", &(dist->random));
    }
    dist->next = n ^ (src->next);
    dist->count = MIN(c, src->count);
}

void get_next_state(in_status *st) {
    int zero_count = 0;

    //DPRINTHT("before next", &(st->random));
    if (st->special) {
	printf("Request next to special\n");
	exit(1);
    }
    if (st->zero) {
	return;
    }
    st->next = gen_rand(&(st->random)) & mask;
    st->count++;
    while (st->next == 0) {
	zero_count++;
	if (zero_count > MAXDEGREE) {
	    st->zero = true;
	    break;
	}
	st->next = gen_rand(&(st->random)) & mask;
	st->count++;
    }
    //DPRINTHT("after next", &(st->random));
}
  
int get_shortest_base(unsigned int bit_len, ht_rand *ht) {
    static in_status bases[32 + 1];
    unsigned int next[bit_len + 1];
    bool dependents[bit_len + 1];
    unsigned int shortest;
    unsigned int i;
    int ret;

    //DPRINT("in get_shortest_base bit_len:%u", bit_len);
    set_bit_len(bit_len);
    for (i = 0; i < bit_len; i++) {
	set_special(&(bases[i]), i);
    }
    set_normal(&(bases[bit_len]), ht);
    for (;;) {
#ifdef DEBUG
	//DPRINT("base", NULL);
	for (i = 0; i <= bit_len; i++) {
	    //DPRINTBASE(i, &(bases[i]));
	}
#endif
	for (i = 0; i <= bit_len; i++) {
	    next[i] = bases[i].next;
	}
	ret = get_dependent_trans(dependents, next);
	if (ret < 0) {
	    break;
	}
#ifdef DEBUG
	fprintf(stderr, "dependents:");
	for (i = 0; i <= bit_len; i++) {
	    fprintf(stderr, "%1d", dependents[i]);
	}
	fprintf(stderr, "\n");
#endif
	shortest = get_shortest(dependents, bases);
	//DPRINT("shortest:%u", shortest);
	for (i = 0; i <= bit_len; i++) {
	    if (i == shortest) {
		continue;
	    }
	    if (dependents[i]) {
		add_status(&(bases[shortest]), &(bases[i]));
	    }
	}
	if (bases[shortest].next == 0) {
	    get_next_state(&(bases[shortest]));
	} else {
	    fprintf(stderr, "next is not zero\n");
	    cout << "debug_mat:" << debug_mat << endl;
	    for (i = 0; i <= bit_len; i++) {
		cout << dependents[i] ;
	    }
	    cout << endl;
	    exit(1);
	}
    }
    shortest = INT_MAX;
    for (i = 0; i <= bit_len; i++) {
	if (!bases[i].zero) {
	    if (bases[i].count < shortest) {
		shortest = bases[i].count;
	    }
	}
    }
    return shortest;
}

uint32_t get_shortest(bool dependents[], in_status bases[]) {
    uint32_t index = 0;
    uint32_t min = UINT_MAX;
    uint32_t i;

    for (i = 0; i <= bit_len; i++) {
	if (dependents[i]) {
	    if (bases[i].count < min) {
		min = bases[i].count;
		index = i;
	    }
	}
    }
    return index;
}

int get_dependent_trans(bool dependent[], unsigned int array[]) {
    mat_GF2 mat;
    uint32_t rank;

    convert(mat, array, bit_len);
    rank = (uint32_t) gauss_plus(mat);
    return dependent_rows(dependent, mat);
}

bool dependent_rows(bool result[], mat_GF2& mat) {
    int32_t rows;
    int32_t cols;
    int i, j, pos;
    bool found = false;

    rows = mat.NumRows();
    cols = mat.NumCols();
    for (i = 0; i < cols; i++) {
	result[i] = false;
    }
    pos = 0;
    for (i = 0; i < cols; i++) {
	if (IsOne(mat.get(i, i + pos))) {
	    continue;
	}
	for (j = 0; j < i; j++) {
	    if (IsOne(mat.get(j, i + pos))) {
		found = true;
		result[j] = true;
	    }
	}
	if (found) {
	    result[i + pos] = true;
	} else {
	    pos++;
	}
    }
    if (!found) {
#if 1
	cout << "not found" << endl;
	cout << mat << endl;
	exit(1);
#endif
    }
    return found;
}

void convert(mat_GF2& mat, uint32_t array[], uint32_t size) {
    uint32_t msk;
    uint32_t i, j;

    mat.SetDims(size + 1, size + 1);
    for (i = 0; i < size + 1; i++) {
	for (j = 0; j < size; j++) {
	    msk = NTH_BIT2(j);
	    if ((array[i] & msk) != 0) {
		mat.put(j, i, 1);
	    } else {
		mat.put(j, i, 0);
	    }
	}
    }
    for (i = 0; i < size + 1; i++) {
	mat.put(size, i, 0);
    }
}
