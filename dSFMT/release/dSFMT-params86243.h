#define POS1	527
#define SL1	43
#define SL2	1
#define SR1	7
#define SR2	8
#define MSK1	UINT64_C(0xfd7be7fffe9ffbff)
#define MSK2	UINT64_C(0xfffffbdfbfefdfff)
#define MSK32_1	0xfd7be7ffU
#define MSK32_2	0xfe9ffbffU
#define MSK32_3	0xfffffbdfU
#define MSK32_4	0xbfefdfffU
#define PCV1	UINT64_C(0x00008e5738000001)
#define PCV2	UINT64_C(0x00098e953253796a)
#define IDSTR	"dSFMT-86243:527-43-1-7-8:fd7be7fffe9ffbff-fffffbdfbfefdfff"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 3
#define SL1_PERM \
(vector unsigned char)(5,6,7,27,27,27,27,27,13,14,15,0,1,2,3,4)
#define SL1_MSK \
(vector unsigned int)(0xfffff800U,0xfffff800U,0xfffff800U,0xfffff800U)
#define SL2_PERM \
(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
#define ALTI_SR1 7
#define SR1_MSK \
(vector unsigned int)(0x017be7ffU,0xfe9ffbffU,0x01fffbdfU,0xbfefdfffU)
#define SR2_PERM \
(vector unsigned char)(17,0,1,2,3,4,5,6,17,8,9,10,11,12,13,14)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (LOW_MASK32_1, LOW_MASK32_2, LOW_MASK32_1, LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(HIGH_CONST32, 0, HIGH_CONST32, 0)
