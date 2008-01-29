#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include "shortbase104.h"
#include "util.h"
#include "dsfmt-util.h"
#include "dsfmt.h"

NTL_CLIENT;

/* internal status */
class in_status {
public:
    bool zero;
    bool special;
    vec_GF2 previous;
    vec_GF2 previous2;
    vec_GF2 next;
    int count;
    int last_weight_mode;
    DSFMT *random;
    in_status() {random = NULL;}
    ~in_status() {
	if (random != NULL) {
	    delete random;
	}
    }
};

#define MIN(a,b) ((a)>(b)?(b):(a))

//mat_GF2 debug_mat0;
//mat_GF2 debug_mat;
//bool debug_dependent[128];
//uint32_t debug_count;
int debug_flag = 0;
static int bit_len;
static int status_mode;	// 0-1 状態空間のどこを見るか
static int weight_mode;	// 0-1 ノルムにweighetをかける位置
static int max_weight_mode;
static int maxdegree;
static void (*get_next_random)(vec_GF2& vec, vec_GF2& prev, vec_GF2& prev2, 
			       DSFMT& sfmt);

static void get_next_random104(vec_GF2& vec, vec_GF2& prev, vec_GF2& prev2, 
			       DSFMT& sfmt);
static void get_next_random52(vec_GF2& vec, vec_GF2& prev, vec_GF2& prev2, 
			      DSFMT& sfmt);
static void get_next_random104_lsb(vec_GF2& vec, vec_GF2& prev, vec_GF2& prev2, 
				   DSFMT& sfmt);
static void get_next_random52_lsb(vec_GF2& vec, vec_GF2& prev, vec_GF2& prev2, 
				  DSFMT& sfmt);
static void set_special(in_status& st, unsigned int special_bit);
static void set_normal(in_status& st, const DSFMT& sfmt);
static void add_status(in_status& dist, in_status& src);
static void get_next_state(in_status& st);
static uint32_t get_shortest(bool dependents[], in_status bases[]);
static bool get_dependent_trans(bool dependent[], vec_GF2 array[]);
static bool dependent_rows(bool result[], mat_GF2& mat);
static void convert(mat_GF2& mat, vec_GF2 array[], int bit_len);
static void change_weight_mode(in_status bases[], vec_GF2 next[]);

uint64_t vecto64(vec_GF2& vec) {
    uint64_t v = 0;
    int i;
    for (i = 0; i < vec.length(); i++) {
	v = (v << 1) | IsOne(vec[i]);
    }
    return v ;
}

static void copy_status(in_status& dist, const in_status& src) {
    dist.zero = src.zero;
    dist.special = src.special;
    dist.previous = src.previous;
    dist.previous2 = src.previous2;
    dist.next = src.next;
    dist.count = src.count;
    dist.random = new DSFMT(*src.random);
}

void set_up(uint32_t bit_mode, uint32_t len, uint32_t p_mode, bool msb) {
    maxdegree = DSFMT::get_rnd_maxdegree();
    if (bit_mode == 128) {
	max_weight_mode = 1;
	if ((len <= 0) || (len > 104)) {
	    printf("bitLength error mode 128\n");
	    exit(1);
	}
	if (msb) {
	    get_next_random = get_next_random104;
	} else {
	    get_next_random = get_next_random104_lsb;
	}
    } else {
	max_weight_mode = 2;
	if ((len <= 0) || (len > 52)) {
	    printf("bitLength error mode 64\n");
	    exit(1);
	}
	if (msb) {
	    get_next_random = get_next_random52;
	} else {
	    get_next_random = get_next_random52_lsb;
	}
    }
    bit_len = len * max_weight_mode;
    weight_mode = max_weight_mode;
    status_mode = p_mode;
}

void set_special(in_status& st, unsigned int special_bit) {
    st.special = true;
    st.zero = false;
    st.count = 0;
    st.previous.SetLength(0);
    st.previous.SetLength(bit_len);
    st.previous2.SetLength(0);
    st.previous2.SetLength(bit_len);
    st.next.SetLength(0);
    st.next.SetLength(bit_len);
    st.next.put(special_bit, 1);
}

/*
 * prevは使わないからこれでよい。
 */
static void get_next_random104(vec_GF2& vec, vec_GF2& prev, vec_GF2& prev2,
			       DSFMT& sfmt) {
    uint64_t array64[2];
    uint64_t mask;
    int i, j, k;

    sfmt.gen_rand104sp(array64, status_mode);
    k = 0;
    for (i = 1; (i >= 0) && (k < bit_len); i--) {
	mask = (uint64_t)1 << 51;
	for (j = 0; (j < 52) && (k < bit_len); j++) {
	    if (array64[i] & mask) {
		vec.put(k, 1);
	    } else {
		vec.put(k, 0);
	    }
	    mask = mask >> 1;
	    k++;
	}
    }
#if 0
    if (debug_flag) {
	cout << vec << endl;
    }
#endif
}

static void get_next_random104_lsb(vec_GF2& vec, vec_GF2& prev, vec_GF2& prev2,
				   DSFMT& sfmt) {
    uint64_t array64[2];
    uint64_t mask;
    int i, j, k;

    sfmt.gen_rand104sp(array64, status_mode);
    k = 0;
    for (i = 0; (i < 2) && (k < bit_len); i++) {
	mask = (uint64_t)1;
	for (j = 0; (j < 52) && (k < bit_len); j++) {
	    if (array64[i] & mask) {
		vec.put(k, 1);
	    } else {
		vec.put(k, 0);
	    }
	    mask = mask << 1;
	    k++;
	}
    }
#if 0
    if (debug_flag) {
	cout << vec << endl;
    }
#endif
}

/*
 *
 */
static void get_next_random52(vec_GF2& vec, vec_GF2& prev, vec_GF2& prev2,
			      DSFMT& sfmt) {
    uint64_t array64[2];
    vec_GF2 tmp;
    uint64_t mask;
    int i, j, k;

    tmp.SetLength(bit_len);
    sfmt.gen_rand104sp(array64, status_mode);
    k = 0;
    for (i = 1; i >= 0; i--) {
	mask = (uint64_t)1 << 51;
	for (j = 0; j <  bit_len; j += 2) {
	    if (array64[i] & mask) {
		tmp.put(k, 1);
	    } else {
		tmp.put(k, 0);
	    }
	    mask = mask >> 1;
	    k++;
	}
    }
    /* weight 付きノルムの計算 */
    for (i = 0; i < bit_len * weight_mode / 2 ; i++) {
	vec.put(i, tmp.get(i));
    }
    for (; i < bit_len; i++) {
	vec.put(i, prev.get(i));
    }
    prev2 = prev;
    prev = tmp;
#if 0
    if (debug_flag) {
	cout << vec << endl;
    }
#endif
}

static void get_next_random52_lsb(vec_GF2& vec, vec_GF2& prev, vec_GF2& prev2,
				  DSFMT& sfmt) {
    uint64_t array64[2];
    vec_GF2 tmp;
    uint64_t mask;
    int i, j, k;

    assert(status_mode % 2 == 0);
    tmp.SetLength(bit_len);
    sfmt.gen_rand104sp(array64, status_mode);
    k = 0;
    for (i = 0; i < 2; i++) {
	mask = 1;
	for (j = 0; j < bit_len; j += 2) {
	    if (array64[0] & mask) {
		tmp.put(k, 1);
	    } else {
		tmp.put(k, 0);
	    }
	    mask = mask << 1;
	    k++;
	}
    }
    /* weight 付きノルムの計算 */
    for (i = 0; i < bit_len * weight_mode / 2 ; i++) {
	vec.put(i, tmp.get(i));
    }
    for (; i < bit_len; i++) {
	vec.put(i, prev.get(i));
    }
    prev2 = prev;
    prev = tmp;
#if 0
    if (debug_flag) {
	cout << vec << endl;
    }
#endif
}

static void set_normal(in_status& st, const DSFMT& dsfmt) {
    int zero_count = 0;

    st.random = new DSFMT(dsfmt);
    st.special = false;
    st.zero = false;
    st.count = 0;
    st.previous.SetLength(0);
    st.previous.SetLength(bit_len);
    st.previous2.SetLength(0);
    st.previous2.SetLength(bit_len);
    st.next.SetLength(0);
    st.next.SetLength(bit_len);
    get_next_random(st.next, st.previous, st.previous2, *st.random);
    st.count++;
    while (IsZero(st.next)) {
	zero_count++;
	if (zero_count > 2 * maxdegree) {
	    st.zero = true;
	    break;
	}
	get_next_random(st.next, st.previous, st.previous2, *st.random);
	st.count++;
    }
}

static void add_status(in_status& dist, in_status& src) {
    int c;
    vec_GF2 next;

    c = dist.count;
    next = dist.next;
#if 0
    if (dist.special && src.special) {
	printf("add special to special\n");
	exit(1);
    }
#endif
    if (dist.special && (!src.special)) {
	copy_status(dist, src);
    } else if ((! dist.special) && (! src.special)) {
	dist.previous += src.previous;
	dist.previous2 += src.previous2;
	dist.random->add(*src.random);
    }
    next += src.next;
    if ((IsZero(next)) && dist.special) {
	printf("something wrong!\n");
        cout << "src:" << &src << endl;
        cout << "dist:" << &dist << endl;
	cout << "dist.next:" << dist.next << endl;
	cout << "src.next:" << src.next << endl;
	cout << "src.special:" << src.special << endl;
	exit(1);
    }
    dist.next = next;
    dist.count = MIN(c, src.count);
}

static void get_next_state(in_status& st) {
    int zero_count = 0;

    if (st.special) {
	printf("Request next to special\n");
	exit(1);
    }
    if (st.zero) {
	return;
    }
    get_next_random(st.next, st.previous, st.previous2, *st.random);
    st.count++;
    while (IsZero(st.next)) {
	zero_count++;
	if (zero_count > 2 * maxdegree) {
	    st.zero = true;
	    break;
	}
	get_next_random(st.next, st.previous, st.previous2, *st.random);
	st.count++;
    }
    st.last_weight_mode = weight_mode;
}
  
int get_shortest_base(const DSFMT& sfmt) {
    in_status bases[bit_len + 1];
    vec_GF2 next[bit_len + 1];
    bool dependents[bit_len + 1];
    int shortest, min_mode;
    int i;
    bool dependent_found;

    for (i = 0; i < bit_len; i++) {
	set_special(bases[i], i);
    }
    set_normal(bases[bit_len], sfmt);
    for (;;) {
	for (i = 0; i <= bit_len; i++) {
	    next[i] = bases[i].next;
	}
	dependent_found = get_dependent_trans(dependents, next);
	while ((!dependent_found) && (weight_mode > 1)) {
#if 0
	    for (i = 0; i <= bit_len; i++) {
		cout << next[i] << endl;
	    }
#endif
	    weight_mode--;
	    change_weight_mode(bases, next);
	    dependent_found = get_dependent_trans(dependents, next);
#if 0
	    printf("dependent_foud:%d\n", dependent_found);
	    cout << "dependent:";
	    for (i = 0; i <= bit_len; i++) {
		cout << dependents[i] ;
	    }
	    cout << endl;
	    for (i = 0; i <= bit_len; i++) {
		cout << next[i] << endl;
	    }
#endif
	}
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
	for (i = 0; i <= bit_len; i++) {
	    if (i == shortest) {
		continue;
	    }
	    if (dependents[i]) {
		add_status(bases[shortest], bases[i]);
	    }
	}
	if (IsZero(bases[shortest].next)) {
	    get_next_state(bases[shortest]);
	} else {
	    fprintf(stderr, "next is not zero\n");
#if 0
	    cout << "debug_mat0:" << debug_mat0 << endl;
	    cout << "debug_mat:" << debug_mat << endl;
	    cout << "dependent:";
#endif
	    for (i = 0; i <= bit_len; i++) {
		cout << dependents[i] ;
	    }
	    cout << endl;
	    for (i = 0; i <= bit_len; i++) {
		cout << next[i] << endl;
	    }
	    exit(1);
	}
    }
#if 0
    for (i = 0; i <= bit_len; i++) {
	if (bases[i].zero) {
	    printf("%d:zero\n", i);
	} else {
	    printf("%d:mode(%d):count(%d)\n", i, bases[i].last_weight_mode,
		   bases[i].count);
	}
    }
#endif
    shortest = INT_MAX;
    for (i = 0; i <= bit_len; i++) {
	if (!bases[i].zero) {
	    if (bases[i].count < shortest) {
		shortest = bases[i].count;
	    }
	}
    }
#if 0
    for (i = 0; i <= bit_len; i++) {
	if (bases[i].count == shortest) {
	    printf("%d:mode(%d):count(%d) = %d\n", i, bases[i].last_weight_mode,
		   bases[i].count, 
		   shortest * max_weight_mode 
		   - (max_weight_mode - bases[i].last_weight_mode)) ;
	}
    }
#endif
    min_mode = 5;
    for (i = 0; i <= bit_len; i++) {
	if (bases[i].count == shortest) {
	    if (bases[i].last_weight_mode < min_mode) {
		min_mode = bases[i].last_weight_mode;
	    }
	}
    }
    return shortest * max_weight_mode - (max_weight_mode - min_mode);
}

static void change_weight_mode(in_status bases[], vec_GF2 next[]) {
    int i, j;

    for (i = 0; i <= bit_len; i++) {
#if 0
	cout << "p" << bases[i].previous2 << " " << bases[i].count << endl;
	cout << "c" << bases[i].next << endl;
#endif
	for (j = bit_len * weight_mode / max_weight_mode; j < bit_len; j++) {
	    bases[i].next.put(j, bases[i].previous2.get(j));
	}
	// これでよいかわからない。
	if (bases[i].special && IsZero(bases[i].next)) {
	    bases[i].zero = true;
	} else if (IsZero(bases[i].next)) {
	    get_next_state(bases[i]);
	}
	next[i] = bases[i].next;
#if 0
	cout << "c" << bases[i].next << " " << bases[i].count 
	     << " " << bases[i].last_weight_mode << endl;
	cout << "n" << next[i] << endl;
#endif
    }
}

static uint32_t get_shortest(bool dependents[], in_status bases[]) {
    int index = 0;
    int min = INT_MAX;
    int i;

    for (i = 0; i <= bit_len; i++) {
	if (dependents[i]) {
	    if (bases[i].count < min) {
		min = bases[i].count;
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
#if 0
    if (debug_flag && (!found)) {
	cout << "not found" << endl;
	cout << mat << endl;
	exit(1);
    }
#endif
    return found;
}

static void convert(mat_GF2& mat, vec_GF2 array[], int size) {
    int i, j;

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
