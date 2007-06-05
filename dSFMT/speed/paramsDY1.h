/* parameters for sfmtD01 */
#define MEXP 19937
#define WORDSIZE 104
#define N (MEXP / WORDSIZE)
#define MAXDEGREE (WORDSIZE * (N + 1))

#define SL1 32
#define SL2 29
#define SR1 22
#define SR2 23
#define MSK1 0xfff7ff5fdfffbeffULL
#define MSK2 0xfdfff3bfeffdbdefULL
#define MSK32_1 0xfff7ff5fU
#define MSK32_2 0xdfffbeffU
#define MSK32_3 0xfdfff3bfU
#define MSK32_4 0xeffdbdefU

#define LOW_MASK  0x000FFFFFFFFFFFFFULL
#define HIGH_CONST 0x3FF0000000000000ULL
#define LOW_MASK32_1 0x000fffffU
#define LOW_MASK32_2 0xffffffffU
#define HIGH_CONST32 0x3ff00000U

/* for sse2 */
#define SSE2_SHUFF 0x4e
