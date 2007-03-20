#ifndef DSFMT_PARAMS4423_H
#define DSFMT_PARAMS4423_H

#define POS1	5
#define SL1	31
#define SL2	1
#define SR1	5
#define SR2	16
#define MSK1	UINT64_C(0xfbff7fffffffffff)
#define MSK2	UINT64_C(0xdabfdb9fffffb77f)
#define MSK32_1	0xfbff7fffU
#define MSK32_2	0xffffffffU
#define MSK32_3	0xdabfdb9fU
#define MSK32_4	0xffffb77fU
#define PCV1	UINT64_C(0x0000000000000001)
#define PCV2	UINT64_C(0x000de72ad8bbb330)
#define IDSTR	"dSFMT-4423:5-31-1-5-16:fbff7fffffffffff-dabfdb9fffffb77f"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 7
#define SL1_PERM \
(vector unsigned char)(3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2)
#define SL1_MSK \
(vector unsigned int)(0xffffffffU,0x80000000U,0xffffffffU,0x80000000U)
#define SL2_PERM \
(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
#define ALTI_SR1 5
#define SR1_MSK \
(vector unsigned int)(0x03ff7fffU,0xffffffffU,0x02bfdb9fU,0xffffb77fU)
#define SR2_PERM \
(vector unsigned char)(18,18,0,1,2,3,4,5,18,18,8,9,10,11,12,13)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (LOW_MASK32_1, LOW_MASK32_2, LOW_MASK32_1, LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(HIGH_CONST32, 0, HIGH_CONST32, 0)

#endif /* DSFMT_PARAMS4423_H */
