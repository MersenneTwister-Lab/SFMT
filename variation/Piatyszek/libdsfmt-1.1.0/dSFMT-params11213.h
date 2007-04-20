#ifndef DSFMT_PARAMS11213_H
#define DSFMT_PARAMS11213_H

#define SFMT_POS1	42
#define SFMT_SL1	37
#define SFMT_SL2	3
#define SFMT_SR1	7
#define SFMT_SR2	16
#define SFMT_MSK1	UINT64_C(0xffdbfdbfdfbb7ffe)
#define SFMT_MSK2	UINT64_C(0xfbf7ff7ffbef3df7)
#define SFMT_MSK32_1	0xffdbfdbfU
#define SFMT_MSK32_2	0xdfbb7ffeU
#define SFMT_MSK32_3	0xfbf7ff7fU
#define SFMT_MSK32_4	0xfbef3df7U
#define SFMT_PCV1	UINT64_C(0x0000000000000001)
#define SFMT_PCV2	UINT64_C(0x00032a9a00000000)
#define SFMT_IDSTR \
"dSFMT-11213:42-37-3-7-16:ffdbfdbfdfbb7ffe-fbf7ff7ffbef3df7"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 5
#define ALTI_SL1_PERM \
(vector unsigned char)(4,5,6,7,28,28,28,28,12,13,14,15,0,1,2,3)
#define ALTI_SL1_MSK \
(vector unsigned int)(0xffffffe0U,0x00000000U,0xffffffe0U,0x00000000U)
#define ALTI_SL2_PERM \
(vector unsigned char)(3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2)
#define ALTI_SR1 7
#define ALTI_SR1_MSK \
(vector unsigned int)(0x01dbfdbfU,0xdfbb7ffeU,0x01f7ff7fU,0xfbef3df7U)
#define ALTI_SR2_PERM \
(vector unsigned char)(18,18,0,1,2,3,4,5,18,18,8,9,10,11,12,13)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2, SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(SFMT_HIGH_CONST32, 0, SFMT_HIGH_CONST32, 0)

#endif /* DSFMT_PARAMS11213_H */
