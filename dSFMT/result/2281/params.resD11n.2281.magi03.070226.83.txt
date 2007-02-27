#define POS1	14
#define SL1	31
#define SL2	1
#define SR1	3
#define SR2	8
#define MSK1	UINT64_C(0xfe9fe9ffe7bffdfd)
#define MSK2	UINT64_C(0xf77fdfdff7ffffff)
#define MSK32_1	0xfe9fe9ffU
#define MSK32_2	0xe7bffdfdU
#define MSK32_3	0xf77fdfdfU
#define MSK32_4	0xf7ffffffU
#define PCV1	UINT64_C(0x0000000000000001)
#define PCV2	UINT64_C(0x0000e00000000000)
#define IDSTR	"dSFMT-2281:14-31-1-3-8:fe9fe9ffe7bffdfd-f77fdfdff7ffffff"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 7
#define SL1_PERM \
(vector unsigned char)(3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2)
#define SL1_MSK \
(vector unsigned int)(0xffffffffU,0x80000000U,0xffffffffU,0x80000000U)
#define SL2_PERM \
(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
#define ALTI_SR1 3
#define SR1_MSK \
(vector unsigned int)(0x1e9fe9ffU,0xe7bffdfdU,0x177fdfdfU,0xf7ffffffU)
#define SR2_PERM \
(vector unsigned char)(17,0,1,2,3,4,5,6,17,8,9,10,11,12,13,14)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (LOW_MASK32_1, LOW_MASK32_2, LOW_MASK32_1, LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(HIGH_CONST32, 0, HIGH_CONST32, 0)
