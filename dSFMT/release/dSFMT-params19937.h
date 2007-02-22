#define POS1	63
#define SL1	17
#define SL2	5
#define SR1	3
#define SR2	16
#define MSK1	UINT64_C(0xf993f7f3cfff7fff)
#define MSK2	UINT64_C(0xfbff37fbfdb5ffff)
#define MSK32_1	0xf993f7f3U
#define MSK32_2	0xcfff7fffU
#define MSK32_3	0xfbff37fbU
#define MSK32_4	0xfdb5ffffU
#define PCV1	UINT64_C(0x0000000000000001)
#define PCV2	UINT64_C(0x000eadddf9c80000)
#define IDSTR	"dSFMT-19937:63-17-5-3-16:f993f7f3cfff7fff-fbff37fbfdb5ffff"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 1
#define SL1_PERM \
(vector unsigned char)(2,3,4,5,6,7,30,30,10,11,12,13,14,15,0,1)
#define SL1_MSK \
(vector unsigned int)(0xffffffffU,0xfffe0000U,0xffffffffU,0xfffe0000U)
#define SL2_PERM \
(vector unsigned char)(5,6,7,27,27,27,27,27,13,14,15,0,1,2,3,4)
#define ALTI_SR1 3
#define SR1_MSK \
(vector unsigned int)(0x1993f7f3U,0xcfff7fffU,0x1bff37fbU,0xfdb5ffffU)
#define SR2_PERM \
(vector unsigned char)(18,18,0,1,2,3,4,5,18,18,8,9,10,11,12,13)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (LOW_MASK32_1, LOW_MASK32_2, LOW_MASK32_1, LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(HIGH_CONST32, 0, HIGH_CONST32, 0)
