/* parameters for sfmtDY3 */
#define MEXP 19937
#define WORDSIZE 104
#define N (MEXP / WORDSIZE)
#define MAXDEGREE (WORDSIZE * (N + 1))

#define SL1 9
#define SL2 3
#define SR1 31
#define SR2 39
#define MSK1 0xcfedffffffffffdeULL
#define MSK2 0xbf9ffb7ffdfffb2fULL
#define MSK32_1 0xcfedffffU
#define MSK32_2 0xffffffdeU
#define MSK32_3 0xbf9ffb7fU
#define MSK32_4 0xfdfffb2fU

#define LOW_MASK  0x000FFFFFFFFFFFFFULL
#define HIGH_CONST 0x3FF0000000000000ULL
#define LOW_MASK32_1 0x000fffffU
#define LOW_MASK32_2 0xffffffffU
#define HIGH_CONST32 0x3ff00000U

/* for sse2 */
#define SSE2_SHUFF 0x4e
