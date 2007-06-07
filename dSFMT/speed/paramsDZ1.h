/* parameters for sfmtD01 */
#define MEXP 19937
#define WORDSIZE 104
#define N ((MEXP - 256) / WORDSIZE + 1)
#define MAXDEGREE (WORDSIZE * N + 256)

#define SL1 2
#define SL2 42
#define SR1 1
#define MSK1 0xffbbffb6be7bff8fULL
#define MSK2 0xfcffffee7fffffffULL
#define MSK32_1 0xffbbffb6U
#define MSK32_2 0xbe7bff8fU
#define MSK32_3 0xfcffffeeU
#define MSK32_4 0x7fffffffU

#define LOW_MASK  0x000FFFFFFFFFFFFFULL
#define HIGH_CONST 0x3FF0000000000000ULL
#define LOW_MASK32_1 0x000fffffU
#define LOW_MASK32_2 0xffffffffU
#define HIGH_CONST32 0x3ff00000U

/* for sse2 */
#define SSE2_SHUFF 0x4e
