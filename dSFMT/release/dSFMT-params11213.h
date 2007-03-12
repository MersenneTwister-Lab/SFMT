#define POS1	42
#define SL1	37
#define SL2	3
#define SR1	7
#define SR2	16
#define MSK1	UINT64_C(0xffdbfdbfdfbb7ffe)
#define MSK2	UINT64_C(0xfbf7ff7ffbef3df7)
#define MSK32_1	0xffdbfdbfU
#define MSK32_2	0xdfbb7ffeU
#define MSK32_3	0xfbf7ff7fU
#define MSK32_4	0xfbef3df7U
#define PCV1	UINT64_C(0x0000000000000001)
#define PCV2	UINT64_C(0x00032a9a00000000)
#define IDSTR	"dSFMT-11213:42-37-3-7-16:ffdbfdbfdfbb7ffe-fbf7ff7ffbef3df7"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 5
#define SL1_PERM \
(vector unsigned char)(4,5,6,7,28,28,28,28,12,13,14,15,0,1,2,3)
#define SL1_MSK \
(vector unsigned int)(0xffffffe0U,0x00000000U,0xffffffe0U,0x00000000U)
#define SL2_PERM \
(vector unsigned char)(3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2)
#define ALTI_SR1 7
#define SR1_MSK \
(vector unsigned int)(0x01dbfdbfU,0xdfbb7ffeU,0x01f7ff7fU,0xfbef3df7U)
#define SR2_PERM \
(vector unsigned char)(18,18,0,1,2,3,4,5,18,18,8,9,10,11,12,13)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (LOW_MASK32_1, LOW_MASK32_2, LOW_MASK32_1, LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(HIGH_CONST32, 0, HIGH_CONST32, 0)
