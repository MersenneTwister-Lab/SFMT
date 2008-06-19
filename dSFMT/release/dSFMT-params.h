#ifndef DSFMT_PARAMS_H
#define DSFMT_PARAMS_H

#include "dSFMT.h"

/*----------------------
  the parameters of DSFMT
  following definitions are in dSFMT-paramsXXXX.h file.
  ----------------------*/
/** the pick up position of the array.
#define DSFMT_POS1 122 
*/

/** the parameter of shift left as four 32-bit registers.
#define DSFMT_SL1 18
 */

/** the parameter of shift left as one 128-bit register. 
 * The 128-bit integer is shifted by (SL2 * 8) bits. 
#define DSFMT_SL2 1 
*/

/** the parameter of shift right as four 32-bit registers.
#define DSFMT_SR1 11
*/

/** the parameter of shift right as one 128-bit register. 
 * The 128-bit integer is shifted by (SL2 * 8) bits. 
#define DSFMT_SR2 1 
*/

/** A bitmask, used in the recursion.  These parameters are introduced
 * to break symmetry of SIMD.
#define DSFMT_MSK1 (uint64_t)0xdfffffefULL
#define DSFMT_MSK2 (uint64_t)0xddfecb7fULL
*/

/** These definitions are part of a 128-bit period certification vector.
#define DSFMT_PCV1	UINT64_C(0x00000001)
#define DSFMT_PCV2	UINT64_C(0x00000000)
*/

#define DSFMT_LOW_MASK  UINT64_C(0x000FFFFFFFFFFFFF)
#define DSFMT_LOW_MASK32_1 0x000fffffU
#define DSFMT_LOW_MASK32_2 0xffffffffU
#define DSFMT_HIGH_CONST UINT64_C(0x3FF0000000000000)
#define DSFMT_HIGH_CONST32 0x3ff00000U

/* for sse2 */
#define SSE2_SHUFF 0x4e

#if DSFMT_MEXP == 607
  #include "dSFMT-params607.h"
#elif DSFMT_MEXP == 1279
  #include "dSFMT-params1279.h"
#elif DSFMT_MEXP == 2281
  #include "dSFMT-params2281.h"
#elif DSFMT_MEXP == 4423
  #include "dSFMT-params4423.h"
#elif DSFMT_MEXP == 11213
  #include "dSFMT-params11213.h"
#elif DSFMT_MEXP == 19937
  #include "dSFMT-params19937.h"
#elif DSFMT_MEXP == 44497
  #include "dSFMT-params44497.h"
#elif DSFMT_MEXP == 86243
  #include "dSFMT-params86243.h"
#elif DSFMT_MEXP == 132049
  #include "dSFMT-params132049.h"
#elif DSFMT_MEXP == 216091
  #include "dSFMT-params216091.h"
#else
#ifdef __GNUC__
  #error "DSFMT_MEXP is not valid."
  #undef DSFMT_MEXP
#else
  #undef DSFMT_MEXP
#endif

#endif

#endif /* DSFMT_PARAMS_H */
