#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#include "sfmt-cls.h"
#include "shortbase.h"
#include "util.h"

NTL_CLIENT;

#define MIN(a,b) ((a)>(b)?(b):(a))

mat_GF2 debug_mat;
mat_GF2 debug_mat2;
static unsigned int bit_len;

#if 0
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
#endif
void set_bit_len(unsigned int len) {
    if ((len <= 0) || (len > 32)) {
	printf("bitLength error\n");
	exit(1);
    }
    bit_len = len;
    //mask = mask_tab[bit_len - 1];
}

void set_special(in_status *st, unsigned int special_bit) {
    /*st->random = NULL;*/
    st->special = true;
    st->zero = false;
    st->count = 0;
    //st->next = 0x80000000U >> special_bit;
    st->next.SetLength(bit_len);
    st->next.put(special_bit, 1);
}

void set_normal(in_status *st, SFMT& sfmt) {
    int zero_count = 0;
    //DPRINTHT("ht:", ht);
    st->random = sfmt;
    st->special = false;
    st->zero = false;
    st->count = 0;
    st->next.SetLength(bit_len);
    //st->next = gen_rand(&(st->random)) & mask;
    st->random.gen_rand(st->next, bit_len);
    st->count++;
    while (IsZero(st->next)) {
	zero_count++;
	if (zero_count > MAXDEGREE) {
	    st->zero = true;
	    break;
	}
	st->random.gen_rand(st->next, bit_len);
	st->count++;
    }
    //DPRINTHT("ht:", &(st->random));
}

void add_status(in_status *dist, in_status *src) {
    unsigned int c;
    vec_GF2 next;

    c = dist->count;
    next = dist->next;
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
	dist->random.add(src->random);
	//DPRINT("after add", NULL);
	//DPRINTHT("dist", &(dist->random));
    }
    dist->next = next + src->next;
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
    st->random.gen_rand(st->next, bit_len);
    st->count++;
    while (IsZero(st->next)) {
	zero_count++;
	if (zero_count > MAXDEGREE) {
	    st->zero = true;
	    break;
	}
	st->random.gen_rand(st->next, bit_len);
	st->count++;
    }
    //DPRINTHT("after next", &(st->random));
}
  
int get_shortest_base(unsigned int bit_len, SFMT& sfmt) {
    static in_status bases[32 + 1];
    vec_GF2 next[bit_len + 1];
    bool dependents[bit_len + 1];
    unsigned int shortest;
    unsigned int i;
    bool dependent_found;

    //DPRINT("in get_shortest_base bit_len:%u", bit_len);
    set_bit_len(bit_len);
    for (i = 0; i < bit_len; i++) {
	set_special(&(bases[i]), i);
    }
    set_normal(&(bases[bit_len]), sfmt);
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
	dependent_found = get_dependent_trans(dependents, next);
	if (!dependent_found) {
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
	if (IsZero(bases[shortest].next)) {
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

bool get_dependent_trans(bool dependent[], vec_GF2 array[]) {
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
    for (i = 0; (i < cols - pos) && (i < rows); i++) {
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
#if 0
	cout << "not found" << endl;
	cout << mat << endl;
	exit(1);
#endif
    }
    return found;
}

void convert(mat_GF2& mat, vec_GF2 array[], uint32_t size) {
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
