#define POS1	2
#define SL1	33
#define SL2	1
#define SR1	7
#define SR2	24
#define MSK1	UINT64_C(0xffcfeef7fdffffff)
#define MSK2	UINT64_C(0xfdffffb7ffffffff)
#define MSK32_1	0xffcfeef7U
#define MSK32_2	0xfdffffffU
#define MSK32_3	0xfdffffb7U
#define MSK32_4	0xffffffffU
#define PCV1	UINT64_C(0x0000000000000001)
#define PCV2	UINT64_C(0x0005196200000000)
#define IDSTR	"dSFMT-607:2-33-1-7-24:ffcfeef7fdffffff-fdffffb7ffffffff"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 1
#define SL1_PERM \
(vector unsigned char)(4,5,6,7,28,28,28,28,12,13,14,15,0,1,2,3)
#define SL1_MSK \
(vector unsigned int)(0xfffffffeU,0x00000000U,0xfffffffeU,0x00000000U)
#define SL2_PERM \
(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
#define ALTI_SR1 7
#define SR1_MSK \
(vector unsigned int)(0x01cfeef7U,0xfdffffffU,0x01ffffb7U,0xffffffffU)
#define SR2_PERM \
(vector unsigned char)(19,19,19,0,1,2,3,4,19,19,19,8,9,10,11,12)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (LOW_MASK32_1, LOW_MASK32_2, LOW_MASK32_1, LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(HIGH_CONST32, 0, HIGH_CONST32, 0)
