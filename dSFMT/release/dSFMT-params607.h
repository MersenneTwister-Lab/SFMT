#ifndef DSFMT_PARAMS607_H
#define DSFMT_PARAMS607_H

#define DSFMT_POS1	2
#define DSFMT_SL1	33
#define DSFMT_SL2	1
#define DSFMT_SR1	7
#define DSFMT_SR2	24
#define DSFMT_MSK1	UINT64_C(0xffcfeef7fdffffff)
#define DSFMT_MSK2	UINT64_C(0xfdffffb7ffffffff)
#define DSFMT_MSK32_1	0xffcfeef7U
#define DSFMT_MSK32_2	0xfdffffffU
#define DSFMT_MSK32_3	0xfdffffb7U
#define DSFMT_MSK32_4	0xffffffffU
#define DSFMT_PCV1	UINT64_C(0x0000000000000001)
#define DSFMT_PCV2	UINT64_C(0x0005196200000000)
#define DSFMT_IDSTR \
	"dSFMT-607:2-33-1-7-24:ffcfeef7fdffffff-fdffffb7ffffffff"


/* PARAMETERS FOR ALTIVEC */
#if defined(__APPLE__)	/* For OSX */
    #define ALTI_SL1 	(vector unsigned int)(1, 1, 1, 1)
    #define ALTI_SL1_PERM \
	(vector unsigned char)(4,5,6,7,28,28,28,28,12,13,14,15,0,1,2,3)
    #define ALTI_SL1_MSK \
	(vector unsigned int)(0xfffffffeU,0x00000000U,0xfffffffeU,0x00000000U)
    #define ALTI_SL2_PERM \
	(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
    #define ALTI_SR1 \
	(vector unsigned int)(DSFMT_SR1, DSFMT_SR1, DSFMT_SR1, DSFMT_SR1)
    #define ALTI_SR1_MSK \
	(vector unsigned int)(0x01cfeef7U,0xfdffffffU,0x01ffffb7U,0xffffffffU)
    #define ALTI_SR2_PERM \
	(vector unsigned char)(19,19,19,0,1,2,3,4,19,19,19,8,9,10,11,12)
    #define ALTI_PERM \
	(vector unsigned char)(8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7)
    #define ALTI_LOW_MSK \
	(vector unsigned int)(DSFMT_LOW_MASK32_1, DSFMT_LOW_MASK32_2, \
		DSFMT_LOW_MASK32_1, DSFMT_LOW_MASK32_2)
    #define ALTI_HIGH_CONST \
	(vector unsigned int)(DSFMT_HIGH_CONST32, 0, DSFMT_HIGH_CONST32, 0)
#else	/* For OTHER OSs(Linux?) */
    #define ALTI_SL1 	{1, 1, 1, 1}
    #define ALTI_SL1_PERM \
	{4,5,6,7,28,28,28,28,12,13,14,15,0,1,2,3}
    #define ALTI_SL1_MSK \
	{0xfffffffeU,0x00000000U,0xfffffffeU,0x00000000U}
    #define ALTI_SL2_PERM \
	{1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0}
    #define ALTI_SR1 \
	{DSFMT_SR1, DSFMT_SR1, DSFMT_SR1, DSFMT_SR1}
    #define ALTI_SR1_MSK \
	{0x01cfeef7U,0xfdffffffU,0x01ffffb7U,0xffffffffU}
    #define ALTI_SR2_PERM \
	{19,19,19,0,1,2,3,4,19,19,19,8,9,10,11,12}
    #define ALTI_PERM \
	{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7}
    #define ALTI_LOW_MSK \
	{DSFMT_LOW_MASK32_1, DSFMT_LOW_MASK32_2, \
		DSFMT_LOW_MASK32_1, DSFMT_LOW_MASK32_2}
    #define ALTI_HIGH_CONST \
	{DSFMT_HIGH_CONST32, 0, DSFMT_HIGH_CONST32, 0}
#endif

#endif /* DSFMT_PARAMS607_H */
