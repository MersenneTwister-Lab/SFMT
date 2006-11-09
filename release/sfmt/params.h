#if !defined(MEXP)
  #warning "MEXP is not defined. I assume MEXP is 19937."
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
#define N (MEXP / 128)
/** N32 is the size of internal state array when regarded as an array
 * of 32-bit integers.*/
#define N32 (N * 4)
/** N64 is the size of internal state array when regarded as an array
 * of 64-bit integers.*/
#define N64 (N * 2)
#if 0
/*----------------------
  the parameters of SFMT
  ----------------------*/
/** the pick up position of the array. */
#define POS1 122
/** the parameter of shift left as four 32-bit registers. */
#define SL1 18
/** the parameter of shift left as one 128-bit register. 
 * The 128-bit integer is shifted by (SL2 * 8) bits. 
 */
#define SL2 1
/** the parameter of shift right as four 32-bit registers. */
#define SR1 11
/** the parameter of shift right as one 128-bit register. 
 * The 128-bit integer is shifted by (SL2 * 8) bits. 
 */
#define SR2 1
/** A bitmask, used in the recursion.  These parameters are introduced
 * to break symmetry of SIMD.*/
#define MSK1 0xdfffffefU
/** A bitmask, used in the recursion.  These parameters are introduced
 * to break symmetry of SIMD.*/
#define MSK2 0xddfecb7fU
/** A bitmask, used in the recursion.  These parameters are introduced
 * to break symmetry of SIMD.*/
#define MSK3 0xbffaffffU
/** A bitmask, used in the recursion.  These parameters are introduced
 * to break symmetry of SIMD.*/
#define MSK4 0xbffffff6U
/** The 32 MSBs of the internal state array is seto to this
 * value. This peculiar value assures that the period length of the
 * output sequence is a multiple of 2^19937-1.
 */
#define INIT_LUNG 0x6d736d6dU
#endif
#if MEXP == 607
  #include "params607.h"
#elif MEXP == 2203
  #include "params2203.h"
#elif MEXP == 4253
  #include "params4253.h"
#elif MEXP == 11213
  #include "params11213.h"
#elif MEXP == 19937
  #include "params19937.h"
#elif MEXP == 44497
  #include "params44497.h"
#elif MEXP == 86243
  #include "params86243.h"
#else
  #error "MEXP is not valid."
#endif
