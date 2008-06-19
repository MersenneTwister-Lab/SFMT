#ifndef DSFMT_PARAMS2281_H
#define DSFMT_PARAMS2281_H

#define DSFMT_POS1	14
#define DSFMT_SL1	31
#define DSFMT_SL2	1
#define DSFMT_SR1	3
#define DSFMT_SR2	8
#define DSFMT_MSK1	UINT64_C(0xfe9fe9ffe7bffdfd)
#define DSFMT_MSK2	UINT64_C(0xf77fdfdff7ffffff)
#define DSFMT_MSK32_1	0xfe9fe9ffU
#define DSFMT_MSK32_2	0xe7bffdfdU
#define DSFMT_MSK32_3	0xf77fdfdfU
#define DSFMT_MSK32_4	0xf7ffffffU
#define DSFMT_PCV1	UINT64_C(0x0000000000000001)
#define DSFMT_PCV2	UINT64_C(0x0000e00000000000)
#define DSFMT_IDSTR \
	"dSFMT-2281:14-31-1-3-8:fe9fe9ffe7bffdfd-f77fdfdff7ffffff"


/* PARAMETERS FOR ALTIVEC */
#if defined(__APPLE__)	/* For OSX */
    #define ALTI_SL1 	(vector unsigned int)(7, 7, 7, 7)
    #define ALTI_SL1_PERM \
	(vector unsigned char)(3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2)
    #define ALTI_SL1_MSK \
	(vector unsigned int)(0xffffffffU,0x80000000U,0xffffffffU,0x80000000U)
    #define ALTI_SL2_PERM \
	(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
    #define ALTI_SR1 \
	(vector unsigned int)(DSFMT_SR1, DSFMT_SR1, DSFMT_SR1, DSFMT_SR1)
    #define ALTI_SR1_MSK \
	(vector unsigned int)(0x1e9fe9ffU,0xe7bffdfdU,0x177fdfdfU,0xf7ffffffU)
    #define ALTI_SR2_PERM \
	(vector unsigned char)(17,0,1,2,3,4,5,6,17,8,9,10,11,12,13,14)
    #define ALTI_PERM \
	(vector unsigned char)(8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7)
    #define ALTI_LOW_MSK \
	(vector unsigned int)(DSFMT_LOW_MASK32_1, DSFMT_LOW_MASK32_2, \
		DSFMT_LOW_MASK32_1, DSFMT_LOW_MASK32_2)
    #define ALTI_HIGH_CONST \
	(vector unsigned int)(DSFMT_HIGH_CONST32, 0, DSFMT_HIGH_CONST32, 0)
#else	/* For OTHER OSs(Linux?) */
    #define ALTI_SL1 	{7, 7, 7, 7}
    #define ALTI_SL1_PERM \
	{3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2}
    #define ALTI_SL1_MSK \
	{0xffffffffU,0x80000000U,0xffffffffU,0x80000000U}
    #define ALTI_SL2_PERM \
	{1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0}
    #define ALTI_SR1 \
	{DSFMT_SR1, DSFMT_SR1, DSFMT_SR1, DSFMT_SR1}
    #define ALTI_SR1_MSK \
	{0x1e9fe9ffU,0xe7bffdfdU,0x177fdfdfU,0xf7ffffffU}
    #define ALTI_SR2_PERM \
	{17,0,1,2,3,4,5,6,17,8,9,10,11,12,13,14}
    #define ALTI_PERM \
	{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7}
    #define ALTI_LOW_MSK \
	{DSFMT_LOW_MASK32_1, DSFMT_LOW_MASK32_2, \
		DSFMT_LOW_MASK32_1, DSFMT_LOW_MASK32_2}
    #define ALTI_HIGH_CONST \
	{DSFMT_HIGH_CONST32, 0, DSFMT_HIGH_CONST32, 0}
#endif

#endif /* DSFMT_PARAMS2281_H */
