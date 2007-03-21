#ifndef DSFMT_PARAMS4423_H
#define DSFMT_PARAMS4423_H

#define SFMT_POS1	5
#define SFMT_SL1	31
#define SFMT_SL2	1
#define SFMT_SR1	5
#define SFMT_SR2	16
#define SFMT_MSK1	UINT64_C(0xfbff7fffffffffff)
#define SFMT_MSK2	UINT64_C(0xdabfdb9fffffb77f)
#define SFMT_MSK32_1	0xfbff7fffU
#define SFMT_MSK32_2	0xffffffffU
#define SFMT_MSK32_3	0xdabfdb9fU
#define SFMT_MSK32_4	0xffffb77fU
#define SFMT_PCV1	UINT64_C(0x0000000000000001)
#define SFMT_PCV2	UINT64_C(0x000de72ad8bbb330)
#define SFMT_IDSTR \
"dSFMT-4423:5-31-1-5-16:fbff7fffffffffff-dabfdb9fffffb77f"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 7
#define ALTI_SL1_PERM \
(vector unsigned char)(3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2)
#define ALTI_SL1_MSK \
(vector unsigned int)(0xffffffffU,0x80000000U,0xffffffffU,0x80000000U)
#define ALTI_SL2_PERM \
(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
#define ALTI_SR1 5
#define ALTI_SR1_MSK \
(vector unsigned int)(0x03ff7fffU,0xffffffffU,0x02bfdb9fU,0xffffb77fU)
#define ALTI_SR2_PERM \
(vector unsigned char)(18,18,0,1,2,3,4,5,18,18,8,9,10,11,12,13)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2, SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(SFMT_HIGH_CONST32, 0, SFMT_HIGH_CONST32, 0)

#endif /* DSFMT_PARAMS4423_H */
