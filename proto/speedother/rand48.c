#include <string.h>
#if defined(__GNUC__)
  #include <stdint.h>
#endif
#include <stdio.h>
#include "random-inline.h"

static INLINE void rand48_advance (void);
//unsigned long int rand48_get (void);
//void rand48_set (unsigned long int s);

static const uint16_t a0 = 0xE66D ;
static const uint16_t a1 = 0xDEEC ;
static const uint16_t a2 = 0x0005 ;

static const uint16_t c0 = 0x000B ;

static uint16_t x0, x1, x2;

static INLINE void
rand48_advance ()
{

  /* work with unsigned long ints throughout to get correct integer
     promotions of any unsigned short ints */

  uint32_t a ;
  
  a = a0 * x0 + c0 ;
  x0 = (a & 0xFFFF) ;
 
  a >>= 16 ;

  /* although the next line may overflow we only need the top 16 bits
     in the following stage, so it does not matter */

  a += a0 * x1 + a1 * x0 ; 
  x1 = (a & 0xFFFF) ;

  a >>= 16 ;
  a += a0 * x2 + a1 * x1 + a2 * x0 ;
  x2 = (a & 0xFFFF) ;
}

INLINE uint32_t
gen_rand()
{

  rand48_advance () ;

  return (x2 << 16) + x1;
}

INLINE void
init_gen_rand (uint32_t s)
{
  if (s == 0)  /* default seed */
    {
      x0 = 0x330E ;
      x1 = 0xABCD ;
      x2 = 0x1234 ;
    }
  else 
    {
      x0 = 0x330E ;
      x1 = s & 0xFFFF ;
      x2 = (s >> 16) & 0xFFFF ;
    }

  return;
}

INLINE void fill_array(uint32_t array[], int size)
{
    int i;
    for (i = 0; i < size; i++) {
	array[i] = gen_rand();
    }
}

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif
