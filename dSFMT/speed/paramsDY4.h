/* parameters for sfmtDY4 */
#define MEXP 19937
#define WORDSIZE 104
#define N (MEXP / WORDSIZE)
#define MAXDEGREE (WORDSIZE * (N + 1))

#define SL1 30
#define SL2 1
#define SR1 7
#define MSK1 0xffff5e1b7ffffaffULL
#define MSK2 0x3eeeff7bffdbbff7ULL
#define MSK32_1 0xffff5e1bU
#define MSK32_2 0x7ffffaffU
#define MSK32_3 0x3eeeff7bU
#define MSK32_4 0xffdbbff7U

#define LOW_MASK  0x000FFFFFFFFFFFFFULL
#define HIGH_CONST 0x3FF0000000000000ULL
#define LOW_MASK32_1 0x000fffffU
#define LOW_MASK32_2 0xffffffffU
#define HIGH_CONST32 0x3ff00000U

/* for sse2 */
#define SSE2_SHUFF 0x4e
