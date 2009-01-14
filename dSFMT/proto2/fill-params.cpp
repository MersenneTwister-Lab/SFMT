/* 
 * Search Parity Check Vector
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>
#include <NTL/GF2XFactoring.h>

#include "util.h"
#include "dsfmt.h"
#include "dsfmt-util.h"

extern "C" {
    #include "mt19937blk.h"
}

NTL_CLIENT;

const int WORD_WIDTH = 128;

void search_lung (GF2X& f, uint64_t parity[2]);
void get_base(vec_GF2 bases[], int size);
int get_dependent_trans(uint8_t dependent[], vec_GF2 array[], int size);
int dependent_rows(uint8_t result[], mat_GF2& mat);
void convert(mat_GF2& mat, vec_GF2 array[], int bit_len);
void search_parity_check_vector(uint64_t parity[2], vec_GF2 base[], int size);
void set_vector(vec_GF2& vec, DSFMT& dsfmt);
static void test_parity0(GF2X& f, uint64_t parity[2]);
void get_lcm(GF2X& lcmpoly, const GF2X& poly);
int fill_state_random(DSFMT& sfmt);
bool calc_fixpoint(uint64_t fix[2], GF2X& small);
void show_factorial(GF2X& pol);
bool chk_fix(DSFMT& fix, DSFMT& con, GF2X& idf);

static int mexp;
static int maxdegree;
static int verbose = false;
static bool debug = false;
static GF2X mexpirr;

int main(int argc, char *argv[]) {
    GF2X poly;
    GF2X lcmpoly;
    GF2X smallpoly;
    GF2X rempoly;
    FILE *fp;
    uint64_t parity[2];
    uint64_t fix[2];

    if (argc != 2) {
	printf("usage:%s filename %d\n", argv[0], argc);
	exit(1);
    }
    mexp = DSFMT::get_rnd_mexp();
    maxdegree = DSFMT::get_rnd_maxdegree();
    printf("mexp = %d\n", mexp);
    printf("filename:%s\n", argv[1]);
    fp = fopen(argv[1], "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("main");
	fclose(fp);
	exit(1);
    }
    DSFMT::read_random_param(fp);
    readFile(poly, fp, true);
    fclose(fp);
    search_lung(poly, parity);
    DSFMT::set_pcv(parity);

    get_lcm(lcmpoly, poly);
    mexpirr = poly;
    DivRem(smallpoly, rempoly, lcmpoly, poly);

    //printf("deg small poly = %ld\n", deg(smallpoly));
    //printf("deg lcm poly = %ld\n", deg(lcmpoly));
    if (!calc_fixpoint(fix, smallpoly)) {
	printf("can't find fix point\n");
    }
    DSFMT::set_fix(fix);

    DSFMT::print_param(stdout);
    printf("deg poly = %ld\n", deg(poly));
    printBinary(stdout, poly);
    test_parity0(poly, parity);
    return 0;
}

int fill_state_random(DSFMT& sfmt) {
    static int count = 0;

    if (count >= maxdegree) {
	return 0;
    }
    sfmt.fill_rnd_all(count);
    count++;
    return 1;
}

void get_lcm(GF2X& lcmpoly, const GF2X& poly) {
    GF2X minpoly;
    GF2X tmp;
    GF2X rempoly;
    vec_GF2 vec;
    DSFMT sfmt(123);
    int i;

    vec.SetLength(2 * maxdegree + 1);
    lcmpoly = 1;
    for (i = 0; i < 104; i++) {
	generating_polynomial104(sfmt, vec, i, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	DivRem(tmp, rempoly, minpoly, poly);
	if (deg(rempoly) != -1) {
	    printf("get_lcm:rem != 0 deg rempoly = %ld: i = %d\n",
		   deg(rempoly), i);
	    printf("deg minpoly = %ld\n", deg(lcmpoly));
	    show_factorial(lcmpoly);
	    return;
	}
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
	// div_check(lcmpoly, poly, i);// for debug
    }
    for (i = 0; i < maxdegree && (int)deg(lcmpoly) < maxdegree; i++) {
	fill_state_random(sfmt);
	generating_polynomial104(sfmt, vec, i % 104, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	DivRem(tmp, rempoly, minpoly, poly);
	if (deg(rempoly) != -1) {
	    printf("get_lcm:rem != 0 deg rempoly = %ld: i = %d\n",
		   deg(rempoly), i);
	    printf("deg minpoly = %ld\n", deg(lcmpoly));
	    show_factorial(lcmpoly);
	    return;
	}
	LCM(tmp, lcmpoly, minpoly);
	lcmpoly = tmp;
    }
}

static void test_parity0(GF2X& f, uint64_t parity[2]) {
    DSFMT dsfmt;
    GF2X minpoly;
    GF2X q, rem;
    vec_GF2 vec;
    int i;
    int r;
    int result = 0;
    int count;

    if (verbose) {
	count = 10;
    } else {
	count = 100;
    }
    printf("start parity zero\n");
    DSFMT::set_pcv(parity);
    for (i = 0; i < count; i++) {
	if (verbose) printf("------\n");
	if (verbose) printf("==shoki (%d)\n", i);
	dsfmt.init_gen_rand(i + 1, 0);
	vec.SetLength(4 * maxdegree);
	generating_polynomial104(dsfmt, vec, 0, 2 * maxdegree);
	berlekampMassey(minpoly, 2 * maxdegree, vec);
	DivRem(q, rem, minpoly, f);
	if (deg(rem) != -1) {
	    printf("minpoly = %ld\n", deg(minpoly));
	    printf("rem != 0 deg rempoly = %ld\n", deg(rem));
	    printf("deg q = %ld\n", deg(q));
	    result = 0;
	    break;
	}
	if (verbose || deg(minpoly) < mexp) {
	    printf("minpoly = %ld\n", deg(minpoly));
	}
	if (deg(minpoly) < mexp) {
	    result = 0;
	    break;
	}
	r = dsfmt.period_certification(true);
	if (r == 1) {
	    if (verbose) printf("period certification OK\n");
	} else {
	    if (verbose) printf("period certification NG -> OK\n");
	    if (!dsfmt.period_certification(true)) {
		result = 0;
		printf("period critification didn't change status!!\n");
		break;
	    }
	}
	//dsfmt.fill_rnd(0);
	dsfmt.init_gen_rand(i + 3, 0);
	//dsfmt.d_p();
	make_zero_state(dsfmt, f);
	if (verbose) printf("==zero\n");
	generating_polynomial104(dsfmt, vec, 0, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	if (verbose || deg(minpoly) >= mexp) {
	    printf("minpoly = %ld\n", deg(minpoly));
	}
	if (deg(minpoly) >= mexp) {
	    printf("make zero state failed\n");
	    result = 0;
	    break;
	}
	//dsfmt.d_p();
	r = dsfmt.period_certification(true);
	if (r == 1) {
	    if (verbose) printf("period certification OK [ERROR]\n");
	    dsfmt.d_p();
	    result = 0;
	    break;
	} else {
	    if (verbose) printf("period certification NG -> OK\n");
	    if (!dsfmt.period_certification(true)) {
		result = 0;
		printf("period certification didn't chanege status!!\n");
		break;
	    }
	}
	generating_polynomial104(dsfmt, vec, 0, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	if (verbose || deg(minpoly) < mexp) {
	    printf("minpoly = %ld\n", deg(minpoly));
	}
	if (deg(minpoly) < mexp) {
	    result = 0;
	    break;
	}
	r = dsfmt.period_certification(true);
	if (r == 1) {
	    if (verbose) printf("period certification OK\n");
	} else {
	    if (verbose) printf("period certification NG -> OK\n");
	    if (!dsfmt.period_certification(true)) {
		printf("error!!\n");
		return;
	    }
	}
	result++;
    }
    if (result) {
	printf("test successed %d / %d\n", result, count);
    } else {
	printf("test failed at count %d\n", count);
    }
}

void chk_minpoly(DSFMT& dsfmt) {
    DSFMT chk(dsfmt);
    GF2X minpoly;
    vec_GF2 vec;

    vec.FixLength(2 * maxdegree);
    generating_polynomial104(chk, vec, 0, maxdegree);
    berlekampMassey(minpoly, maxdegree, vec);
    printf("deg minpoly = %d\n", (int)deg(minpoly));
}

void set_bit(vec_GF2& vec, GF2X& f, int *bit_pos) {
    DSFMT dsfmt;
    for (;*bit_pos <= maxdegree;) {
	dsfmt.fill_rnd_all(*bit_pos);
	(*bit_pos)++;
	make_zero_state(dsfmt, f);
	set_vector(vec, dsfmt);
	if (!IsZero(vec)) {
	    break;
	}
    }
}

void search_lung (GF2X& f, uint64_t parity[2]) {
    static vec_GF2 bases[WORD_WIDTH];
    int i;
    int count;
    int bit_pos = 0;
    int size = 2;
    int base_num = maxdegree - mexp;

    set_bit(bases[0], f, &bit_pos);
    set_bit(bases[1], f, &bit_pos);
    //while(size <= base_num) {
    while((bit_pos < maxdegree) && (size <= base_num)) {
	get_base(bases, size);
	count = 0;
	for (i = 0; i < size; i++) {
	    if (!IsZero(bases[i])) {
		count++;
	    }
	}
	if (count == size) {
	    if (size + 1 <= base_num) {
		set_bit(bases[size], f, &bit_pos);
		size++;
	    } else {
		break;
	    }
	} else {
	    for (i = 0; i < size; i++) {
		if (IsZero(bases[i])) {
		    set_bit(bases[i], f, &bit_pos);
		}
	    }
	}
#if 0
	fprintf(stderr, "size = %d\n", size);
	fprintf(stderr, "count = %d\n", count);
	fprintf(stderr, "bit_pos = %d\n", bit_pos);
#endif
    }
#if 0
    for (i = 0; i < size; i++) {
	chk_minpoly(*bases[i].dsfmt);
    }
#endif
#if 0
    printf("----\n");
    for (i = 0; i < size; i++) {
	for (j = 0; j < WORD_WIDTH; j++) {
	    if (IsZero(bases[i][j])) {
		printf("0");
	    } else {
		printf("1");
	    }
	}
	printf("\n");
    }
#endif
    search_parity_check_vector(parity, bases, size);
}

#include <inttypes.h>

void search_parity_check_vector(uint64_t parity[2], vec_GF2 base[], int size) {
    mat_GF2 mx;
    mat_GF2 my;
    int i, j;

    mx.SetDims(WORD_WIDTH, size);
    for (i = 0; i < WORD_WIDTH; i++) {
	for (j = 0; j < size; j++) {
	    mx.put(i, j, base[j][i]);
	}
    }
    kernel(my, mx);
    if (my.NumRows() == 0) {
	printf("initial lung can't find\n");
	return;
    }
#if 0
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
#endif
    vec_to_uint128(parity, my[0], 64);
#if 0
    printf("parity check vector\n");
    for (i = 0; i < 2; i++) {
	printf("p[%d] = 0x%016" PRIx64 "\n", i, parity[i]);
    }
#endif
}

void set_vector(vec_GF2& vec, DSFMT& dsfmt) {
    int zero_count = 0;
    uint64_t lung[2];
    uint64_t ar[1][2];

    dsfmt.gen_rand104spar(ar, 1);
    dsfmt.get_lung(lung);
    uint128_to_vec(vec, lung, 64);
    while (IsZero(vec)) {
	zero_count++;
	if (zero_count > maxdegree / 100) {
	    break;
	}
	dsfmt.gen_rand104spar(ar, 1);
	dsfmt.get_lung(lung);
	uint128_to_vec(vec, lung, 64);
    }
}

void get_base(vec_GF2 bases[], int size) {
    uint8_t dependents[size];
    int ret;
    int i;

    ret = get_dependent_trans(dependents, bases, size);
    if (ret < 0) {
	return;
    }
    for (i = 0; i < size; i++) {
	if (dependents[i] != 0) {
	    clear(bases[i]);
	    break;
	}
    }
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

bool calc_fixpoint(uint64_t fix[2], GF2X& small) {
    GF2X a, b, d, idf;
    GF2X t1(1, 1);
    DSFMT sfmt_const0(123);
    DSFMT sfmt_const(123);
    DSFMT sfmt_const2(123);
    DSFMT const_L_save(123);

    if (debug) {
	printf("calc_fixpoint: small=");
	printBinary(stdout, small);
    }
    SetCoeff(t1, 0);
    /* a*poly + b*smallpoly = d */
    XGCD(d, a, b, mexpirr, small);
    if (deg(d) != 0) {
	printf("failure d != 1\n");
	return false;
    }
    b *= small;
    idf = b;
    a *= mexpirr;
    sfmt_const.set_const();
    sfmt_const0 = sfmt_const;
    make_zero_state(sfmt_const, b);
    const_L_save = sfmt_const;
    sfmt_const2.set_const();
    make_zero_state(sfmt_const2, a);
    sfmt_const2.add(sfmt_const);
    if (!(sfmt_const2 == sfmt_const0)) {
	if (debug) {
	    printf("modoranai\n");
	    sfmt_const2.add(sfmt_const0);
	    sfmt_const2.d_p();
	}
	return false;
    }

    /* a*poly + b*t1 = d */
    XGCD(d, a, b, mexpirr, t1);
    if (deg(d) != 0) {
	printf("failure d != 1\n");
	return false;
    }
    make_zero_state(sfmt_const, b);
    sfmt_const.get_lung(fix);
    //printf("fix1 0x%16"PRIx64"\n", fix[0]);
    //printf("fix2 0x%16"PRIx64"\n", fix[1]);
    if (chk_fix(sfmt_const, const_L_save, idf)) {
	return true;
    }
    return false;
}

void show_factorial(GF2X& pol) {
    vec_pair_GF2X_long factors;
    int i;
    
    CanZass(factors, pol);
    for (i = 0; i < factors.length(); i++) {
	printf("deg = %d, mul = %d",
	       (int)deg(factors[i].a), (int)factors[i].b);
	if (deg(factors[i].a) > 30) {
	    printf("\n");
	} else {
	    printf(" : ");
	    printBinary(stdout, factors[i].a);
	}
    }
}

bool chk_fix(DSFMT& fix, DSFMT& con, GF2X& idf) {
    DSFMT tmp;
    uint64_t ar[1][2];

    if (debug) printf("===chk fix start ===\n");
    if (debug) printf("deg min fix = %d\n", deg_min_pol(fix, maxdegree));
    if (debug) fix.d_p();
    if (debug) printf("deg min con = %d\n", deg_min_pol(con, maxdegree));
    if (debug) con.d_p();
    tmp = fix;
    tmp.gen_rand104spar(ar, 1);
    if (debug) printf("deg min tmp after gen = %d\n",
		      deg_min_pol(tmp, maxdegree));
    if (debug) tmp.d_p();
    make_zero_state(tmp, idf);
    tmp.add(con);
    if (debug) tmp.d_p();
    if (debug) printf("===chk fix end ===\n");
    return (tmp == fix);
}
