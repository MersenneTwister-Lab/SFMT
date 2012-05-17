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
#include <string>
#include <inttypes.h>
#include <stdint.h>
#include <NTL/GF2X.h>
#include "SFMT-calc-jump.hpp"

using namespace NTL;
using namespace std;
using namespace sfmt;

int main(int argc, char * argv[]) {
    if (argc <= 1) {
	cout << argv[0] << " deg" << endl;
	return -1;
    }
    string step_string = argv[1];
    GF2X poly;
    uint64_t d = strtoull(argv[1], NULL, 10);
    SetCoeff(poly, d, 1);
    string tmp;
    polytostring(tmp, poly);
    cout << "string: " << tmp << endl;
    cout << "poly: " << poly << endl;
    GF2X r;
    GF2X s;
    string diff;
    for (int i = 0; i < 10; i++) {
	random(r, 1234);
	polytostring(tmp, r);
	cout << "1:" << tmp << endl;
	stringtopoly(s, tmp);
	polytostring(diff, s);
	cout << "2:" <<diff << endl;
	if (r != s) {
	    cout << "bad!" << endl;
	    long degree = deg(r);
	    if (degree != deg(s)) {
		cout << "deg(s) = " << deg(s)
		     << " deg(r) = " << deg(r) << endl;
		break;
	    }
	}
	if (tmp != diff) {
	    cout << "bad!" << endl;
	    int len = tmp.length();
	    for (int j = 0; j < len; j++) {
		cout << "[" << tmp[j] << diff[j] << "]";
		if (tmp[j] != diff[j]) {
		    cout << "!" << endl;
		    break;
		}
	    }
	    break;
	}
    }
    return 0;
}
