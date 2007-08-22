#ifndef DSFMT_PARAMS_H
#define DSFMT_PARAMS_H

#if !defined(SFMT_MEXP)
#ifdef __GNUC__
  #warning "SFMT_MEXP is not defined. I assume SFMT_MEXP is 19937."
#endif
  #define SFMT_MEXP 19937
#endif
/*-----------------
  BASIC DEFINITIONS
  -----------------*/
/** Mersenne Exponent. The period of the sequence 
 *  is a multiple of 2^SFMT_MEXP-1.
 * #define SFMT_MEXP 19937 */
/** SFMT generator has an internal state array of 128-bit integers,
 * and N is its size. */
#define SFMT_N (SFMT_MEXP / 104)
/** N32 is the size of internal state array when regarded as an array
 * of 32-bit integers.*/
#define SFMT_N32 (SFMT_N * 4)
/** N64 is the size of internal state array when regarded as an array
 * of 64-bit integers.*/
#define SFMT_N64 (SFMT_N * 2)

/*----------------------
  the parameters of SFMT
  following definitions are in dSFMT-paramsXXXX.h file.
  ----------------------*/
/** the pick up position of the array.
#define SFMT_POS1 122 
*/

/** the parameter of shift left as four 32-bit registers.
#define SFMT_SL1 18
 */

/** the parameter of shift left as one 128-bit register. 
 * The 128-bit integer is shifted by (SL2 * 8) bits. 
#define SFMT_SL2 1 
*/

/** the parameter of shift right as four 32-bit registers.
#define SFMT_SR1 11
*/

/** the parameter of shift right as one 128-bit register. 
 * The 128-bit integer is shifted by (SL2 * 8) bits. 
#define SFMT_SR2 1 
*/

/** A bitmask, used in the recursion.  These parameters are introduced
 * to break symmetry of SIMD.
#define SFMT_MSK1 (uint64_t)0xdfffffefULL
#define SFMT_MSK2 (uint64_t)0xddfecb7fULL
*/

/** These definitions are part of a 128-bit period certification vector.
#define SFMT_PCV1	UINT64_C(0x00000001)
#define SFMT_PCV2	UINT64_C(0x00000000)
*/

#define SFMT_LOW_MASK  UINT64_C(0x000FFFFFFFFFFFFF)
#define SFMT_LOW_MASK32_1 0x000fffffU
#define SFMT_LOW_MASK32_2 0xffffffffU
#define SFMT_HIGH_CONST UINT64_C(0x3FF0000000000000)
#define SFMT_HIGH_CONST32 0x3ff00000U

/* for sse2 */
#define SSE2_SHUFF 0x4e

#if SFMT_MEXP == 607
  #include "dSFMT-params607.h"
#elif SFMT_MEXP == 1279
  #include "dSFMT-params1279.h"
#elif SFMT_MEXP == 2281
  #include "dSFMT-params2281.h"
#elif SFMT_MEXP == 4423
  #include "dSFMT-params4423.h"
#elif SFMT_MEXP == 11213
  #include "dSFMT-params11213.h"
#elif SFMT_MEXP == 19937
  #include "dSFMT-params19937.h"
#elif SFMT_MEXP == 44497
  #include "dSFMT-params44497.h"
#elif SFMT_MEXP == 86243
  #include "dSFMT-params86243.h"
#elif SFMT_MEXP == 132049
  #include "dSFMT-params132049.h"
#elif SFMT_MEXP == 216091
  #include "dSFMT-params216091.h"
#else
#ifdef __GNUC__
  #error "SFMT_MEXP is not valid."
  #undef SFMT_MEXP
#else
  #undef SFMT_MEXP
#endif

#endif

#endif /* DSFMT_PARAMS_H */
