/* parameters for sfmtD01 */
#define MEXP 19937
#define WORDSIZE 104
#define N (MEXP / WORDSIZE)
#define MAXDEGREE (WORDSIZE * (N + 1))

#define SL1 25
#define SL2 1
#define SR1 41
#define SR2 19
#define MSK1 0xdf6ef8edeff7ffbbULL
#define MSK2 0xfffdbf7fff7bffedULL
#define MSK32_1 0xdf6ef8edU
#define MSK32_2 0xeff7ffbbU
#define MSK32_3 0xfffdbf7fU
#define MSK32_4 0xff7bffedU

#define LOW_MASK  0x000FFFFFFFFFFFFFULL
#define HIGH_CONST 0x3FF0000000000000ULL
#define LOW_MASK32_1 0x000fffffU
#define LOW_MASK32_2 0xffffffffU
#define HIGH_CONST32 0x3ff00000U

/* for sse2 */
#define SSE2_SHUFF 0x4e
