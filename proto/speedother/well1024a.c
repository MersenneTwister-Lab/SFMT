/* ************************************************************************** */
/* Copyright:   Francois Panneton and Pierre L'Ecuyer, University of Montreal */
/*              Makoto Matsumoto, Hiroshima University                        */
/* Notice:      This code can be used freely for personal, academic,          */
/*              or non-commercial purposes. For commercial purposes,          */
/*              please contact P. L'Ecuyer at: lecuyer@iro.UMontreal.ca       */
/* ************************************************************************** */
#include <string.h>
#if defined(__GNUC__)
  #include <stdint.h>
#endif
#include <stdio.h>
#include "random-inline.h"
#include <assert.h>

#define W 32
#define R 32
#define M1 3
#define M2 24
#define M3 10

#define MAT0POS(t,v) (v^(v>>t))
#define MAT0NEG(t,v) (v^(v<<(-(t))))
#define Identity(v) (v)

#define V0            STATE[state_i                   ]
#define VM1           STATE[(state_i+M1) & 0x0000001fU]
#define VM2           STATE[(state_i+M2) & 0x0000001fU]
#define VM3           STATE[(state_i+M3) & 0x0000001fU]
#define VRm1          STATE[(state_i+31) & 0x0000001fU]
#define newV0         STATE[(state_i+31) & 0x0000001fU]
#define newV1         STATE[state_i                   ]

#define FACT 2.32830643653869628906e-10

static unsigned int state_i = 0;
static uint32_t STATE[R];
static uint32_t z0, z1, z2;

void InitWELLRNG1024a (unsigned int *init);
void InitWELLRNG1024a (unsigned int *init)
{
   int j;
   state_i = 0;
   for (j = 0; j < R; j++)
     STATE[j] = init[j];
}

INLINE void init_gen_rand(uint32_t seed)
{
    uint32_t init_vec[R + 1];
    int i;

    memset(init_vec, 0, sizeof(init_vec));
    for (i = 0; i <= R; i++) {
	init_vec[i] = seed + i;
    }
    InitWELLRNG1024a(init_vec);
}

INLINE uint32_t gen_rand(void) {
  z0    = VRm1;
  z1    = Identity(V0)       ^ MAT0POS (8, VM1);
  z2    = MAT0NEG (-19, VM2) ^ MAT0NEG(-14,VM3);
  newV1 = z1                 ^ z2; 
  newV0 = MAT0NEG (-11,z0)   ^ MAT0NEG(-7,z1)    ^ MAT0NEG(-13,z2) ;
  state_i = (state_i + 31) & 0x0000001fU;
  return STATE[state_i];
}

INLINE void fill_array(uint32_t array[], int size)
{
    int i;

    for (i = 0; i < size; i++) {
	array[i] = gen_rand();
    }
}

#ifndef FUNCALL
  #ifdef TICK
    #include "test_time_inline.c"
  #else
    #include "test_time2_inline.c"
  #endif
#endif
