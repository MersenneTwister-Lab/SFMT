#ifndef DSFMT_PARAMS44497_H
#define DSFMT_PARAMS44497_H

#define POS1	244
#define SL1	9
#define SL2	3
#define SR1	7
#define SR2	16
#define MSK1	UINT64_C(0xfefffcfdeffef7ff)
#define MSK2	UINT64_C(0xffffffffd7eff6ff)
#define MSK32_1	0xfefffcfdU
#define MSK32_2	0xeffef7ffU
#define MSK32_3	0xffffffffU
#define MSK32_4	0xd7eff6ffU
#define PCV1	UINT64_C(0x0000000000000001)
#define PCV2	UINT64_C(0x0001930400000000)
#define IDSTR	"dSFMT-44497:244-9-3-7-16:fefffcfdeffef7ff-ffffffffd7eff6ff"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 1
#define SL1_PERM \
(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
#define SL1_MSK \
(vector unsigned int)(0xffffffffU,0xfffffe00U,0xffffffffU,0xfffffe00U)
#define SL2_PERM \
(vector unsigned char)(3,4,5,6,7,29,29,29,11,12,13,14,15,0,1,2)
#define ALTI_SR1 7
#define SR1_MSK \
(vector unsigned int)(0x00fffcfdU,0xeffef7ffU,0x01ffffffU,0xd7eff6ffU)
#define SR2_PERM \
(vector unsigned char)(18,18,0,1,2,3,4,5,18,18,8,9,10,11,12,13)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (LOW_MASK32_1, LOW_MASK32_2, LOW_MASK32_1, LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(HIGH_CONST32, 0, HIGH_CONST32, 0)

#endif /* DSFMT_PARAMS44497_H */
