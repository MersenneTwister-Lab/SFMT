#ifndef DSFMT_PARAMS1279_H
#define DSFMT_PARAMS1279_H

#define SFMT_POS1	5
#define SFMT_SL1	33
#define SFMT_SL2	1
#define SFMT_SR1	4
#define SFMT_SR2	8
#define SFMT_MSK1	UINT64_C(0xfbaffbde7bffef7f)
#define SFMT_MSK2	UINT64_C(0xf7ffbcbff5ffffff)
#define SFMT_MSK32_1	0xfbaffbdeU
#define SFMT_MSK32_2	0x7bffef7fU
#define SFMT_MSK32_3	0xf7ffbcbfU
#define SFMT_MSK32_4	0xf5ffffffU
#define SFMT_PCV1	UINT64_C(0x0007f1de08000001)
#define SFMT_PCV2	UINT64_C(0x000d303ee3092a2b)
#define SFMT_IDSTR \
"dSFMT-1279:5-33-1-4-8:fbaffbde7bffef7f-f7ffbcbff5ffffff"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 1
#define ALTI_SL1_PERM \
(vector unsigned char)(4,5,6,7,28,28,28,28,12,13,14,15,0,1,2,3)
#define ALTI_SL1_MSK \
(vector unsigned int)(0xfffffffeU,0x00000000U,0xfffffffeU,0x00000000U)
#define ALTI_SL2_PERM \
(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
#define ALTI_SR1 4
#define ALTI_SR1_MSK \
(vector unsigned int)(0x0baffbdeU,0x7bffef7fU,0x07ffbcbfU,0xf5ffffffU)
#define ALTI_SR2_PERM \
(vector unsigned char)(17,0,1,2,3,4,5,6,17,8,9,10,11,12,13,14)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2, SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(SFMT_HIGH_CONST32, 0, SFMT_HIGH_CONST32, 0)

#endif /* DSFMT_PARAMS1279_H */
