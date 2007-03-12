#define POS1	147
#define SL1	43
#define SL2	3
#define SR1	4
#define SR2	16
#define MSK1	UINT64_C(0xfffffdfaff3ffeff)
#define MSK2	UINT64_C(0xf3efffffffd6fbbe)
#define MSK32_1	0xfffffdfaU
#define MSK32_2	0xff3ffeffU
#define MSK32_3	0xf3efffffU
#define MSK32_4	0xffd6fbbeU
#define PCV1	UINT64_C(0x000b8f71b0000001)
#define PCV2	UINT64_C(0x000c0671c63a820d)
#define IDSTR	"dSFMT-86243:147-43-3-4-16:fffffdfaff3ffeff-f3efffffffd6fbbe"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 3
#define SL1_PERM \
(vector unsigned char)(5,6,7,27,27,27,27,27,13,14,15,0,1,2,3,4)
#define SL1_MSK \
(vector unsigned int)(0xfffff800U,0x00000000U,0xfffff800U,0x00000000U)
#define SL2_PERM \
(vector unsigned char)(3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2)
#define ALTI_SR1 4
#define SR1_MSK \
(vector unsigned int)(0x0ffffdfaU,0xff3ffeffU,0x03efffffU,0xffd6fbbeU)
#define SR2_PERM \
(vector unsigned char)(18,18,0,1,2,3,4,5,18,18,8,9,10,11,12,13)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (LOW_MASK32_1, LOW_MASK32_2, LOW_MASK32_1, LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(HIGH_CONST32, 0, HIGH_CONST32, 0)
