#define POS1	23
#define SL1	15
#define SL2	5
#define SR1	3
#define SR2	8
#define MSK1	UINT64_C(0xfd7fbfffeeb7ff7d)
#define MSK2	UINT64_C(0xfcdefffe7efffffd)
#define MSK32_1	0xfd7fbfffU
#define MSK32_2	0xeeb7ff7dU
#define MSK32_3	0xfcdefffeU
#define MSK32_4	0x7efffffdU
#define PCV1	UINT64_C(0x0000000000000001)
#define PCV2	UINT64_C(0x000b006a00000000)
#define IDSTR	"dSFMT-11213:23-15-5-3-8:fd7fbfffeeb7ff7d-fcdefffe7efffffd"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 7
#define SL1_PERM \
(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
#define SL1_MSK \
(vector unsigned int)(0xffffffffU,0xffff8000U,0xffffffffU,0xffff8000U)
#define SL2_PERM \
(vector unsigned char)(5,6,7,27,27,27,27,27,13,14,15,0,1,2,3,4)
#define ALTI_SR1 3
#define SR1_MSK \
(vector unsigned int)(0x1d7fbfffU,0xeeb7ff7dU,0x1cdefffeU,0x7efffffdU)
#define SR2_PERM \
(vector unsigned char)(17,0,1,2,3,4,5,6,17,8,9,10,11,12,13,14)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (LOW_MASK32_1, LOW_MASK32_2, LOW_MASK32_1, LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(HIGH_CONST32, 0, HIGH_CONST32, 0)
