/* ************************************************************************* */
/* Copyright:  Francois Panneton and Pierre L'Ecuyer, University of Montreal */
/*             Makoto Matsumoto, Hiroshima University                        */
/* Notice:     This code can be used freely for personal, academic,          */
/*             or non-commercial purposes. For commercial purposes,          */
/*             please contact P. L'Ecuyer at: lecuyer@iro.UMontreal.ca       */
/* ************************************************************************* */

#define W 32
#define R 624
#define P 31
#define MASKU (0xffffffffU>>(W-P))
#define MASKL (~MASKU)
#define M1 70
#define M2 179
#define M3 449

#define MAT0POS(t,v) (v^(v>>t))
#define MAT0NEG(t,v) (v^(v<<(-(t))))
#define MAT1(v) v
#define MAT3POS(t,v) (v>>t)

/* To obtain the WELL19937c, uncomment the following line */
/* #define TEMPERING                                      */
#define TEMPERB 0xe46e1700U
#define TEMPERC 0x9b868000U

#define V0            STATE[state_i]
#define VM1Over       STATE[state_i+M1-R]
#define VM1           STATE[state_i+M1]
#define VM2Over       STATE[state_i+M2-R]
#define VM2           STATE[state_i+M2]
#define VM3Over       STATE[state_i+M3-R]
#define VM3           STATE[state_i+M3]
#define VRm1          STATE[state_i-1]
#define VRm1Under     STATE[state_i+R-1]
#define VRm2          STATE[state_i-2]
#define VRm2Under     STATE[state_i+R-2]

#define newV0         STATE[state_i-1]
#define newV0Under    STATE[state_i-1+R]
#define newV1         STATE[state_i]
#define newVRm1       STATE[state_i-2]
#define newVRm1Under  STATE[state_i-2+R]

#define FACT 2.32830643653869628906e-10

static int state_i = 0;
static unsigned int STATE[R];
static unsigned int z0, z1, z2;
static double case_1 (void);
static double case_2 (void);
static double case_3 (void);
static double case_4 (void);
static double case_5 (void);
static double case_6 (void);
       double (*WELLRNG19937a) (void);
#define generate WELLRNG19937a
static unsigned int y;

void InitWELLRNG19937a (unsigned int *init){
   int j;
   state_i = 0;
   WELLRNG19937a = case_1;
   for (j = 0; j < R; j++)
     STATE[j] = init[j];
}

void init (unsigned int seed) {
   state_i = 0;
   WELLRNG19937a = case_1;
   STATE[0] = seed;
}

double case_1 (void){
   // state_i == 0
   z0 = (VRm1Under & MASKL) | (VRm2Under & MASKU);
   z1 = MAT0NEG (-25, V0) ^ MAT0POS (27, VM1);
   z2 = MAT3POS (9, VM2) ^ MAT0POS (1, VM3);
   newV1      = z1 ^ z2;
   newV0Under = MAT1 (z0) ^ MAT0NEG (-9, z1) ^ MAT0NEG (-21, z2) ^ MAT0POS (21, newV1);
   state_i = R - 1;
   WELLRNG19937a = case_3;
#ifdef TEMPERING
   y = STATE[state_i] ^ ((STATE[state_i] << 7) & TEMPERB);
   y =              y ^ ((             y << 15) & TEMPERC);
   return ((double) y * FACT);
#else
   return ((double) STATE[state_i] * FACT);
#endif
}

static double case_2 (void){
   // state_i == 1
   z0 = (VRm1 & MASKL) | (VRm2Under & MASKU);
   z1 = MAT0NEG (-25, V0) ^ MAT0POS (27, VM1);
   z2 = MAT3POS (9, VM2) ^ MAT0POS (1, VM3);
   newV1 = z1 ^ z2;
   newV0 = MAT1 (z0) ^ MAT0NEG (-9, z1) ^ MAT0NEG (-21, z2) ^ MAT0POS (21, newV1);
   state_i = 0;
   WELLRNG19937a = case_1;
#ifdef TEMPERING
   y = STATE[state_i] ^ ((STATE[state_i] << 7) & TEMPERB);
   y =              y ^ ((             y << 15) & TEMPERC);
   return ((double) y * FACT);
#else
   return ((double) STATE[state_i] * FACT);
#endif
}

static double case_3 (void){
   // state_i+M1 >= R
   z0 = (VRm1 & MASKL) | (VRm2 & MASKU);
   z1 = MAT0NEG (-25, V0) ^ MAT0POS (27, VM1Over);
   z2 = MAT3POS (9, VM2Over) ^ MAT0POS (1, VM3Over);
   newV1 = z1 ^ z2;
   newV0 = MAT1 (z0) ^ MAT0NEG (-9, z1) ^ MAT0NEG (-21, z2) ^ MAT0POS (21, newV1);
   state_i--;
   if (state_i + M1 < R)
      WELLRNG19937a = case_5;
#ifdef TEMPERING
   y = STATE[state_i] ^ ((STATE[state_i] << 7) & TEMPERB);
   y =              y ^ ((             y << 15) & TEMPERC);
   return ((double) y * FACT);
#else
   return ((double) STATE[state_i] * FACT);
#endif
}

static double case_4 (void){
   // state_i+M3 >= R
   z0 = (VRm1 & MASKL) | (VRm2 & MASKU);
   z1 = MAT0NEG (-25, V0) ^ MAT0POS (27, VM1);
   z2 = MAT3POS (9, VM2) ^ MAT0POS (1, VM3Over);
   newV1 = z1 ^ z2;
   newV0 = MAT1 (z0) ^ MAT0NEG (-9, z1) ^ MAT0NEG (-21, z2) ^ MAT0POS (21, newV1);
   state_i--;
   if (state_i + M3 < R)
      WELLRNG19937a = case_6;
#ifdef TEMPERING
   y = STATE[state_i] ^ ((STATE[state_i] << 7) & TEMPERB);
   y =              y ^ ((             y << 15) & TEMPERC);
   return ((double) y * FACT);
#else
   return ((double) STATE[state_i] * FACT);
#endif
}

static double case_5 (void){
   // state_i+M2 >= R
   z0 = (VRm1 & MASKL) | (VRm2 & MASKU);
   z1 = MAT0NEG (-25, V0) ^ MAT0POS (27, VM1);
   z2 = MAT3POS (9, VM2Over) ^ MAT0POS (1, VM3Over);
   newV1 = z1 ^ z2;
   newV0 = MAT1 (z0) ^ MAT0NEG (-9, z1) ^ MAT0NEG (-21, z2) ^ MAT0POS (21, newV1);
   state_i--;
   if (state_i + M2 < R)
      WELLRNG19937a = case_4;
#ifdef TEMPERING
   y = STATE[state_i] ^ ((STATE[state_i] << 7) & TEMPERB);
   y =              y ^ ((             y << 15) & TEMPERC);
   return ((double) y * FACT);
#else
   return ((double) STATE[state_i] * FACT);
#endif
}

static double case_6 (void){
   // 2 <= state_i <= (R - M3 - 1)
   z0 = (VRm1 & MASKL) | (VRm2 & MASKU);
   z1 = MAT0NEG (-25, V0) ^ MAT0POS (27, VM1);
   z2 = MAT3POS (9, VM2) ^ MAT0POS (1, VM3);
   newV1 = z1 ^ z2;
   newV0 = MAT1 (z0) ^ MAT0NEG (-9, z1) ^ MAT0NEG (-21, z2) ^ MAT0POS (21, newV1);
   state_i--;
   if (state_i == 1)
      WELLRNG19937a = case_2;
#ifdef TEMPERING
   y = STATE[state_i] ^ ((STATE[state_i] << 7) & TEMPERB);
   y =              y ^ ((             y << 15) & TEMPERC);
   return ((double) y * FACT);
#else
   return ((double) STATE[state_i] * FACT);
#endif
}

#include "test_time2.c"
