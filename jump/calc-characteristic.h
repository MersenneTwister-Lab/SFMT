#pragma onece
#ifndef CALC_CHARACTERISTIC_H
#define CALC_CHARACTERISTIC_H
/**
 * @file SFMText.hpp
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
#include <stdexcept>
#include <stdint.h>
#include <inttypes.h>
#include <inttypes.h>
#include <stdint.h>
#include "SFMText.hpp"
#include "SFMT-calc-jump.hpp"
#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/GF2XFactoring.h>

namespace sfmt {
    void get_characteristic(GF2X& lcmpoly, SFMText& sfmt);
}
