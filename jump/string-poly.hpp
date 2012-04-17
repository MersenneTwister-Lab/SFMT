#ifndef STRING_POLY_HPP
#define STRING_POLY_HPP
/**
 * @file string-poly.hpp
 *
 * @brief exteted SFMT class for calculating characteristic polynomial.
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
#include <NTL/GF2X.h>

static inline void polytostring(std::string& x, NTL::GF2X& characteristic)
{
    using namespace NTL;
    using namespace std;

    long degree = deg(characteristic);
    int size = degree / 64 + 1;
    uint64_t buffer[size];
    for (int i = 0; i < size; i++) {
	buffer[i] = 0;
    }
    for (int i = 0; i <= degree; i++) {
	int pos = i / 64;
	int shift = i % 64;
	if (IsOne(coeff(characteristic, i))) {
	    buffer[pos] |= UINT64_C(1) << shift;
	}
    }
    ostringstream os;
    for (int i = 0; i < size; i++) {
	os << hex << setw(16) << setfill('0') << buffer[i];
    }
    os << flush;
    x = os.str();
}

static inline void stringtopoly(NTL::GF2X& poly, std::string& str)
{
    using namespace NTL;
    using namespace std;

    stringstream ss(str);
    char temp[17];
    int length = str.length();
    int size = length / 16 + 1;
    uint64_t work[size];

    for (int i = 0; i < size; i++) {
	work[i] = 0;
    }
    temp[16] = '\0';
    for (int i = 0; i < size; i++) {
	for (int j = 0; j < 16; j++) {
	    temp[j] = '\0';
	    if (ss) {
		ss >> temp[j];
	    } else {
		break;
	    }
	}
	work[i] = strtoull(temp, NULL, 16);
    }
    for (int i = 0; i < size; i++) {
	for (int j = 0; j < 64; j++) {
	    int pos = i * 64 + j;
	    uint64_t mask = UINT64_C(1) << j;
	    if ((work[i] & mask) != 0) {
		SetCoeff(poly, pos, 1);
	    } else {
		SetCoeff(poly, pos, 0);
	    }
	}
    }
}

#endif
