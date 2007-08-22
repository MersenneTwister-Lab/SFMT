#ifndef DSFMT_PARAMS216091_H
#define DSFMT_PARAMS216091_H

#define SFMT_POS1	1009
#define SFMT_SL1	15
#define SFMT_SL2	3
#define SFMT_SR1	1
#define SFMT_SR2	16
#define SFMT_MSK1	UINT64_C(0xffffffffffebffbe)
#define SFMT_MSK2	UINT64_C(0xdfbbf7fb9ffdffab)
#define SFMT_MSK32_1	0xffffffffU
#define SFMT_MSK32_2	0xffebffbeU
#define SFMT_MSK32_3	0xdfbbf7fbU
#define SFMT_MSK32_4	0x9ffdffabU
#define SFMT_PCV1	UINT64_C(0x0000000000000001)
#define SFMT_PCV2	UINT64_C(0x000cadf580000000)
#define SFMT_IDSTR \
	"dSFMT-216091:1009-15-3-1-16:ffffffffffebffbe-dfbbf7fb9ffdffab"


/* PARAMETERS FOR ALTIVEC */
#if defined(__APPLE__)	/* For OSX */
    #define ALTI_SL1 	(vector unsigned int)(7, 7, 7, 7)
    #define ALTI_SL1_PERM \
	(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
    #define ALTI_SL1_MSK \
	(vector unsigned int)(0xffffffffU,0xffff8000U,0xffffffffU,0xffff8000U)
    #define ALTI_SL2_PERM \
	(vector unsigned char)(3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2)
    #define ALTI_SR1 \
	(vector unsigned int)(SFMT_SR1, SFMT_SR1, SFMT_SR1, SFMT_SR1)
    #define ALTI_SR1_MSK \
	(vector unsigned int)(0x7fffffffU,0xffebffbeU,0x5fbbf7fbU,0x9ffdffabU)
    #define ALTI_SR2_PERM \
	(vector unsigned char)(18,18,0,1,2,3,4,5,18,18,8,9,10,11,12,13)
    #define ALTI_PERM \
	(vector unsigned char)(8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7)
    #define ALTI_LOW_MSK \
	(vector unsigned int)(SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2, \
		SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2)
    #define ALTI_HIGH_CONST \
	(vector unsigned int)(SFMT_HIGH_CONST32, 0, SFMT_HIGH_CONST32, 0)
#else	/* For OTHER OSs(Linux?) */
    #define ALTI_SL1 	{7, 7, 7, 7}
    #define ALTI_SL1_PERM \
	{1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0}
    #define ALTI_SL1_MSK \
	{0xffffffffU,0xffff8000U,0xffffffffU,0xffff8000U}
    #define ALTI_SL2_PERM \
	{3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2}
    #define ALTI_SR1 \
	{SFMT_SR1, SFMT_SR1, SFMT_SR1, SFMT_SR1}
    #define ALTI_SR1_MSK \
	{0x7fffffffU,0xffebffbeU,0x5fbbf7fbU,0x9ffdffabU}
    #define ALTI_SR2_PERM \
	{18,18,0,1,2,3,4,5,18,18,8,9,10,11,12,13}
    #define ALTI_PERM \
	{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7}
    #define ALTI_LOW_MSK \
	{SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2, \
		SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2}
    #define ALTI_HIGH_CONST \
	{SFMT_HIGH_CONST32, 0, SFMT_HIGH_CONST32, 0}
#endif

#endif /* DSFMT_PARAMS216091_H */
