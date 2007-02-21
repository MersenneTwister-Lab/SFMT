/* parameters for sfmtD01 */
#define MEXP 19937
#define WORDSIZE 104
#define N (MEXP / WORDSIZE)
#define MAXDEGREE (WORDSIZE * (N + 1))

#define POS1 102
#define SL1 4
#define SL2 1
#define SR1 1
#define SR2 40
#define MSK1 0xfbfffff3fffff6ffULL
#define MSK2 0xfeffcfbdfefefe6eULL
#define MSK32_1 0xfbfffff3U
#define MSK32_2 0xfffff6ffU
#define MSK32_3 0xfeffcfbdU
#define MSK32_4 0xfefefe6eU

#define LOW_MASK  0x000FFFFFFFFFFFFFULL
//#define HIGH_CONST 0xBFF0000000000000ULL
#define HIGH_CONST 0x3FF0000000000000ULL
//#define HIGH_CONST 0x0000000000000ULL
#define LOW_MASK32_1 0x000fffffU
#define LOW_MASK32_2 0xffffffffU
//#define HIGH_CONST32 0xbff00000U
#define HIGH_CONST32 0x3ff00000U

/* for sse2 */
#define SSE2_SHUFF 0x4e
