#ifndef DSFMT_PARAMS19937_H
#define DSFMT_PARAMS19937_H

#define DSFMT_POS1	36
#define DSFMT_SL1	29
#define DSFMT_SL2	1
#define DSFMT_SR1	7
#define DSFMT_SR2	16
#define DSFMT_MSK1	UINT64_C(0x57fbfffdffff575f)
#define DSFMT_MSK2	UINT64_C(0xffff6febffffffee)
#define DSFMT_MSK32_1	0x57fbfffdU
#define DSFMT_MSK32_2	0xffff575fU
#define DSFMT_MSK32_3	0xffff6febU
#define DSFMT_MSK32_4	0xffffffeeU
#define DSFMT_PCV1	UINT64_C(0x0000000000000001)
#define DSFMT_PCV2	UINT64_C(0x000ec8f3d0b00000)
#define DSFMT_IDSTR \
	"dSFMT-19937:36-29-1-7-16:57fbfffdffff575f-ffff6febffffffee"


/* PARAMETERS FOR ALTIVEC */
#if defined(__APPLE__)	/* For OSX */
    #define ALTI_SL1 	(vector unsigned int)(5, 5, 5, 5)
    #define ALTI_SL1_PERM \
	(vector unsigned char)(3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2)
    #define ALTI_SL1_MSK \
	(vector unsigned int)(0xffffffffU,0xe0000000U,0xffffffffU,0xe0000000U)
    #define ALTI_SL2_PERM \
	(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
    #define ALTI_SR1 \
	(vector unsigned int)(DSFMT_SR1, DSFMT_SR1, DSFMT_SR1, DSFMT_SR1)
    #define ALTI_SR1_MSK \
	(vector unsigned int)(0x01fbfffdU,0xffff575fU,0x01ff6febU,0xffffffeeU)
    #define ALTI_SR2_PERM \
	(vector unsigned char)(18,18,0,1,2,3,4,5,18,18,8,9,10,11,12,13)
    #define ALTI_PERM \
	(vector unsigned char)(8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7)
    #define ALTI_LOW_MSK \
	(vector unsigned int)(DSFMT_LOW_MASK32_1, DSFMT_LOW_MASK32_2, \
		DSFMT_LOW_MASK32_1, DSFMT_LOW_MASK32_2)
    #define ALTI_HIGH_CONST \
	(vector unsigned int)(DSFMT_HIGH_CONST32, 0, DSFMT_HIGH_CONST32, 0)
#else	/* For OTHER OSs(Linux?) */
    #define ALTI_SL1 	{5, 5, 5, 5}
    #define ALTI_SL1_PERM \
	{3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2}
    #define ALTI_SL1_MSK \
	{0xffffffffU,0xe0000000U,0xffffffffU,0xe0000000U}
    #define ALTI_SL2_PERM \
	{1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0}
    #define ALTI_SR1 \
	{DSFMT_SR1, DSFMT_SR1, DSFMT_SR1, DSFMT_SR1}
    #define ALTI_SR1_MSK \
	{0x01fbfffdU,0xffff575fU,0x01ff6febU,0xffffffeeU}
    #define ALTI_SR2_PERM \
	{18,18,0,1,2,3,4,5,18,18,8,9,10,11,12,13}
    #define ALTI_PERM \
	{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7}
    #define ALTI_LOW_MSK \
	{DSFMT_LOW_MASK32_1, DSFMT_LOW_MASK32_2, \
		DSFMT_LOW_MASK32_1, DSFMT_LOW_MASK32_2}
    #define ALTI_HIGH_CONST \
	{DSFMT_HIGH_CONST32, 0, DSFMT_HIGH_CONST32, 0}
#endif

#endif /* DSFMT_PARAMS19937_H */
