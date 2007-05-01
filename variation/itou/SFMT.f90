!===============================================
! SFMT for Fortran 90 (95?)
! file name : SFMT.f90
! auther : Atsushi ITO (Nagoya Univ.)
! version 20070501 (original c source version : 1.2)
!
!===============================================
!/** ORIGINAL_SOURCECODE *************************
! * @file  SFMT.c
! * @brief SIMD oriented Fast Mersenne Twister(SFMT)
! *
! * @author Mutsuo Saito (Hiroshima University)
! * @author Makoto Matsumoto (Hiroshima University)
! *
! * Copyright (C) 2006,2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
! * University. All rights reserved.
! *
! * The new BSD License is applied to this software, see LICENSE.txt
! */



module MOD_SFMT
    implicit none
!/*-----------------
!  BASIC DEFINITIONS
!  -----------------*/
!/** Mersenne Exponent. The period of the sequence 
! *  is a multiple of 2^MEXP-1.
! * #define MEXP 19937 */
!/** SFMT generator has an internal state array of 128-bit integers,
! * and N is its size. */
    integer,parameter:: MEXP = 19937
    integer,parameter:: N = (MEXP / 128 + 1)
!/** N32 is the size of internal state array when regarded as an array
! * of 32-bit integers.*/
    integer,parameter:: N32 = (N * 4)
!/** N64 is the size of internal state array when regarded as an array
! * of 64-bit integers.*/
    integer,parameter:: N64 = (N * 2)
    


    
    integer*4:: psfmt32(0:N32-1)
    integer*8:: psfmt64(0:N64-1)
    integer:: idx
    integer:: initialized = 0


!====for MEXP == 19937
    integer,parameter:: POS1 = 122
    integer,parameter:: SL1 = 18
    integer,parameter:: SL2 = 1
    integer,parameter:: SR1 = 11
    integer,parameter:: SR2 = 1
!   integer,parameter:: MSK1 =      ! = 0x001fffefU = 0xdfffffefU
!   integer,parameter:: MSK2 =      ! = 0x001ecb7fU = 0xddfecb7fU
!   integer,parameter:: MSK3 =      ! = 0x001affffU = 0xbffaffffU
!   integer,parameter:: MSK4 =      ! = 0x001ffff6U = 0xbffffff6U
    integer*8,parameter:: MSK12 = 8667995624701935_8    ! = 0x001ecb7f001fffefU
    integer*8,parameter:: MSK34 = 9007156306837503_8    ! = 0x001ffff6001affffU
    integer*8,parameter:: MSK_L = 70364449226751_8  ! = 0x00003fff00003fffU
    integer*4,parameter:: PARITY1 = 1
    integer*4,parameter:: PARITY2 = 0
    integer*4,parameter:: PARITY3 = 0
    integer*4,parameter:: PARITY4 = 331998852
!#define ALTI_SL2_PERM \
!(vector unsigned char)(1,2,3,23,5,6,7,0,9,10,11,4,13,14,15,8)
!#define ALTI_SL2_PERM64 \
!(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
!#define ALTI_SR2_PERM \
!(vector unsigned char)(7,0,1,2,11,4,5,6,15,8,9,10,17,12,13,14)
!#define ALTI_SR2_PERM64 \
!(vector unsigned char)(15,0,1,2,3,4,5,6,17,8,9,10,11,12,13,14)
!#define IDSTR  "SFMT-19937:122-18-1-11-1:dfffffef-ddfecb7f-bffaffff-bffffff6"
    
    integer*4:: parity(0:3)= (/PARITY1, PARITY2, PARITY3, PARITY4/)

end module

!#include <string.h>
!#include <assert.h>
!#include "SFMT.h"
!#include "SFMT-params.h"
!
!#if defined(ALTIVEC)
!  #include "SFMT-alti.h"
!#elif defined(SSE2)
!  #include "SFMT-sse2.h"
!#else
!/*------------------------------------------
!  128-bit SIMD like data type for standard C
!  ------------------------------------------*/
!/** 128-bit data structure */
!struct W128_T {
!    uint32_t u[4];
!};
!
!/** 128-bit data type */
!typedef struct W128_T w128_t;
!
!#endif
!
!/*--------------------------------------
!  FILE GLOBAL VARIABLES
!  internal state, index counter and flag 
!  --------------------------------------*/
!/** the 128-bit internal state array */
!static w128_t sfmt[N];
!/** the 32bit integer pointer to the 128-bit internal state array */
!static uint32_t *psfmt32 = &sfmt[0].u[0];
!#if !defined(BIG_ENDIAN64) || defined(ONLY64)
!/** the 64bit integer pointer to the 128-bit internal state array */
!static uint64_t *psfmt64 = (uint64_t *)&sfmt[0].u[0];
!#endif
!/** index counter to the 32-bit internal state array */
!static int idx;
!/** a flag: it is 0 if and only if the internal state is not yet
! * initialized. */
!static int initialized = 0;
!/** a parity check vector which certificate the period of 2^{MEXP} */
!static uint32_t parity[4] = {PARITY1, PARITY2, PARITY3, PARITY4};
!
!/*----------------
!  STATIC FUNCTIONS
!  ----------------*/
!inline static int idxof(int i);
!inline static void rshift128(w128_t *out,  w128_t const *in, int shift);
!inline static void lshift128(w128_t *out,  w128_t const *in, int shift);
!inline static void gen_rand_all(void);
!inline static void gen_rand_array(w128_t array[], int size);
!inline static uint32_t func1(uint32_t x);
!inline static uint32_t func2(uint32_t x);
!static void period_certification(void);
!#if defined(BIG_ENDIAN64) && !defined(ONLY64)
!inline static void swap(w128_t array[], int size);
!#endif
!
!#if defined(ALTIVEC)
!  #include "SFMT-alti.c"
!#elif defined(SSE2)
!  #include "SFMT-sse2.c"
!#endif
!
!/**
! * This function simulate a 64-bit index of LITTLE ENDIAN 
! * in BIG ENDIAN machine.
! */
!#ifdef ONLY64
!inline static int idxof(int i) {
!    return i ^ 1;
!}
!#else
!inline static int idxof(int i) {
!integer function idxof(i)
!   integer,intent(in):: i
!   idxof = i
!end function
!    return i;
!}
!#endif
!/**
! * This function simulates SIMD 128-bit right shift by the standard C.
! * The 128-bit integer given in in is shifted by (shift * 8) bits.
! * This function simulates the LITTLE ENDIAN SIMD.
! * @param out the output of this function
! * @param in the 128-bit data to be shifted
! * @param shift the shift value
! */
!#ifdef ONLY64
!inline static void rshift128(w128_t *out, w128_t const *in, int shift) {
!    uint64_t th, tl, oh, ol;
!
!    th = ((uint64_t)in->u[2] << 32) | ((uint64_t)in->u[3]);
!    tl = ((uint64_t)in->u[0] << 32) | ((uint64_t)in->u[1]);
!
!    oh = th >> (shift * 8);
!    ol = tl >> (shift * 8);
!    ol |= th << (64 - shift * 8);
!    out->u[0] = (uint32_t)(ol >> 32);
!    out->u[1] = (uint32_t)ol;
!    out->u[2] = (uint32_t)(oh >> 32);
!    out->u[3] = (uint32_t)oh;
!}
!#else
!inline static void rshift128(w128_t *out, w128_t const *in, int shift) {
subroutine rshift128(outTop, outBtm, inTop, inBtm, shift)
    implicit none
    integer*8,intent(out):: outTop, outBtm
    integer*8,intent(in):: inTop, inBtm
    integer:: shift
!   integer*8:: th, tl, oh, ol;
    
    outTop = ishft(inTop, -shift * 8)
    outBtm = ior(ishft(inTop, 64-shift * 8), ishft(inBtm, -shift * 8))
!   th = ((uint64_t)in->u[3] << 32) | ((uint64_t)in->u[2]);
!   tl = ((uint64_t)in->u[1] << 32) | ((uint64_t)in->u[0]);
!
!   oh = th >> (shift * 8);
!   ol = tl >> (shift * 8);
!   ol |= th << (64 - shift * 8);
!   out->u[1] = (uint32_t)(ol >> 32);
!   out->u[0] = (uint32_t)ol;
!   out->u[3] = (uint32_t)(oh >> 32);
!   out->u[2] = (uint32_t)oh;
end subroutine
!#endif
!/**
! * This function simulates SIMD 128-bit left shift by the standard C.
! * The 128-bit integer given in in is shifted by (shift * 8) bits.
! * This function simulates the LITTLE ENDIAN SIMD.
! * @param out the output of this function
! * @param in the 128-bit data to be shifted
! * @param shift the shift value
! */
!#ifdef ONLY64
!inline static void lshift128(w128_t *out, w128_t const *in, int shift) {
!    uint64_t th, tl, oh, ol;
!
!    th = ((uint64_t)in->u[2] << 32) | ((uint64_t)in->u[3]);
!    tl = ((uint64_t)in->u[0] << 32) | ((uint64_t)in->u[1]);
!
!    oh = th << (shift * 8);
!    ol = tl << (shift * 8);
!    oh |= tl >> (64 - shift * 8);
!    out->u[0] = (uint32_t)(ol >> 32);
!    out->u[1] = (uint32_t)ol;
!    out->u[2] = (uint32_t)(oh >> 32);
!    out->u[3] = (uint32_t)oh;
!}
!#else
!inline static void lshift128(w128_t *out, w128_t const *in, int shift) {
subroutine lshift128(outTop, outBtm, inTop, inBtm, shift)
    implicit none
    integer*8,intent(out):: outTop, outBtm
    integer*8,intent(in):: inTop, inBtm
    integer:: shift
    
    outTop = ior(ishft(inTop, shift * 8), ishft(inBtm, shift * 8-64))
    outBtm = ishft(inBtm, shift * 8)
end subroutine
!    uint64_t th, tl, oh, ol;
!
!    th = ((uint64_t)in->u[3] << 32) | ((uint64_t)in->u[2]);
!    tl = ((uint64_t)in->u[1] << 32) | ((uint64_t)in->u[0]);
!
!    oh = th << (shift * 8);
!    ol = tl << (shift * 8);
!    oh |= tl >> (64 - shift * 8);
!    out->u[1] = (uint32_t)(ol >> 32);
!    out->u[0] = (uint32_t)ol;
!    out->u[3] = (uint32_t)(oh >> 32);
!    out->u[2] = (uint32_t)oh;
!}
!#endif
!
!/**
! * This function represents the recursion formula.
! * @param r output
! * @param a a 128-bit part of the internal state array
! * @param b a 128-bit part of the internal state array
! * @param c a 128-bit part of the internal state array
! * @param d a 128-bit part of the internal state array
! */
!#ifdef ONLY64
!inline static void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *c,
!               w128_t *d) {
!    w128_t x;
!    w128_t y;
!
!    lshift128(&x, a, SL2);
!    rshift128(&y, c, SR2);
!    r->u[0] = a->u[0] ^ x.u[0] ^ ((b->u[0] >> SR1) & MSK2) ^ y.u[0] 
!   ^ (d->u[0] << SL1);
!    r->u[1] = a->u[1] ^ x.u[1] ^ ((b->u[1] >> SR1) & MSK1) ^ y.u[1] 
!   ^ (d->u[1] << SL1);
!    r->u[2] = a->u[2] ^ x.u[2] ^ ((b->u[2] >> SR1) & MSK4) ^ y.u[2] 
!   ^ (d->u[2] << SL1);
!    r->u[3] = a->u[3] ^ x.u[3] ^ ((b->u[3] >> SR1) & MSK3) ^ y.u[3] 
!   ^ (d->u[3] << SL1);
!}
!#else
!inline static void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *c,
!               w128_t *d) {
subroutine do_recursion(rTop, rBtm, aTop, aBtm, bTop, bBtm, cTop, cBtm, dTop, dBtm)
    use MOD_SFMT
    implicit none
    
    integer*8,intent(out):: rTop, rBtm
    integer*8,intent(in):: aTop, aBtm, bTop, bBtm, cTop, cBtm, dTop, dBtm
!    w128_t x;
!    w128_t y;
    integer*8:: xTop, xBtm, yTop, yBtm
    
    call lshift128(xTop,xBtm, aTop,aBtm, SL2)
    call rshift128(ytop,yBtm, cTop,cBtm, SR2)
                            
    rBtm = ieor(aBtm, xBtm)
    rTop = ieor(aTop, xTop)
    rBtm = ieor(rBtm, iand(ishft(bBtm, -SR1), MSK12))
    rTop = ieor(rTop, iand(ishft(bTop, -SR1), MSK34))
    rBtm = ieor(rBtm, yBtm)
    rTop = ieor(rTop, yTop)
    rBtm = ieor(rBtm, ishft(iand(dBtm, MSK_L), SL1))
    rTop = ieor(rTop, ishft(iand(dTop, MSK_L), SL1))


end subroutine
!#endif
!
!#if (!defined(ALTIVEC)) && (!defined(SSE2))
!/**
! * This function fills the internal state array with pseudorandom
! * integers.
! */
subroutine gen_rand_all()
    use MOD_SFMT
    implicit none
    
    integer:: i
    integer*8:: r1Top, r1Btm, r2Top, r2Btm

    r1Btm = psfmt64(N64 - 4)
    r1Top = psfmt64(N64 - 3)
    r2Btm = psfmt64(N64 - 2)
    r2Top = psfmt64(N64 - 1)

    do i=0,N - POS1 - 1
         
        call do_recursion(psfmt64(i*2 + 1),psfmt64(i*2), psfmt64(i*2 + 1),psfmt64(i*2), psfmt64((i + POS1)*2+1), psfmt64((i + POS1)*2), r1Top, r1Btm, r2Top, r2Btm);
        r1Btm = r2Btm
        r1Top = r2Top
        r2Btm = psfmt64(i*2)
        r2Top = psfmt64(i*2 + 1)
        
    end do
    do i=N - POS1, N-1
        call do_recursion(psfmt64(i*2 + 1),psfmt64(i*2), psfmt64(i*2 + 1),psfmt64(i*2), psfmt64((i + POS1 - N)*2+1), psfmt64((i + POS1 - N)*2), r1Top, r1Btm, r2Top, r2Btm);
        r1Btm = r2Btm
        r1Top = r2Top
        r2Btm = psfmt64(i*2)
        r2Top = psfmt64(i*2 + 1)
    end do
end subroutine

!
!/**
! * This function fills the user-specified array with pseudorandom
! * integers.
! *
! * @param array an 128-bit array to be filled by pseudorandom numbers.  
! * @param size number of 128-bit pseudorandom numbers to be generated.
! */
!inline static void gen_rand_array(w128_t array[], int size) {
!    int i, j;
!    w128_t *r1, *r2;
!
!    r1 = &sfmt[N - 2];
!    r2 = &sfmt[N - 1];
!    for (i = 0; i < N - POS1; i++) {
!   do_recursion(&array[i], &sfmt[i], &sfmt[i + POS1], r1, r2);
!   r1 = r2;
!   r2 = &array[i];
!    }
!    for (; i < N; i++) {
!   do_recursion(&array[i], &sfmt[i], &array[i + POS1 - N], r1, r2);
!   r1 = r2;
!   r2 = &array[i];
!    }
!    for (; i < size - N; i++) {
!   do_recursion(&array[i], &array[i - N], &array[i + POS1 - N], r1, r2);
!   r1 = r2;
!   r2 = &array[i];
!    }
!    for (j = 0; j < 2 * N - size; j++) {
!   sfmt[j] = array[j + size - N];
!    }
!    for (; i < size; i++, j++) {
!   do_recursion(&array[i], &array[i - N], &array[i + POS1 - N], r1, r2);
!   r1 = r2;
!   r2 = &array[i];
!   sfmt[j] = array[i];
!    }
!}
!#endif
!
!#if defined(BIG_ENDIAN64) && !defined(ONLY64) && !defined(ALTIVEC)
!inline static void swap(w128_t array[], int size) {
!    int i;
!    uint32_t x, y;
!
!    for (i = 0; i < size; i++) {
!   x = array[i].u[0];
!   y = array[i].u[2];
!   array[i].u[0] = array[i].u[1];
!   array[i].u[2] = array[i].u[3];
!   array[i].u[1] = x;
!   array[i].u[3] = y;
!    }
!}
!#endif
!/**
! * This function represents a function used in the initialization
! * by init_by_array
! * @param x 32-bit integer
! * @return 32-bit integer
! */
!static uint32_t func1(uint32_t x) {
!    return (x ^ (x >> 27)) * (uint32_t)1664525UL;
!}
!
!/**
! * This function represents a function used in the initialization
! * by init_by_array
! * @param x 32-bit integer
! * @return 32-bit integer
! */
!static uint32_t func2(uint32_t x) {
!    return (x ^ (x >> 27)) * (uint32_t)1566083941UL;
!}
!
!/**
! * This function certificate the period of 2^{MEXP}
! */
!static void period_certification(void) {
subroutine period_certification()
    use MOD_SFMT
    implicit none
    
    integer:: inner = 0
    integer:: i, j
    integer*4:: work

    do i = 0, 3
        work = iand(psfmt32(i), parity(i))
        do j = 0, 31
            inner = ieor(inner, iand(work, 1))
            work = ishft(work,-1)
        end do
    end do
!    /* check OK */
    if (inner == 1) return
    
    
!    /* check NG, and modification */
    do i = 0, 3
        work = 1;
        do j = 0, 31
            if (iand(work, parity(i)) /= 0) then
                psfmt32(i) = ieor(psfmt32(i), work)
                return
            end if
            work = ishft(work,1)
        end do
    end do
end subroutine

!
!/*----------------
!  PUBLIC FUNCTIONS
!  ----------------*/
!/**
! * This function returns the identification string.
! * The string shows the word size, the Mersenne exponent,
! * and all parameters of this generator.
! */
!char *get_idstring(void) {
!    return IDSTR;
!}
!
!/**
! * This function returns the minimum size of array used for \b
! * fill_array32() function.
! * @return minimum size of array used for fill_array32() function.
! */
!int get_min_array_size32(void) {
!    return N32;
!}
!
!/**
! * This function returns the minimum size of array used for \b
! * fill_array64() function.
! * @return minimum size of array used for fill_array64() function.
! */
!int get_min_array_size64(void) {
!    return N64;
!}
!
!#ifndef ONLY64
!/**
! * This function generates and returns 32-bit pseudorandom number.
! * init_gen_rand or init_by_array must be called before this function.
! * @return 32-bit pseudorandom number
! */
integer*4 function gen_rand32()
    use MOD_SFMT
    implicit none

    if(initialized==0)stop
    
    if (idx >= N32) then
        call gen_rand_all()
        idx = 0
    end if
    if (btest(idx,0))then
        gen_rand32 = ibits(psfmt64(idx/2),32,32)
    else
        gen_rand32 = ibits(psfmt64(idx/2),0,32)
    end if
    idx = idx + 1
end function
!#endif
!/**
! * This function generates and returns 64-bit pseudorandom number.
! * init_gen_rand or init_by_array must be called before this function.
! * The function gen_rand64 should not be called after gen_rand32,
! * unless an initialization is again executed. 
! * @return 64-bit pseudorandom number
! */
integer*8 function gen_rand64()
    use MOD_SFMT
    implicit none
    integer::i

!
    if (idx >= N32) then
        call gen_rand_all()
        idx = 0
    end if
    gen_rand64 = psfmt64(idx/2);
    idx = idx + 2
end function
!    idx += 2;
!    return r;
!#endif
!}
!
!#ifndef ONLY64
!/**
! * This function generates pseudorandom 32-bit integers in the
! * specified array[] by one call. The number of pseudorandom integers
! * is specified by the argument size, which must be at least 624 and a
! * multiple of four.  The generation by this function is much faster
! * than the following gen_rand function.
! *
! * For initialization, init_gen_rand or init_by_array must be called
! * before the first call of this function. This function can not be
! * used after calling gen_rand function, without initialization.
! *
! * @param array an array where pseudorandom 32-bit integers are filled
! * by this function.  The pointer to the array must be \b "aligned"
! * (namely, must be a multiple of 16) in the SIMD version, since it
! * refers to the address of a 128-bit integer.  In the standard C
! * version, the pointer is arbitrary.
! *
! * @param size the number of 32-bit pseudorandom integers to be
! * generated.  size must be a multiple of 4, and greater than or equal
! * to (MEXP / 128 + 1) * 4.
! *
! * @note \b memalign or \b posix_memalign is available to get aligned
! * memory. Mac OSX doesn't have these functions, but \b malloc of OSX
! * returns the pointer to the aligned memory block.
! */
!inline void fill_array32(uint32_t array[], int size) {
!    assert(initialized);
!    assert(idx == N32);
!    assert(size % 4 == 0);
!    assert(size >= N32);
!
!    gen_rand_array((w128_t *)array, size / 4);
!    idx = N32;
!}
!#endif
!
!/**
! * This function generates pseudorandom 64-bit integers in the
! * specified array[] by one call. The number of pseudorandom integers
! * is specified by the argument size, which must be at least 312 and a
! * multiple of two.  The generation by this function is much faster
! * than the following gen_rand function.
! *
! * For initialization, init_gen_rand or init_by_array must be called
! * before the first call of this function. This function can not be
! * used after calling gen_rand function, without initialization.
! *
! * @param array an array where pseudorandom 64-bit integers are filled
! * by this function.  The pointer to the array must be "aligned"
! * (namely, must be a multiple of 16) in the SIMD version, since it
! * refers to the address of a 128-bit integer.  In the standard C
! * version, the pointer is arbitrary.
! *
! * @param size the number of 64-bit pseudorandom integers to be
! * generated.  size must be a multiple of 2, and greater than or equal
! * to (MEXP / 128 + 1) * 2
! *
! * @note \b memalign or \b posix_memalign is available to get aligned
! * memory. Mac OSX doesn't have these functions, but \b malloc of OSX
! * returns the pointer to the aligned memory block.
! */
!inline void fill_array64(uint64_t array[], int size) {
!    assert(initialized);
!    assert(idx == N32);
!    assert(size % 2 == 0);
!    assert(size >= N64);
!
!    gen_rand_array((w128_t *)array, size / 2);
!    idx = N32;
!
!#if defined(BIG_ENDIAN64) && !defined(ONLY64)
!    swap((w128_t *)array, size /2);
!#endif
!}
!
!/**
! * This function initializes the internal state array with a 32-bit
! * integer seed.
! *
! * @param seed a 32-bit integer used as the seed.
! */
!void init_gen_rand(uint32_t seed) {
subroutine init_gen_rand(seed)
    use MOD_SFMT
    implicit none
    
    integer*4:: seed
    integer:: i
    integer*8:: temp64
    
    psfmt32(0) = seed;
    do i=1, N32-1
        temp64 = ieor( psfmt32(i-1), ishft( psfmt32(i-1), -30 ) )
        temp64 = 1812433253_8 * temp64 + i
        psfmt32(i) = ibits( temp64, 0, 32 )
    end do
    
    
    idx = N32
    call period_certification()

!original method by A.I.
    do i=0, N64-1
        psfmt64(i) = psfmt32(i*2 + 1)
        temp64 = psfmt32(i*2)
        temp64 = iand(temp64, 4294967295_8)
        psfmt64(i) = ior(ishft(psfmt64(i), 32), temp64)
    end do
    
    initialized = 1
end subroutine
!
!/**
! * This function initializes the internal state array,
! * with an array of 32-bit integers used as the seeds
! * @param init_key the array of 32-bit integers, used as a seed.
! * @param key_length the length of init_key.
! */
!void init_by_array(uint32_t init_key[], int key_length) {
!    int i, j, count;
!    uint32_t r;
!    int lag;
!    int mid;
!    int size = N * 4;
!
!    if (size >= 623) {
!   lag = 11;
!    } else if (size >= 68) {
!   lag = 7;
!    } else if (size >= 39) {
!   lag = 5;
!    } else {
!   lag = 3;
!    }
!    mid = (size - lag) / 2;
!
!    memset(sfmt, 0x8b, sizeof(sfmt));
!    if (key_length + 1 > N32) {
!   count = key_length + 1;
!    } else {
!   count = N32;
!    }
!    r = func1(psfmt32[idxof(0)] ^ psfmt32[idxof(mid)] 
!         ^ psfmt32[idxof(N32 - 1)]);
!    psfmt32[idxof(mid)] += r;
!    r += key_length;
!    psfmt32[idxof(mid + lag)] += r;
!    psfmt32[idxof(0)] = r;
!    i = 1;
!    count--;
!    for (i = 1, j = 0; (j < count) && (j < key_length); j++) {
!   r = func1(psfmt32[idxof(i)] ^ psfmt32[idxof((i + mid) % N32)] 
!         ^ psfmt32[idxof((i + N32 - 1) % N32)]);
!   psfmt32[idxof((i + mid) % N32)] += r;
!   r += init_key[j] + i;
!   psfmt32[idxof((i + mid + lag) % N32)] += r;
!   psfmt32[idxof(i)] = r;
!   i = (i + 1) % N32;
!    }
!    for (; j < count; j++) {
!   r = func1(psfmt32[idxof(i)] ^ psfmt32[idxof((i + mid) % N32)] 
!         ^ psfmt32[idxof((i + N32 - 1) % N32)]);
!   psfmt32[idxof((i + mid) % N32)] += r;
!   r += i;
!   psfmt32[idxof((i + mid + lag) % N32)] += r;
!   psfmt32[idxof(i)] = r;
!   i = (i + 1) % N32;
!    }
!    for (j = 0; j < N32; j++) {
!   r = func2(psfmt32[idxof(i)] + psfmt32[idxof((i + mid) % N32)] 
!         + psfmt32[idxof((i + N32 - 1) % N32)]);
!   psfmt32[idxof((i + mid) % N32)] ^= r;
!   r -= i;
!   psfmt32[idxof((i + mid + lag) % N32)] ^= r;
!   psfmt32[idxof(i)] = r;
!   i = (i + 1) % N32;
!    }
!
!    idx = N32;
!    period_certification();
!    initialized = 1;
!}


!/** generates a random number on [0,1) with 53-bit resolution*/
real*8 function genrand_res53() 
    integer*8:: gen_rand64
    integer*8:: i
    i = gen_rand64()
    i = ishft(i, -1)
    genrand_res53 = dble(i) * (1d0/9223372036854775808d0)
end function

