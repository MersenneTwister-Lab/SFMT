/**
 * @file calc-characteristic.cpp
 *
 * @brief calculate characteristic polynomial.
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
#include "SFMText.hpp"
#include "SFMT-calc-jump.hpp"
#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/GF2XFactoring.h>

namespace sfmt {
    using namespace NTL;
    using namespace std;
    static void calc_minimal(GF2X& minimal, SFMText& sfmt, int bitpos)
    {
	uint32_t mask[4];
	for (int i = 0; i < 4; i++) {
	    mask[i] = 0;
	}
	int pos = bitpos / 32;
	uint32_t m = 1 << (bitpos % 32);
	mask[pos] = m;
	int maxdegree = sfmt.get_maxdegree();
	vec_GF2 seq;
	seq.SetLength(2 * maxdegree);
	for (int i = 0; i < 2 * maxdegree; i++) {
	    seq[i] = sfmt.next(mask);
	}
	MinPolySeq(minimal, seq, maxdegree);
#ifdef DEBUG
	if (deg(minimal) == 0) {
	    cout << "deg minimal:" << dec << deg(minimal) << endl;
	    cout << "minimal:" << minimal << endl;
	    cout << "seq:" << seq << endl;
	}
#endif
    }

    static void LCM(GF2X& lcm, const GF2X& x, const GF2X& y) {
	GF2X gcd;
	mul(lcm, x, y);
	GCD(gcd, x, y);
	lcm /= gcd;
    }

    void get_characteristic(GF2X& lcmpoly, SFMText& sfmt) {
	GF2X minimal;
	GF2X tmp;
	int maxdegree = sfmt.get_maxdegree();
	sfmt.seeding(1234);
	for (int bitpos = 0; bitpos < 128; bitpos++) {
	    calc_minimal(minimal, sfmt, bitpos);
	    LCM(tmp, lcmpoly, minimal);
	    lcmpoly = tmp;
	    if (deg(lcmpoly) == maxdegree) {
		return;
	    }
	}
	for (int i = 0; i < maxdegree; i++) {
	    sfmt.init_basis();
	    for (int bitpos = 0; bitpos < 128; bitpos++) {
		calc_minimal(minimal, sfmt, bitpos);
		LCM(tmp, lcmpoly, minimal);
		lcmpoly = tmp;
		if (deg(lcmpoly) == maxdegree) {
		    return;
		}
	    }
	}
	cerr << "deg:" << deg(lcmpoly) << endl;
	throw new logic_error("can't find lcm");
    }

    static void check(SFMText& sfmt) {
	sfmt.seeding(1234);

	for (int i = 0; i < 10; i++) {
	    w128_t x = sfmt.next();
	    for (int j = 0; j < 4; j++) {
		cout << dec << x.u[j] << endl;
	    }
	}
    }

    static int has_large_irreducible(GF2X& fpoly, int degree) {
	static const GF2X t2(2, 1);
	static const GF2X t1(1, 1);
	GF2X t2m;
	GF2X t;
	GF2X alpha;
	int m;

	t2m = t2;
	if (deg(fpoly) < degree) {
	    return 0;
	}
	t = t1;
	t += t2m;

	for (m = 1; deg(fpoly) > degree; m++) {
	    for(;;) {
		GCD(alpha, fpoly, t);
		if (IsOne(alpha)) {
		    break;
		}
		fpoly /= alpha;
		if (deg(fpoly) < degree) {
		    return 0;
		}
	    }
	    t2m *= t2m;
	    t2m %= fpoly;
	    add(t, t2m, t1);
	}
	if (deg(fpoly) != degree) {
	    return 0;
	}
	return IterIrredTest(fpoly);
    }
}
#if defined(MAIN)
using namespace sfmt;
using namespace NTL;
using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 11) {
	cout << argv[0]
	     << " mexp sl1 sl2 sr1 sr2 pos1 mask1 mask2 mask3 mask4"
	     << " parity1 parity2 parity3 parity4"
	     << endl;
	return -1;
    }
    int mexp = strtol(argv[1], NULL, 10);
    int pos1 = strtol(argv[2], NULL, 10);
    int sl1 = strtol(argv[3], NULL, 10);
    int sl2 = strtol(argv[4], NULL, 10);
    int sr1 = strtol(argv[5], NULL, 10);
    int sr2 = strtol(argv[6], NULL, 10);
    uint32_t mask[4];
    mask[0] = strtoull(argv[7], NULL, 16);
    mask[1] = strtoull(argv[8], NULL, 16);
    mask[2] = strtoull(argv[9], NULL, 16);
    mask[3] = strtoull(argv[10], NULL, 16);
    uint32_t parity[4];
    parity[0] = strtoull(argv[11], NULL, 16);
    parity[1] = strtoull(argv[12], NULL, 16);
    parity[2] = strtoull(argv[13], NULL, 16);
    parity[3] = strtoull(argv[14], NULL, 16);
#if defined(DEBUG)
    cout << "mexp:" << dec << mexp << endl;
    cout << "pos1:" << dec << pos1 << endl;
    cout << "sl1:" << dec << sl1 << endl;
    cout << "sl2:" << dec << sl2 << endl;
    cout << "sr1:" << dec << sr1 << endl;
    cout << "sr2:" << dec << sr2 << endl;
    cout << "mask1:" << hex<< mask[0] << endl;
    cout << "mask2:" << hex << mask[1] << endl;
    cout << "mask3:" << hex << mask[2] << endl;
    cout << "mask4:" << hex << mask[3] << endl;
    cout << "parity1:" << hex<< parity[0] << endl;
    cout << "parity2:" << hex << parity[1] << endl;
    cout << "parity3:" << hex << parity[2] << endl;
    cout << "parity4:" << hex << parity[3] << endl;
#endif
    SFMText sfmt(mexp, sl1, sl2, sr1, sr2, pos1,
		 mask, parity);
    GF2X characteristic(0, 1);
#if defined(DEBUG)
    check(sfmt);
#endif
    get_characteristic(characteristic, sfmt);
    GF2X work;
    work = characteristic;
#if defined(DEBUG)
    cout << "degree:" << deg(characteristic) << endl;
    cout << characteristic << endl;
#endif
    if (!has_large_irreducible(characteristic, mexp)) {
	cout << "error?" << endl;
	return -1;
    }
    string x;
    polytostring(x, work);
#if defined(DEBUG)
    cout << "characteristic:" << x << endl;
#endif
    cout << "#" << dec << mexp;
    cout << "," << dec << pos1;
    cout << "," << dec << sl1;
    cout << "," << dec << sl2;
    cout << "," << dec << sr1;
    cout << "," << dec << sr2;
    cout << "," << hex << mask[0];
    cout << "," << hex << mask[1];
    cout << "," << hex << mask[2];
    cout << "," << hex << mask[3];
    cout << "," << hex << parity[0];
    cout << "," << hex << parity[1];
    cout << "," << hex << parity[2];
    cout << "," << hex << parity[3];
    cout << endl;
    cout << x << endl;
    cout << dec << flush;
    return 0;
}
#endif
