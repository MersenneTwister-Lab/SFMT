/* 
 * This program is for searching initial value of lung
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>

extern "C" {
#include "sfmt-st.h"
#include "mt19937ar.h"
}

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>
#include "util.h"

NTL_CLIENT;

/* internal status */
struct IN_STATUS {
    bool zero;
    unsigned int next;
    sfmt_t random;
};
typedef struct IN_STATUS in_status;

#define NTH_BIT2(i) (1U << (31 - (i)))

void search_lung (GF2X& f);
void set_status(in_status *st);
void add_status(in_status *dist, in_status *src);
void get_next_state(in_status *st);
void get_base(in_status bases[], uint32_t size);
int get_dependent_index(uint8_t dependents[], in_status bases[], uint32_t size);
int get_dependent_trans(uint8_t dependent[], uint32_t array[], uint32_t size);
int dependent_rows(uint8_t result[], mat_GF2& mat);
void convert(mat_GF2& mat, unsigned int array[], uint32_t bit_len);
void make_zero_state(sfmt_t *sfmt, GF2X& poly);
void check_init_lung(in_status *st, int size);

static uint32_t mexp;
static uint32_t maxdegree;

int main(int argc, char *argv[]) {
    GF2X f;
    FILE *fp;

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
    readFile(f, fp);
    printf("deg poly = %ld\n", deg(f));
    fclose(fp);
    search_lung(f);
    return 0;
}

void set_bit(in_status *st, GF2X& f, uint32_t bit_pos) {
    uint32_t i, j;

    memset(st, 0, sizeof(in_status));
    for (;;) {
	for (i = 0; i < N; i++) {
	    for (j = 0; j < 4; j++) {
		st->random.sfmt[i][j] = genrand_int32();
	    }
	}
	make_zero_state(&(st->random), f);
	set_status(st);
	if (!st->zero) {
	    break;
	}
    }
}

void make_zero_state(sfmt_t *sfmt, GF2X& poly) {
  static sfmt_t sfmtnew;
  uint64_t hi, low;
  int i;

  memset(&sfmtnew, 0, sizeof(sfmtnew));
  for (i = 0; i <= deg(poly); i++) {
    if (coeff(poly, i) != 0) {
	add_rnd(&sfmtnew, sfmt);
    }
    gen_rand128(sfmt, &hi, &low);
  }
  *sfmt = sfmtnew;
}

void search_lung (GF2X& f) {
    static in_status bases[32];
    int i, j;
    int count;
    int bit_pos;
    int size = 2;
    int base_num = (32 - mexp % 32);

    set_bit(&(bases[0]), f, 0);
    set_bit(&(bases[1]), f, 1);
    bit_pos = 2;
    while(size <= base_num) {
	get_base(bases, size);
	count = 0;
	for (i = 0; i < size; i++) {
	    if (!bases[i].zero) {
		count++;
	    }
	}
	if (count == size) {
	    if (size + 1 <= base_num) {
		set_bit(&bases[size++], f, bit_pos++);
	    } else {
		break;
	    }
	} else {
	    for (i = 0; i < size; i++) {
		if (bases[i].zero) {
		    set_bit(&bases[i], f, bit_pos++);
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
    for (i = 0; i < base_num; i++) {
	for (j = 0; j < 32; j++) {
	    printf("%d", (bases[i].next >> (31 - j)) & 1);
	}
	printf("\n");
    }
    check_init_lung(bases, base_num);
}

void check_init_lung(in_status base[], int size) {
    static char* initial[] = {"MSNH", "MMTM", "SFMT",
			      "msnh", "mmtm", "sfmt",
			      NULL};
    mat_GF2 matrix;
    int i, j;
    int rank;
    uint32_t lung;

    matrix.SetDims(size + 1, 32);
    for (i = 0; i < size; i++) {
	for (j = 0; j < 32; j++) {
	    matrix.put(i, j, (base[i].next >> (31 - j)) & 1);
	}
    }
    printf("-----\n");
    for (i = 0; initial[i] != NULL; i++) {
	lung = 0;
	for (j = 0; j < 4; j++) {
	    lung |= initial[i][j] << ((3 - j) * 8);
	}
	for (j = 0; j < 32; j++) {
	    matrix.put(size, j, (lung >> (31 - j)) & 1);
	}
	rank = gauss(matrix);
	if (rank == size + 1) {
	    printf("initial lung = %08x(%s)\n", lung, initial[i]);
	    return;
	}
    }
    printf("initial lung can't find\n");
}

void set_status(in_status *st) {
    uint32_t zero_count = 0;
    st->zero = false;
    st->next = get_lung(&(st->random));
    while (st->next == 0) {
	zero_count++;
	if (zero_count > maxdegree) {
	    st->zero = true;
	    break;
	}
	st->next = get_lung(&(st->random));
    }
}

void add_status(in_status *dist, in_status *src) {
    unsigned int n;

    n = dist->next;
    add_rnd(&(dist->random), &(src->random));
    dist->next = n ^ (src->next);
}

void get_next_state(in_status *st) {
    uint32_t zero_count = 0;

    if (st->zero) {
	return;
    }
    st->next = get_lung(&(st->random));
    while (st->next == 0) {
	zero_count++;
	if (zero_count > maxdegree) {
	    st->zero = true;
	    break;
	}
	st->next = get_lung(&(st->random));
    }
}

void get_base(in_status bases[], unsigned int size) {
    uint32_t next[size];
    uint8_t dependents[size];
    unsigned int index;
    unsigned int i;
    int ret;
    int isZero;

    for (;;) {
	isZero = false;
	for (i = 0; i < size; i++) {
	    if (bases[i].next == 0) {
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
	index = get_dependent_index(dependents, bases, size);
	for (i = 0; i < size; i++) {
	    if (i == index) {
		continue;
	    }
	    if (dependents[i] != 0) {
		add_status(&(bases[index]), &(bases[i]));
	    }
	}
	if (bases[index].next == 0) {
	    get_next_state(&(bases[index]));
	} else {
	    fprintf(stderr, "next is not zero:%x\n", bases[index].next);
	    fprintf(stderr, "ret = %u\n", ret);
	    fprintf(stderr, "index = %u\n", index);
	    exit(1);
	}
    }
}

int get_dependent_index(uint8_t dependents[], in_status bases[], uint32_t size)
{
    uint32_t i;

    for (i = 0; i < size; i++) {
	if (dependents[i] != 0) {
	    return i;
	}
    }
    return 0;
}

int get_dependent_trans(uint8_t dependent[], uint32_t array[], 
			uint32_t size) {
    mat_GF2 mat;
    uint32_t rank;

    convert(mat, array, size);
    rank = (uint32_t) gauss(mat);
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

void convert(mat_GF2& mat, uint32_t array[], uint32_t size) {
    uint32_t msk;
    uint32_t i, j;

    mat.SetDims(32, size);
    for (i = 0; i < size; i++) {
	for (j = 0; j < 32; j++) {
	    msk = NTH_BIT2(j);
	    if ((array[i] & msk) != 0) {
		mat.put(j, i, 1);
	    } else {
		mat.put(j, i, 0);
	    }
	}
    }
}
