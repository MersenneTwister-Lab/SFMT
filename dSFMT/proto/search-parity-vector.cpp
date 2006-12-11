/* 
 * Search Parity Check Vector
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>

extern "C" {
#include "dsfmt-st.h"
#include "mt19937ar.h"
}

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>
#include "util.h"

NTL_CLIENT;

const int WORD_WIDTH = 104;

/* internal status */
struct IN_STATUS {
    bool zero;
    vec_GF2 next;
    dsfmt_t random;
};
typedef struct IN_STATUS in_status;

#define NTH_BIT2(i) (1U << (31 - (i)))

void search_lung (GF2X& f);
void set_status(in_status *st);
void add_status(in_status *dist, in_status *src);
void get_next_state(in_status *st);
void get_base(in_status bases[], int size);
int get_dependent_index(uint8_t dependents[], int size);
int get_dependent_trans(uint8_t dependent[], vec_GF2 array[], int size);
int dependent_rows(uint8_t result[], mat_GF2& mat);
void convert(mat_GF2& mat, vec_GF2 array[], int bit_len);
void make_zero_state(dsfmt_t *sfmt, GF2X& poly);
void search_parity_check_vector(in_status *st, int size);
void set_bit(in_status *st, GF2X& f, uint32_t *bit_pos);

static int mexp;
static int maxdegree;

int main(int argc, char *argv[]) {
    GF2X f;
    FILE *fp;
    char c;
    char s[257];

    if (argc != 2) {
	printf("usage:%s filename %d\n", argv[0], argc);
	exit(1);
    }
    mexp = get_rnd_mexp();
    maxdegree = get_rnd_maxdegree();
    printf("mexp = %d\n", mexp);
    printf("filename:%s\n", argv[1]);
    fp = fopen(argv[1], "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("main");
	fclose(fp);
	exit(1);
    }
    read_random_param(fp);
    print_param(stdout);
    c = getc(fp);
    if (c < '0' || c > '9') {
	fgets(s, 256, fp);
    } else {
	ungetc(c, fp);
    }
    readFile(f, fp);
    printf("deg poly = %ld\n", deg(f));
    fclose(fp);
    search_lung(f);
    return 0;
}

void set_bit(in_status *st, GF2X& f, int *bit_pos) {
  uint32_t i, j, k;
  uint32_t mask;

  for (;*bit_pos <= maxdegree;) {
    k = *bit_pos % 52;
    j = (*bit_pos / 52) % 2;
    i = *bit_pos / (52 * 2);
    memset(&(st->random), 0, sizeof(dsfmt_t));
    mask = 1U << k;
    st->random.status[i][j] = mask;
    (*bit_pos)++;
    make_zero_state(&(st->random), f);
    set_status(st);
    if (!st->zero) {
      break;
    }
  }
}

void make_zero_state(dsfmt_t *sfmt, GF2X& poly) {
  static dsfmt_t sfmtnew;
  uint64_t array[2];
  int i;

  memset(&sfmtnew, 0, sizeof(sfmtnew));
  for (i = 0; i <= deg(poly); i++) {
    if (coeff(poly, i) != 0) {
	add_rnd(&sfmtnew, sfmt);
    }
    gen_rand104sp(sfmt, array, 0);
  }
  *sfmt = sfmtnew;
}

void search_lung (GF2X& f) {
    static in_status bases[WORD_WIDTH];
    int i, j;
    int count;
    int bit_pos = 0;
    int size = 2;
    int base_num = (WORD_WIDTH - mexp % WORD_WIDTH);

    set_bit(&(bases[0]), f, &bit_pos);
    set_bit(&(bases[1]), f, &bit_pos);
    //while(size <= base_num) {
    while((bit_pos < maxdegree) && (size <= base_num)) {
	get_base(bases, size);
	count = 0;
	for (i = 0; i < size; i++) {
	    if (!bases[i].zero) {
		count++;
	    }
	}
	if (count == size) {
	    if (size + 1 <= base_num) {
		set_bit(&bases[size++], f, &bit_pos);
	    } else {
		break;
	    }
	} else {
	    for (i = 0; i < size; i++) {
		if (bases[i].zero) {
		    set_bit(&bases[i], f, &bit_pos);
		}
	    }
	}
#if 0
	fprintf(stderr, "size = %d\n", size);
	fprintf(stderr, "count = %d\n", count);
	fprintf(stderr, "bit_pos = %d\n", bit_pos);
#endif
    }
    printf("----\n");
    for (i = 0; i < size; i++) {
	for (j = 0; j < WORD_WIDTH; j++) {
	    if (IsZero(bases[i].next[j])) {
		printf("0");
	    } else {
		printf("1");
	    }
	}
	printf("\n");
    }
    search_parity_check_vector(bases, size);
}

#include <inttypes.h>

void search_parity_check_vector(in_status base[], int size) {
    mat_GF2 mx;
    mat_GF2 my;
    uint64_t mask;
    uint64_t parity[2];
    int i, j, k;

    mx.SetDims(WORD_WIDTH, size);
    for (i = 0; i < WORD_WIDTH; i++) {
	for (j = 0; j < size; j++) {
	    mx.put(i, j, base[j].next[i]);
	}
    }
    kernel(my, mx);
    if (my.NumRows() == 0) {
	printf("initial lung can't find\n");
	return;
    }
    printf("dim kernel = %ld\n", my.NumRows());
    printf("-----\n");
    for (i = 0; i < my.NumRows(); i++) {
	for (j = 0; j < WORD_WIDTH; j++) {
	    if (IsZero(my.get(i, j))) {
		printf("0");
	    } else {
		printf("1");
	    }
	}
	printf("\n");
    }
    k = 0;
    for (i = 0; i < 2; i++) {
	mask = (uint64_t)1 << 51;
	parity[i] = 0;
	for (j = 0; j < 52; j++) {
	    if (!IsZero(my.get(0, k))) {
		parity[i] = parity[i] | mask;
	    }
	    mask = mask >> 1;
	    k++;
	}
    }
    printf("parity check vector\n");
    for (i = 0, j = 1; i < 2; i++) {
	printf("p[%d] = 0x%016" PRIx64 "\n", i, parity[j--]);
    }
}

void set_vector(vec_GF2& vec, uint64_t lung[2]) {
    int i, j, k;
    uint64_t mask;

    vec.SetLength(WORD_WIDTH);
    clear(vec);
    k = 0;
    for (i = 1; i >= 0; i--) {
	mask = (uint64_t)1 << 51;
	for (j = 0; j < 52; j++) {
	    if ((lung[i] & mask) != 0) {
		vec.put(k, 1);
	    } else {
		vec.put(k, 0);
	    }
	    k++;
	    mask = mask >> 1;
	}
    }
}

void set_status(in_status *st) {
    int zero_count = 0;
    uint64_t lung[2];

    st->zero = false;
    get_lung(&(st->random), lung);
    set_vector(st->next, lung);
    while (IsZero(st->next)) {
	zero_count++;
	if (zero_count > maxdegree) {
	    st->zero = true;
	    break;
	}
	get_lung(&(st->random), lung);
	set_vector(st->next, lung);
    }
}

void add_status(in_status *dist, in_status *src) {
    add_rnd(&(dist->random), &(src->random));
    dist->next += src->next;
}

void get_next_state(in_status *st) {
    int zero_count = 0;
    uint64_t lung[2];

    if (st->zero) {
	return;
    }
    get_lung(&(st->random), lung);
    set_vector(st->next, lung);
    while (IsZero(st->next)) {
	zero_count++;
	if (zero_count > maxdegree) {
	    st->zero = true;
	    break;
	}
	get_lung(&(st->random), lung);
	set_vector(st->next, lung);
    }
}

void get_base(in_status bases[], int size) {
    vec_GF2 next[size];
    uint8_t dependents[size];
    int index;
    int i;
    int ret;
    int isZero;

    for (;;) {
	isZero = false;
	for (i = 0; i < size; i++) {
	    if (IsZero(bases[i].next)) {
		isZero = true;
		break;
	    }
	    next[i] = bases[i].next;
	}
	if (isZero) {
	    break;
	}
	ret = get_dependent_trans(dependents, next, size);
	if (ret < 0) {
	    break;
	}
#if 0
	fprintf(stderr, "dependents:");
	for (i = 0; i < size; i++) {
	    fprintf(stderr, "%1d", dependents[i]);
	}
	fprintf(stderr, "\n");
#endif
	index = get_dependent_index(dependents, size);
	for (i = 0; i < size; i++) {
	    if (i == index) {
		continue;
	    }
	    if (dependents[i] != 0) {
		add_status(&(bases[index]), &(bases[i]));
	    }
	}
	if (IsZero(bases[index].next)) {
	    get_next_state(&(bases[index]));
	} else {
	    fprintf(stderr, "next is not zero:");
	    for (i = 0; i < WORD_WIDTH; i++) {
		if (IsZero(bases[index].next[i])) {
		    printf("0");
		} else {
		    printf("1");
		}
	    }
	    printf("\n");
	    fprintf(stderr, "ret = %u\n", ret);
	    fprintf(stderr, "index = %u\n", index);
	    exit(1);
	}
    }
}

int get_dependent_index(uint8_t dependents[], int size)
{
    int i;

    for (i = 0; i < size; i++) {
	if (dependents[i] != 0) {
	    return i;
	}
    }
    return 0;
}

int get_dependent_trans(uint8_t dependent[], vec_GF2 array[], int size) {
    mat_GF2 mat;
    int rank;

    convert(mat, array, size);
    rank = gauss(mat);
    if (rank == size) {
	return -1;
    }
    return dependent_rows(dependent, mat);
}

int dependent_rows(uint8_t result[], mat_GF2& mat) {
    int size;
    int index;
    int i, j;

    size = mat.NumCols();
    index = size - 1;
    memset(result, 0, sizeof(char)*(size));
    for (i = 0; i < size; i++) {
	if (IsZero(mat.get(i, i))) {
	    index = i;
	    result[i] = 1;
	    break;
	}
    }
    for (i = index - 1; i >= 0; i--) {
	if (IsOne(mat.get(i, index))) {
	    result[i] = 1;
	    for (j = 0; j <= i; j++) {
		mat.put(j, index, mat.get(j, index) + mat.get(j, i));
	    }
	}
    }
#if 0
    printf("result is \n");
    for (i = 0; i < size; i++) {
	printf("%d", result[i]);
    }
    printf("\n");
#endif
    return 0;
}

void convert(mat_GF2& mat, vec_GF2 array[], int size) {
    int i, j;

    mat.SetDims(WORD_WIDTH, size);
    for (i = 0; i < size; i++) {
	for (j = 0; j < WORD_WIDTH; j++) {
	    mat.put(j, i, array[i].get(j));
	}
    }
}
