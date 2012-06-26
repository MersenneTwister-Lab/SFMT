/**
 * @file test-jump.cpp
 *
 * @brief test jump function.
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
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <inttypes.h>
#include <stdint.h>
#include <time.h>
#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/ZZ.h>
#include "SFMT-calc-jump.hpp"
#include "SFMT-jump.h"

using namespace NTL;
using namespace std;
using namespace sfmt;

static void read_file(GF2X& characteristic, int line_no, const string& file);
static void test(sfmt_t * sfmt, GF2X& poly);
static int check(sfmt_t *a, sfmt_t *b);
static void print_state(sfmt_t *a, sfmt_t * b);
static void print_sequence(sfmt_t *a, sfmt_t * b);
static void speed(sfmt_t * sfmt, GF2X& characteristic);

int main(int argc, char * argv[]) {
    if (argc <= 1) {
	printf("%s -s|-c\n", argv[0]);
	return -1;
    }
    stringstream ss_file;
    GF2X characteristic;
    ss_file << "characteristic." << SFMT_MEXP << ".txt";
    read_file(characteristic, 0, ss_file.str());
#if 0
    string step_string = argv[1];
    ZZ step;
    stringstream ss(step_string);
    ss >> step;
    string jump_str;
    calc_jump(jump_str, step, characteristic);

    string tmp;
    polytostring(tmp, characteristic);
    cout << "tmp :" << endl;
    cout << tmp << endl;
    cout << jump_str << endl;
#endif
    sfmt_t sfmt;
    if (argv[1][1] == 's') {
	speed(&sfmt, characteristic);
    } else {
	test(&sfmt, characteristic);
    }
    return 0;
}

static void speed(sfmt_t * sfmt, GF2X& characteristic)
{
    uint32_t seed = 1234;
    long step = 10000;
    int exp = 4;
    ZZ test_count;
    string jump_string;
    clock_t start;
    double elapsed1;
    double elapsed2;

    sfmt_init_gen_rand(sfmt, seed);
    test_count = step;
    for (int i = 0; i < 10; i++) {
	start = clock();
	calc_jump(jump_string, test_count, characteristic);
	elapsed1 = clock() - start;
	elapsed1 = elapsed1 * 1000 / CLOCKS_PER_SEC;
	cout << "mexp "
	     << setw(5)
	     << SFMT_MEXP
	     << " jump 10^"
	     << setfill('0') << setw(2)
	     << exp
	     << " steps calc_jump:"
	     << setfill(' ') << setiosflags(ios::fixed)
	     << setw(6) << setprecision(3)
	     << elapsed1
	     << "ms"
	     << endl;
	start = clock();

	for (int j = 0; j < 10; j++) {
	    SFMT_jump(sfmt, jump_string.c_str());
	}
	elapsed2 = clock() - start;
	elapsed2 = elapsed2 * 1000 / 10 / CLOCKS_PER_SEC;
	cout << "mexp "
	     << setw(5)
	     << SFMT_MEXP
	     << " jump 10^"
	     << setfill('0') << setw(2)
	     << exp
	     << " steps SFMT_jump:"
	     << setfill(' ') << setiosflags(ios::fixed)
	     << setw(6) << setprecision(3)
	     << elapsed2
	     << "ms"
	     << endl;
	test_count *= 100;
	exp += 2;
#if 0
	if (elapsed1 > 200.0) {
	    break;
	}
#endif
    }
}

static void read_file(GF2X& characteristic, int line_no, const string& file)
{
    ifstream ifs(file.c_str());
    string line;
    for (int i = 0; i < line_no; i++) {
	ifs >> line;
	ifs >> line;
    }
    if (ifs) {
	ifs >> line;
	line = "";
	ifs >> line;
    }
    stringtopoly(characteristic, line);
#if defined(DEBUG)
    cout << "line = " << line << endl;
    cout << "deg = " << deg(characteristic) << endl;
    cout << "cha = " << characteristic << endl;
    string x;
    polytostring(x, characteristic);
    cout << "x = " << x << endl;
#endif
}

static int check(sfmt_t *a, sfmt_t *b)
{
    int check = 0;
    for (int i = 0; i < 100; i++) {
	uint32_t x = sfmt_genrand_uint32(a);
	uint32_t y = sfmt_genrand_uint32(b);
	if (x != y) {
	    print_state(a, b);
	    print_sequence(a, b);
	    check = 1;
	    break;
	}
    }
    if (check == 0) {
	printf("OK!\n");
    } else {
	printf("NG!\n");
    }
    return check;
}

static void print_state(sfmt_t *a, sfmt_t * b)
{
    printf("idx = %d                             idx = %d\n",
	   a->idx, b->idx);
    for (int i = 0; (i < 10) && (i < SFMT_N); i++) {
	printf("[");
	for (int j = 0; j < 4; j++) {
	    printf("%08"PRIx32, a->state[i].u[j]);
	    if (j < 3) {
		printf(" ");
	    } else {
		printf("]");
	    }
	}
	printf("[");
	for (int j = 0; j < 4; j++) {
	    printf("%08"PRIx32, b->state[i].u[j]);
	    if (j < 3) {
		printf(" ");
	    } else {
		printf("]");
	    }
	}
	printf("\n");
    }
}

static void print_sequence(sfmt_t *a, sfmt_t * b)
{
    for (int i = 0; i < 25; i++) {
	uint32_t c, d;
	c = sfmt_genrand_uint32(a);
	d = sfmt_genrand_uint32(b);
	printf("[%08"PRIx32" %08"PRIx32"]\n", c, d);
    }
}

static void test(sfmt_t * sfmt, GF2X& characteristic)
{
    sfmt_t new_sfmt_z;
    sfmt_t * new_sfmt = &new_sfmt_z;
//    uint32_t seed[] = {1, 998102, 1234, 0, 5};
    uint32_t seed[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    long steps[] = {1, 2, SFMT_N + 1,
		    SFMT_N * 128 - 1,
		    SFMT_N * 128 + 1,
		    3003,
		    200004,
		    10000005};
    int steps_size = sizeof(steps) / sizeof(long);
    ZZ test_count;
    string jump_string;

    sfmt_init_gen_rand(sfmt, seed[0]);
    sfmt_genrand_uint32(sfmt);
    sfmt_genrand_uint32(sfmt);
    sfmt_genrand_uint32(sfmt);
    sfmt_genrand_uint32(sfmt);
    /* plus jump */
    for (int index = 0; index < steps_size; index++) {
//	sfmt_init_gen_rand(sfmt, seed[index]);
	test_count = steps[index];
	cout << "mexp " << dec << SFMT_MEXP << " jump "
	     << test_count << " steps" << endl;
	*new_sfmt = *sfmt;
	for (long i = 0; i < steps[index] * 4; i++) {
	    sfmt_genrand_uint32(sfmt);
	}
	calc_jump(jump_string, test_count, characteristic);
#if defined(DEBUG)
	cout << "jump string:" << jump_string << endl;
	cout << "before jump:" << endl;
	print_state(new_sfmt, sfmt);
#endif
	SFMT_jump(new_sfmt, jump_string.c_str());
#if defined(DEBUG)
	cout << "after jump:" << endl;
	print_state(new_sfmt, sfmt);
#endif
	if (check(new_sfmt, sfmt)) {
	    return;
	}
    }
}
