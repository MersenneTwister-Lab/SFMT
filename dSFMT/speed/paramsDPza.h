#define SFMT_LOW_MASK  0x000FFFFFFFFFFFFFULL
//#define HIGH_CONST 0xBFF0000000000000ULL
#define SFMT_HIGH_CONST 0x3FF0000000000000ULL
//#define HIGH_CONST 0x0000000000000ULL
#define SFMT_LOW_MASK32_1 0x000fffffU
#define SFMT_LOW_MASK32_2 0xffffffffU
//#define HIGH_CONST32 0xbff00000U
#define SFMT_HIGH_CONST32 0x3ff00000U

/* for sse2 */
#define SSE2_SHUFF 0x1b

#define SFMT_POS1       125
#define SFMT_SL1	21
#define SFMT_SR1	12
#define SFMT_MSK1	UINT64_C(0x000fdfafefeeffff)
#define SFMT_MSK2	UINT64_C(0x000ffdfbbbdbffff)
#define SFMT_MSK32_1	0x000fdfafU
#define SFMT_MSK32_2	0xefeeffffU
#define SFMT_MSK32_3	0x000ffdfbU
#define SFMT_MSK32_4	0xbbdbffffU
#define SFMT_PCV1	UINT64_C(0x0000000000000001)
#define SFMT_PCV2	UINT64_C(0x113640cf17e4e100)
#define SFMT_IDSTR \
	"dSFMT2-19937:26-21:abffedffef5fffff-feff6fffdddfbfff:0x0000000000000001-0x113640cf17e4e100"


/* PARAMETERS FOR ALTIVEC */
#if defined(__APPLE__)	/* For OSX */
    #define ALTI_SL1 (vector unsigned char)(5)
    #define ALTI_SL1_PERM \
        (vector unsigned char)(2,3,4,5,6,7,30,30,10,11,12,13,14,15,0,1)
    #define ALTI_SL1_MSK \
        (vector unsigned int)(0xffffffffU,0xffe00000U,0xffffffffU,0xffe00000U)
    #define ALTI_SR1 (vector unsigned char)(4)
    #define ALTI_SR1_PERM \
        (vector unsigned char)(15,0,1,2,3,4,5,6,15,8,9,10,11,12,13,14)
    #define ALTI_SR1_MSK \
        (vector unsigned int)(0x000fffffU,0xffffffffU,0x000fffffU,0xffffffffU)
    #define ALTI_MSK1 \
        (vector unsigned int)(SFMT_MSK32_1, SFMT_MSK32_2, \
                              SFMT_MSK32_3, SFMT_MSK32_4)
    #define ALTI_PERM \
        (vector unsigned char)(12,13,14,15,8,9,10,11,4,5,6,7,0,1,2,3)
    #define ALTI_LOW_MSK \
	(vector unsigned int)(SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2, \
		SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2)
    #define ALTI_HIGH_CONST \
	(vector unsigned int)(SFMT_HIGH_CONST32, 0, SFMT_HIGH_CONST32, 0)
#else	/* For OTHER OSs(Linux?) */
    #define ALTI_SL1 	{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}
    #define ALTI_SL1_PERM \
	{3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2}
    #define ALTI_SL1_MSK \
	{0xffffffffU,0xfc000000U,0xffffffffU,0xfc000000U}
    #define ALTI_SL2 	{21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21}
    #define ALTI_SL2_PERM \
	{2,3,4,5,6,7,30,30,10,11,12,13,14,15,0,1}
    #define ALTI_SL2_MSK \
	{0xabffedffU,0xef400000U,0xfeff6fffU,0xddc00000U}
    #define ALTI_SR \
	{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4}
    #define ALTI_SR_PERM \
	{15,0,1,2,3,4,5,6,15,8,9,10,11,12,13,14}
    #define ALTI_SR_MSK \
	{0x000fffffU,0xffffffffU,0x000fffffU,0xffffffffU}
    #define ALTI_PERM \
	{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7}
    #define ALTI_LOW_MSK \
	{SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2, \
		SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2}
    #define ALTI_HIGH_CONST \
	{SFMT_HIGH_CONST32, 0, SFMT_HIGH_CONST32, 0}
#endif	/* For OSX */
