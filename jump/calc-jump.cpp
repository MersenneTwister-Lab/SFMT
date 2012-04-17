/**
 * @file calc-jump.cpp
 *
 * @brief calculate jump polynomial.
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
#include <iomanip>
#include <sstream>
#include <string>
#include <inttypes.h>
#include <stdint.h>
#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/ZZ.h>
#include "string-poly.hpp"

using namespace NTL;
using namespace std;

/**
 * The internal state jump.
 * SFMT generates 4 32-bit integers from one internal state.
 * @param jump_str output string which represents jump polynomial.
 * @param step jump step of internal state
 */
void calc_jump(string& jump_str, ZZ& step, GF2X& characteristic)
{
    GF2X jump;
    PowerXMod(jump, step, characteristic);
    polytostring(jump_str, jump);
}

int main(int argc, char * argv[]) {
    if (argc <= 2) {
	cout << argv[0] << " characteristic-polynomial jumpstep" << endl;
	return -1;
    }
    string chara_string = argv[1];
    string step_string = argv[2];
    GF2X characteristic;
    stringtopoly(characteristic, chara_string);
    ZZ step;
    stringstream ss(step_string);
    ss >> step;
    string jump_str;
    calc_jump(jump_str, step, characteristic);
    cout << jump_str << endl;
    return 0;
}
