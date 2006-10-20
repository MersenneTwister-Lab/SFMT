/* parameters for sfmtD01 */
#define MEXP 19937
#define WORDSIZE 104
#define N (MEXP / WORDSIZE)
#define MAXDEGREE (WORDSIZE * (N + 1))

#define POS1 138
#define SL1 14
#define SL2 5
#define SR1 46
#define SR2 31
#define MSK1 0x7ffebfd7ff6affffULL
#define MSK2 0xfeffbff7fffffbbfULL
#define MSK32_1 0x7ffebfd7U
#define MSK32_2 0xff6affffU
#define MSK32_3 0xfeffbff7U
#define MSK32_4 0xfffffbbfU

#define LOW_MASK  0x000FFFFFFFFFFFFFULL
#define HIGH_CONST 0xBFF0000000000000ULL
//#define HIGH_CONST 0x0000000000000ULL
#define LOW_MASK32_1 0x000fffffU
#define LOW_MASK32_2 0xffffffffU
#define HIGH_CONST32 0xbff00000U

/* for altivec */
#define ALTI_SL1 6
#define SL1_PERM \
  (vector unsigned char)(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15)
#define SL1_MSK \
    (vector unsigned int)(0xffffffffU, 0xffffc000U, 0xffffffffU, 0xffffc000U)
#define ALTI_SR1 6
#define SR1_PERM \
    (vector unsigned char)(0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
#define SR1_MSK (vector unsigned int)(0, 0x3ffffU, 0, 0x3ffffU)
#define ALTI_SR2 7
#define SR2_PERM \
    (vector unsigned char)(0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12)
#define SR2_MSK (vector unsigned int)(1, 0xffffffffU, 1, 0xffffffffU)
#define SL2_PERM \
  (vector unsigned char)(5, 6, 7, 27, 27, 27, 27, 27, 13, 14, 15, 0, 1, 2, 3, 4)
#define ALTI_PERM \
  (vector unsigned char)(8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_MSK \
    (vector unsigned int)(0x7ffebfd7U, 0xff6affffU, 0xfeffbff7U, 0xfffffbbfU)
#define ALTI_LOW_MSK \
    (vector unsigned int)(0x000fffffU, 0xffffffffU, 0x000fffffU, 0xffffffffU)
#define ALTI_HIGH_CONST (vector unsigned int)(0xbff00000U, 0, 0xbff00000U, 0)

/* for sse2 */
#define SSE2_SHUFF 0x4e
