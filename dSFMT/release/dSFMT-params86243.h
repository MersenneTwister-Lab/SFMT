#ifndef DSFMT_PARAMS86243_H
#define DSFMT_PARAMS86243_H

#define SFMT_POS1	147
#define SFMT_SL1	43
#define SFMT_SL2	3
#define SFMT_SR1	4
#define SFMT_SR2	16
#define SFMT_MSK1	UINT64_C(0xfffffdfaff3ffeff)
#define SFMT_MSK2	UINT64_C(0xf3efffffffd6fbbe)
#define SFMT_MSK32_1	0xfffffdfaU
#define SFMT_MSK32_2	0xff3ffeffU
#define SFMT_MSK32_3	0xf3efffffU
#define SFMT_MSK32_4	0xffd6fbbeU
#define SFMT_PCV1	UINT64_C(0x000b8f71b0000001)
#define SFMT_PCV2	UINT64_C(0x000c0671c63a820d)
#define SFMT_IDSTR \
	"dSFMT-86243:147-43-3-4-16:fffffdfaff3ffeff-f3efffffffd6fbbe"


/* PARAMETERS FOR ALTIVEC */
#if defined(__APPLE__)	/* For OSX */
    #define ALTI_SL1 	(vector unsigned int)(3, 3, 3, 3)
    #define ALTI_SL1_PERM \
	(vector unsigned char)(5,6,7,27,27,27,27,27,13,14,15,0,1,2,3,4)
    #define ALTI_SL1_MSK \
	(vector unsigned int)(0xfffff800U,0x00000000U,0xfffff800U,0x00000000U)
    #define ALTI_SL2_PERM \
	(vector unsigned char)(3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2)
    #define ALTI_SR1 \
	(vector unsigned int)(SFMT_SR1, SFMT_SR1, SFMT_SR1, SFMT_SR1)
    #define ALTI_SR1_MSK \
	(vector unsigned int)(0x0ffffdfaU,0xff3ffeffU,0x03efffffU,0xffd6fbbeU)
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
    #define ALTI_SL1 	{3, 3, 3, 3}
    #define ALTI_SL1_PERM \
	{5,6,7,27,27,27,27,27,13,14,15,0,1,2,3,4}
    #define ALTI_SL1_MSK \
	{0xfffff800U,0x00000000U,0xfffff800U,0x00000000U}
    #define ALTI_SL2_PERM \
	{3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2}
    #define ALTI_SR1 \
	{SFMT_SR1, SFMT_SR1, SFMT_SR1, SFMT_SR1}
    #define ALTI_SR1_MSK \
	{0x0ffffdfaU,0xff3ffeffU,0x03efffffU,0xffd6fbbeU}
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

#endif /* DSFMT_PARAMS86243_H */
