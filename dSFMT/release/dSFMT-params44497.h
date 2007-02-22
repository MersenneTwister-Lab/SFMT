#define POS1	86
#define SL1	15
#define SL2	5
#define SR1	7
#define SR2	16
#define MSK1	UINT64_C(0xffbedbffff7fefff)
#define MSK2	UINT64_C(0xff7effffff3b2fff)
#define MSK32_1	0xffbedbffU
#define MSK32_2	0xff7fefffU
#define MSK32_3	0xff7effffU
#define MSK32_4	0xff3b2fffU
#define PCV1	UINT64_C(0x0000000000000001)
#define PCV2	UINT64_C(0x000e739000000000)
#define IDSTR	"dSFMT-44497:86-15-5-7-16:ffbedbffff7fefff-ff7effffff3b2fff"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 7
#define SL1_PERM \
(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
#define SL1_MSK \
(vector unsigned int)(0xffffffffU,0xffff8000U,0xffffffffU,0xffff8000U)
#define SL2_PERM \
(vector unsigned char)(5,6,7,27,27,27,27,27,13,14,15,0,1,2,3,4)
#define ALTI_SR1 7
#define SR1_MSK \
(vector unsigned int)(0x01bedbffU,0xff7fefffU,0x017effffU,0xff3b2fffU)
#define SR2_PERM \
(vector unsigned char)(18,18,0,1,2,3,4,5,18,18,8,9,10,11,12,13)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (LOW_MASK32_1, LOW_MASK32_2, LOW_MASK32_1, LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(HIGH_CONST32, 0, HIGH_CONST32, 0)
