#ifndef DSFMT2_PARAMS521_H
#define DSFMT2_PARAMS521_H

/* fix.resDPza.521.mac.01.txt */
#define DSFMT2_POS1	1
#define DSFMT2_SL1	19
#define DSFMT2_MSK1	UINT64_C(0x000f77b6dffdefdb)
#define DSFMT2_MSK2	UINT64_C(0x0009afff5ff37ebb)
#define DSFMT2_MSK32_1	0x000f77b6U
#define DSFMT2_MSK32_2	0xdffdefdbU
#define DSFMT2_MSK32_3	0x0009afffU
#define DSFMT2_MSK32_4	0x5ff37ebbU
#define DSFMT2_FIX1	UINT64_C(0xe67921ff2ed2e282)
#define DSFMT2_FIX2	UINT64_C(0xabcbf06aeda196c5)
#define DSFMT2_PCV1	UINT64_C(0x0000000000000001)
#define DSFMT2_PCV2	UINT64_C(0x0000000000000000)
#define DSFMT2_IDSTR \
"dSFMT2-521:1-19:ffcfeef7fdffffff-fdffffb7ffffffff"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 1
#define ALTI_SL1_PERM \
(vector unsigned char)(4,5,6,7,28,28,28,28,12,13,14,15,0,1,2,3)
#define ALTI_SL1_MSK \
(vector unsigned int)(0xfffffffeU,0x00000000U,0xfffffffeU,0x00000000U)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#endif /* DSFMT2_PARAMS521_H */
