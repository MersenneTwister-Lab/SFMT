#ifndef DSFMT_PARAMS132049_H
#define DSFMT_PARAMS132049_H

#define SFMT_POS1	695
#define SFMT_SL1	15
#define SFMT_SL2	5
#define SFMT_SR1	4
#define SFMT_SR2	8
#define SFMT_MSK1	UINT64_C(0xdffffffedf53ffff)
#define SFMT_MSK2	UINT64_C(0xffffeffffbd35ffe)
#define SFMT_MSK32_1	0xdffffffeU
#define SFMT_MSK32_2	0xdf53ffffU
#define SFMT_MSK32_3	0xffffefffU
#define SFMT_MSK32_4	0xfbd35ffeU
#define SFMT_PCV1	UINT64_C(0x0000000000000001)
#define SFMT_PCV2	UINT64_C(0x000395e065400000)
#define SFMT_IDSTR \
	"dSFMT-132049:695-15-5-4-8:dffffffedf53ffff-ffffeffffbd35ffe"


/* PARAMETERS FOR ALTIVEC */
#if defined(__APPLE__)	/* For OSX */
    #define ALTI_SL1 	(vector unsigned int)(7, 7, 7, 7)
    #define ALTI_SL1_PERM \
	(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
    #define ALTI_SL1_MSK \
	(vector unsigned int)(0xffffffffU,0xffff8000U,0xffffffffU,0xffff8000U)
    #define ALTI_SL2_PERM \
	(vector unsigned char)(5,6,7,27,27,27,27,27,13,14,15,0,1,2,3,4)
    #define ALTI_SR1 \
	(vector unsigned int)(SFMT_SR1, SFMT_SR1, SFMT_SR1, SFMT_SR1)
    #define ALTI_SR1_MSK \
	(vector unsigned int)(0x0ffffffeU,0xdf53ffffU,0x0fffefffU,0xfbd35ffeU)
    #define ALTI_SR2_PERM \
	(vector unsigned char)(17,0,1,2,3,4,5,6,17,8,9,10,11,12,13,14)
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
	{5,6,7,27,27,27,27,27,13,14,15,0,1,2,3,4}
    #define ALTI_SR1 \
	{SFMT_SR1, SFMT_SR1, SFMT_SR1, SFMT_SR1}
    #define ALTI_SR1_MSK \
	{0x0ffffffeU,0xdf53ffffU,0x0fffefffU,0xfbd35ffeU}
    #define ALTI_SR2_PERM \
	{17,0,1,2,3,4,5,6,17,8,9,10,11,12,13,14}
    #define ALTI_PERM \
	{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7}
    #define ALTI_LOW_MSK \
	{SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2, \
		SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2}
    #define ALTI_HIGH_CONST \
	{SFMT_HIGH_CONST32, 0, SFMT_HIGH_CONST32, 0}
#endif

#endif /* DSFMT_PARAMS132049_H */
