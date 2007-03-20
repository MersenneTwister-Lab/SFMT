#ifndef DSFMT_PARAMS_H
#define DSFMT_PARAMS_H

#if !defined(MEXP)
#ifdef __GNUC__
  #warning "MEXP is not defined. I assume MEXP is 19937."
#endif
  #define MEXP 19937
#endif
/*-----------------
  BASIC DEFINITIONS
  -----------------*/
/** Mersenne Exponent. The period of the sequence 
 *  is a multiple of 2^MEXP-1.
 * #define MEXP 19937 */
/** SFMT generator has an internal state array of 128-bit integers,
 * and N is its size. */
#define N (MEXP / 104)
/** N32 is the size of internal state array when regarded as an array
 * of 32-bit integers.*/
#define N32 (N * 4)
/** N64 is the size of internal state array when regarded as an array
 * of 64-bit integers.*/
#define N64 (N * 2)

/*----------------------
  the parameters of SFMT
  following definitions are in dSFMT-paramsXXXX.h file.
  ----------------------*/
/** the pick up position of the array.
#define POS1 122 
*/

/** the parameter of shift left as four 32-bit registers.
#define SL1 18
 */

/** the parameter of shift left as one 128-bit register. 
 * The 128-bit integer is shifted by (SL2 * 8) bits. 
#define SL2 1 
*/

/** the parameter of shift right as four 32-bit registers.
#define SR1 11
*/

/** the parameter of shift right as one 128-bit register. 
 * The 128-bit integer is shifted by (SL2 * 8) bits. 
#define SR2 1 
*/

/** A bitmask, used in the recursion.  These parameters are introduced
 * to break symmetry of SIMD.
#define MSK1 (uint64_t)0xdfffffefULL
#define MSK2 (uint64_t)0xddfecb7fULL
*/

/** These definitions are part of a 128-bit period certification vector.
#define PCV1	UINT64_C(0x00000001)
#define PCV2	UINT64_C(0x00000000)
*/

#define LOW_MASK  UINT64_C(0x000FFFFFFFFFFFFF)
#define LOW_MASK32_1 0x000fffffU
#define LOW_MASK32_2 0xffffffffU
#define HIGH_CONST UINT64_C(0x3FF0000000000000)
#define HIGH_CONST32 0x3ff00000U

/* for sse2 */
#define SSE2_SHUFF 0x4e

#if MEXP == 607
  #include "dSFMT-params607.h"
#elif MEXP == 1279
  #include "dSFMT-params1279.h"
#elif MEXP == 2281
  #include "dSFMT-params2281.h"
#elif MEXP == 4423
  #include "dSFMT-params4423.h"
#elif MEXP == 11213
  #include "dSFMT-params11213.h"
#elif MEXP == 19937
  #include "dSFMT-params19937.h"
#elif MEXP == 44497
  #include "dSFMT-params44497.h"
#elif MEXP == 86243
  #include "dSFMT-params86243.h"
#else
#ifdef __GNUC__
  #error "MEXP is not valid."
  #undef MEXP
#else
  #undef MEXP
#endif

#endif

#endif /* DSFMT_PARAMS_H */
