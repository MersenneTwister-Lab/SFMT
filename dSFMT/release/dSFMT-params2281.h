#define POS1	13
#define SL1	18
#define SL2	5
#define SR1	7
#define SR2	16
#define MSK1	UINT64_C(0xdceffefcf3efbfef)
#define MSK2	UINT64_C(0xe4bfb5beffffbfff)
#define MSK32_1	0xdceffefcU
#define MSK32_2	0xf3efbfefU
#define MSK32_3	0xe4bfb5beU
#define MSK32_4	0xffffbfffU
#define PCV1	UINT64_C(0x0000000000000001)
#define PCV2	UINT64_C(0x0002a00000000000)
#define IDSTR	"dSFMT-2281:13-18-5-7-16:dceffefcf3efbfef-e4bfb5beffffbfff"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 2
#define SL1_PERM \
(vector unsigned char)(2,3,4,5,6,7,30,30,10,11,12,13,14,15,0,1)
#define SL1_MSK \
(vector unsigned int)(0xffffffffU,0xfffc0000U,0xffffffffU,0xfffc0000U)
#define SL2_PERM \
(vector unsigned char)(5,6,7,27,27,27,27,27,13,14,15,0,1,2,3,4)
#define ALTI_SR1 7
#define SR1_MSK \
(vector unsigned int)(0x00effefcU,0xf3efbfefU,0x00bfb5beU,0xffffbfffU)
#define SR2_PERM \
(vector unsigned char)(18,18,0,1,2,3,4,5,18,18,8,9,10,11,12,13)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (LOW_MASK32_1, LOW_MASK32_2, LOW_MASK32_1, LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(HIGH_CONST32, 0, HIGH_CONST32, 0)
