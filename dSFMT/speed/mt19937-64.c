/* 
   A C-program for MT19937-64 (2004/9/29 version).
   Coded by Takuji Nishimura and Makoto Matsumoto.

   This is a 64-bit version of Mersenne Twister pseudorandom number
   generator.

   Before using, initialize the state by using init_genrand64(seed)  
   or init_by_array64(init_key, key_length).

   Copyright (C) 2004, Makoto Matsumoto and Takuji Nishimura,
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

   References:
   T. Nishimura, ``Tables of 64-bit Mersenne Twisters''
     ACM Transactions on Modeling and 
     Computer Simulation 10. (2000) 348--357.
   M. Matsumoto and T. Nishimura,
     ``Mersenne Twister: a 623-dimensionally equidistributed
       uniform pseudorandom number generator''
     ACM Transactions on Modeling and 
     Computer Simulation 8. (Jan. 1998) 3--30.

   Any feedback is very welcome.
   http://www.math.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove spaces)
*/

#include <inttypes.h>
#include <stdio.h>
#include "mt64.h"
#include "random.h"

#define NN 312
#define MM 156
#define MATRIX_A UINT64_C(0xB5026F5AA96619E9)
#define UM UINT64_C(0xFFFFFFFF80000000) /* Most significant 33 bits */
#define LM UINT64_C(0x7FFFFFFF) /* Least significant 31 bits */


/* The array for the state vector */
static uint64_t mt[NN+1]; 
/* mti==NN+1 means mt[NN] is not initialized */
static int mti=NN+1; 

/* initializes mt[NN] with a seed */
void init_gen_rand(uint64_t seed)
{
    mt[0] = seed;
    for (mti=1; mti<NN; mti++) 
        mt[mti] =  (UINT64_C(6364136223846793005) 
		    * (mt[mti-1] ^ (mt[mti-1] >> 62)) + mti);
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
void init_by_array64(uint64_t init_key[], uint64_t key_length)
{
    uint64_t i, j, k;
    init_gen_rand(UINT64_C(19650218));
    i=1; j=0;
    k = (NN>key_length ? NN : key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 62)) 
			  * UINT64_C(3935559000370003845)))
	    + init_key[j] + j; /* non linear */
        i++; j++;
        if (i>=NN) { mt[0] = mt[NN-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=NN-1; k; k--) {
	/* non linear */
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 62)) 
			  * UINT64_C(2862933555777941757))) - i;
        i++;
        if (i>=NN) { mt[0] = mt[NN-1]; i=1; }
    }

    mt[0] = UINT64_C(1) << 63; /* MSB is 1; assuring non-zero initial array */ 
}

inline static uint64_t temper(uint64_t x) {
    x ^= (x >> 29) & UINT64_C(0x5555555555555555);
    x ^= (x << 17) & UINT64_C(0x71D67FFFEDA60000);
    x ^= (x << 37) & UINT64_C(0xFFF7EEE000000000);
    x ^= (x >> 43);

    return x;
}

/* generates a random number on [0, 2^64-1]-interval */
inline uint64_t genrand64_int64(void)
{
    int i;
    uint64_t x;
    static uint64_t mag01[2]={0, MATRIX_A};

    if (mti >= NN) { /* generate NN words at one time */

        /* if init_genrand64() has not been called, */
        /* a default initial seed is used     */
#if 0
        if (mti == NN+1) 
            init_genrand64(UINT64_C(5489)); 
#endif
        for (i=0;i<NN-MM;i++) {
            x = (mt[i]&UM)|(mt[i+1]&LM);
            mt[i] = mt[i+MM] ^ (x>>1) ^ mag01[(int)(x&1)];
        }
        for (;i<NN-1;i++) {
            x = (mt[i]&UM)|(mt[i+1]&LM);
            mt[i] = mt[i+(MM-NN)] ^ (x>>1) ^ mag01[(int)(x & 1)];
        }
        x = (mt[NN-1]&UM)|(mt[0]&LM);
        mt[NN-1] = mt[MM-1] ^ (x>>1) ^ mag01[(int)(x & 1)];

        mti = 0;
    }
  
    x = mt[mti++];
    return temper(x);
}

inline void fill_array(double in_array[], int size)
{
    int i, j;
    uint64_t x;
    uint64_t *array = (uint64_t *)in_array;
    static uint64_t mag01[2]={0, MATRIX_A};

    mt[NN] = mt[0];
    for (i=0;i<NN-MM;i++) {
	x = (mt[i]&UM) | (mt[i+1]&LM);
	array[i] = mt[i+MM] ^ (x>>1) ^ mag01[(int)(x&1)];
    }
    for (;i<NN-1;i++) {
	x = (mt[i]&UM)|(mt[i+1]&LM);
	array[i] = array[i+(MM-NN)] ^ (x>>1) ^ mag01[(int)(x & 1)];
    }
    x = (mt[NN-1]&UM)|(array[0]&LM);
    array[NN-1] = array[MM-1] ^ (x>>1) ^ mag01[(int)(x & 1)];
    i++;
    for (; i < size - NN; i++) {
	x = (array[i - NN]&UM)|(array[i+1-NN]&LM);
	array[i] = array[i+(MM-NN)] ^ (x>>1) ^ mag01[(int)(x & 1)];
	array[i - NN] = temper(array[i - NN]);
	array[i - NN] = (array[i - NN] & LOW_MASK) | HIGH_CONST;
    }
    for (j = 0; j < 2 * NN - size; j++) {
	mt[j] = array[j + size - NN];
    }
    for (; i < size; i++, j++) {
	x = (array[i - NN]&UM)|(array[i+1-NN]&LM);
	array[i] = array[i+(MM-NN)] ^ (x>>1) ^ mag01[(int)(x & 1)];
	mt[j] = array[i];
	array[i - NN] = temper(array[i - NN]);
	array[i - NN] = (array[i - NN] & LOW_MASK) | HIGH_CONST;
    }
    for (j = size - NN; j < size; j++) {
	array[j] = temper(array[j]);
	array[j] = (array[j] & LOW_MASK) | HIGH_CONST;
    }
}

#ifdef MAIN
int main(void)
{
    int i;
    uint64_t init[4]={0x12345, 0x23456, 0x34567, 0x45678}, length=4;
    init_by_array64(init, length);
    printf("1000 outputs of genrand64_int64()\n");
    for (i=0; i<1000; i++) {
      printf("%20llu ", genrand64_int64());
      if (i%5==4) printf("\n");
    }
    printf("\n1000 outputs of genrand64_real2()\n");
    for (i=0; i<1000; i++) {
      printf("%10.8f ", genrand64_real2());
      if (i%5==4) printf("\n");
    }
    return 0;
}
#else
#include "test_time.c"
#endif
