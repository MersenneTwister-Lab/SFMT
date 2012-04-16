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

using namespace NTL;
using namespace std;

void stringtopoly(GF2X& poly, string& str) {
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
	    ss >> temp[j];
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

int main(int argc, char * argv[]) {
    string chara_string = "0800000000000001488431882308c4212c2f488951440612c9946ea5e4133b8caf1f05e78676cf9db5a91c1719cf55c35770203f6d5a323826f6c94479ffdfb286409ac8d1d964d517dd7ca5993c1e77406a900b9fbf03097db8c43cb0dcf6a08aa07486bf1ded65d9b97a474cd7bb4c75c2cb8d701a0a017b25ee68e18b54da3fa6ef12dca7fc36393b719c61a30d5eb3e75488053e83260000000200c5b1ac0000000000000001";
    GF2X characteristic;
    stringtopoly(characteristic, chara_string);
    cout << characteristic << endl;
    ZZ step;
    stringstream ss("100000");
    ss >> step;
    GF2X jump;
    PowerXMod(jump, step, characteristic);
    cout << jump << endl;
    return 0;
}
