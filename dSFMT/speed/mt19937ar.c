/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          
   Copyright (C) 2005, Mutsuo Saito,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#include <stdio.h>
#include "mt19937ar.h"

/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

#define M_RAN_INVM32 2.32830643653869628906e-010

#if defined(__GNUC__)
#define INLINE inline
#else
#define INLINE
#endif

#if defined(SSE2)
static __m128i dmt[1];
#elif defined(ALTIVEC)
static vector unsigned int dmt[1];
#endif
static unsigned int mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */
#if defined(__GNUC__) && !defined(DEBUG) 
#define ALWAYSINLINE __attribute__((always_inline)) 
#else
#define ALWAYSINLINE
#endif
#if defined(SSE2)
INLINE static __m128i mm_recursion(uint32_t *x, uint32_t *y, uint32_t *z,
				   __m128i u_mask, __m128i l_mask, 
				   __m128i mat_a, __m128i one) 
    ALWAYSINLINE;
#elif defined(ALTIVEC)
INLINE static vector unsigned int vec_recursion(vector unsigned int a,
						vector unsigned int b,
						vector unsigned int one,
						vector unsigned int zero,
						vector unsigned int mat_a) 
    ALWAYSINLINE;
#endif
INLINE static void gen_rand_all(void) ALWAYSINLINE;

/* initializes mt[N] with a seed */
void init_genrand(unsigned int s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<N; mti++) {
        mt[mti] = 
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void init_by_array(unsigned int init_key[], int key_length)
{
    int i, j, k;
    init_genrand(19650218UL);
    i=1; j=0;
    k = (N>key_length ? N : key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=N-1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

#if defined(SSE2)
INLINE static __m128i mm_recursion(uint32_t *x, uint32_t *y, uint32_t *z,
				   __m128i u_mask, __m128i l_mask, 
				   __m128i mat_a, __m128i one) {
    __m128i a0, a1, b, r;
    a0 = _mm_load_si128((__m128i *)x);
    a1 = _mm_loadu_si128((__m128i *)y);
    b = _mm_loadu_si128((__m128i *)z);
    a0 = _mm_and_si128(a0, u_mask);
    a1 = _mm_and_si128(a1, l_mask);
    r = _mm_or_si128(a0, a1);
    a1 = _mm_and_si128(a1, one);
    a1 = _mm_cmpeq_epi32(a1, one);
    a1 = _mm_and_si128(a1, mat_a);
    a0 = _mm_srli_epi32(r, 1);
    r = _mm_xor_si128(b, a0);
    r = _mm_xor_si128(r, a1);
    return r;
}

INLINE static void gen_rand_all(void)
{
    //uint32_t y;
    __m128i r, u_mask, l_mask, mat_a, one;
    //static uint32_t mag01[2] = { 0x0UL, MATRIX_A };
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    int i;
    u_mask = _mm_set_epi32(UPPER_MASK, UPPER_MASK, UPPER_MASK, UPPER_MASK);
    l_mask = _mm_set_epi32(LOWER_MASK, LOWER_MASK, LOWER_MASK, LOWER_MASK);
    mat_a = _mm_set_epi32(MATRIX_A, MATRIX_A, MATRIX_A, MATRIX_A);
    one = _mm_set_epi32(1, 1, 1, 1);

    for (i = 0; i < N - M - 4; i += 4) {
	r = mm_recursion(&mt[i], &mt[i + 1], &mt[i + M], u_mask, l_mask,
			 mat_a, one);
	_mm_store_si128((__m128i *)&mt[i], r);
    }
    mt[N] = mt[0];
    r = mm_recursion(&mt[i], &mt[i + 1], &mt[i + M], u_mask, l_mask,
		     mat_a, one);
    _mm_store_si128((__m128i *)&mt[i], r);
    i += 4;
    for (; i < N; i += 4) {
	r = mm_recursion(&mt[i], &mt[i + 1], &mt[i + M - N], u_mask, l_mask,
			 mat_a, one);
	_mm_store_si128((__m128i *)&mt[i], r);
    }
    mti = 0;

}
#elif defined(ALTIVEC)
INLINE vector unsigned int vec_recursion(vector unsigned int a,
				      vector unsigned int b,
				      vector unsigned int one,
				      vector unsigned int zero,
				      vector unsigned int mat_a) {
    vector unsigned int r, m;
    
    m = vec_and(a, one);
    r = vec_sr(a, one);
    m = vec_cmpeq(m, zero);
    r = vec_xor(b, r);
    m = vec_sel(mat_a, zero, m);
    r = vec_xor(r, m);
		
    return r;
}

INLINE void gen_rand_all(void) {
    //uint32_t y;
    vector unsigned int a0, a1, a, b0, b1, b, r;
    vector unsigned int u_mask = (vector unsigned int)(UPPER_MASK);
    vector unsigned int l_mask = (vector unsigned int)(LOWER_MASK);
    vector unsigned int one = (vector unsigned int)(1);
    vector unsigned int zero = (vector unsigned int)(0);
    vector unsigned char perm1 = (vector unsigned char)
	(4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19);
    vector unsigned int mat_a = (vector unsigned int)(MATRIX_A);
    //static uint32_t mag01[2] = { 0x0UL, MATRIX_A };
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    int i;

    a0 = vec_ld(0, &mt[0]);
    b0 = vec_ld(0, &mt[M]);
    for (i = 0; i < N - M - 4; i += 4) {
	a1 = vec_ld(0, &mt[i + 4]);
	a = vec_perm(a0, a1, perm1);
	b1 = vec_ld(0, &mt[i + M + 4]);
	b = vec_perm(b0, b1, perm1);
	r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
	r = vec_recursion(r, b, one, zero, mat_a);
	vec_st(r, 0, &mt[i]);
	a0 = a1;
	b0 = b1;
    }
    a1 = vec_ld(0, &mt[i + 4]);
    a = vec_perm(a0, a1, perm1);
    b1 = vec_ld(0, &mt[0]);
    b = vec_perm(b0, b1, perm1);
    r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
    r = vec_recursion(r, b, one, zero, mat_a);
    vec_st(r, 0, &mt[i]);
    a0 = a1;
    b0 = b1;
    i += 4;
    for (; i < N - 4; i += 4) {
	a1 = vec_ld(0, &mt[i + 4]);
	a = vec_perm(a0, a1, perm1);
	b1 = vec_ld(0, &mt[i + M - N + 4]);
	b = vec_perm(b0, b1, perm1);
	r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
	r = vec_recursion(r, b, one, zero, mat_a);
	vec_st(r, 0, &mt[i]);
	a0 = a1;
	b0 = b1;
    }
    a1 = vec_ld(0, &mt[0]);
    a = vec_perm(a0, a1, perm1);
    b1 = vec_ld(0, &mt[i + M - N + 4]);
    b = vec_perm(b0, b1, perm1);
    r = vec_or(vec_and(a0, u_mask), vec_and(a, l_mask));
    r = vec_recursion(r, b, one, zero, mat_a);
    vec_st(r, 0, &mt[i]);

    mti = 0;
}
#else
INLINE void gen_rand_all(void) {
    static unsigned int mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */
    unsigned int y;
    int kk;

    for (kk=0;kk<N-M;kk++) {
	y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
	mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    for (;kk<N-1;kk++) {
	y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
	mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
    mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

    mti = 0;
}
#endif
/* generates a random number on [0,0xffffffff]-interval */
INLINE unsigned int genrand_int32(void)
{
    unsigned int y;

    if (mti >= N) { /* generate N words at one time */
	gen_rand_all();
    }
  
    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
int genrand_int31(void)
{
    return (int)(genrand_int32()>>1);
}

/* generates a random number on [0,1]-real-interval */
INLINE double genrand_real1(void)
{
    return genrand_int32()*M_RAN_INVM32;
    /* divided by 2^32-1 */ 
}

/* JURGEN A DOORNIK */
INLINE double genrand_real2(void)
{
    return ((int)genrand_int32()) * M_RAN_INVM32 + 0.5;
}

/* generates a random number on [0,1) with 53-bit resolution*/
double genrand_res53(void) 
{ 
    unsigned long a=genrand_int32()>>5, b=genrand_int32()>>6; 
    return(a*67108864.0+b)*(1.0/9007199254740992.0); 
} 
#ifndef NOINCLUDE
#include "test_time2.c"
#endif
