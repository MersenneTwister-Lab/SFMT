#define POS1	776
#define SL1	13
#define SL2	5
#define SR1	2
#define SR2	8
#define MSK1	UINT64_C(0xc75ffffeda7dfcdf)
#define MSK2	UINT64_C(0xda7dbddfefbd7bfd)
#define MSK32_1	0xc75ffffeU
#define MSK32_2	0xda7dfcdfU
#define MSK32_3	0xda7dbddfU
#define MSK32_4	0xefbd7bfdU
#define PCV1	UINT64_C(0x0000000000000001)
#define PCV2	UINT64_C(0x0004c28456900000)
#define IDSTR	"dSFMT-132049:776-13-5-2-8:c75ffffeda7dfcdf-da7dbddfefbd7bfd"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 5
#define SL1_PERM \
(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
#define SL1_MSK \
(vector unsigned int)(0xffffffffU,0xffffe000U,0xffffffffU,0xffffe000U)
#define SL2_PERM \
(vector unsigned char)(5,6,7,27,27,27,27,27,13,14,15,0,1,2,3,4)
#define ALTI_SR1 2
#define SR1_MSK \
(vector unsigned int)(0x075ffffeU,0xda7dfcdfU,0x1a7dbddfU,0xefbd7bfdU)
#define SR2_PERM \
(vector unsigned char)(17,0,1,2,3,4,5,6,17,8,9,10,11,12,13,14)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (LOW_MASK32_1, LOW_MASK32_2, LOW_MASK32_1, LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(HIGH_CONST32, 0, HIGH_CONST32, 0)
