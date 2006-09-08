#include <string.h>
#if defined(__GNUC__)
  #include <stdint.h>
#endif
#include <stdio.h>
#include "random-inline.h"

static int si, sj;
static int32_t sx[63];

static INLINE uint32_t
random_get (int * i, int * j, int n, int32_t * x)
{
  int32_t k ;

  x[*i] += x[*j] ;
  k = (x[*i] >> 1) & 0x7FFFFFFF ;
  
  (*i)++ ;
  if (*i == n)
    *i = 0 ;
  
  (*j)++ ;
  if (*j == n)
    *j = 0 ;

  return (uint32_t)k ;
}

INLINE uint32_t
gen_rand (void)
{
  uint32_t k = random_get (&si, &sj, 63, sx) ; 
  return k ;
}

static void 
glibc2_initialize (int32_t * x, int n, uint32_t s)
{
  int i; 

  if (s == 0)
    s = 1 ;

  x[0] = s;

  for (i = 1 ; i < n ; i++)
    {
      const int32_t h = s / 127773;
      const int32_t t = 16807 * (s - h * 127773) - h * 2836;
      if (t < 0)
        {
          s = t + 2147483647 ;
        }
      else
        {
          s = t ;
        }

    x[i] = s ;
    }
}

void
init_gen_rand (uint32_t s)
{
  int i;

  glibc2_initialize (sx, 63, s) ;

  si = 1;
  sj = 0;
  
  for (i = 0 ; i < 10 * 63 ; i++)
    gen_rand () ; 
}

INLINE void fill_array(uint32_t array[], int size)
{
    int i, j;
    int32_t *sa;

    sa = (int32_t *)array;
    sa[0] = sx[1] + sx[0];
    for (i = 0; i < 62; i++) {
	sa[i] = sx[i] + sx[i + 1];
    }
    sa[62] = sx[62] + sa[0];
    for (i = 63; i < size - 63; i++) {
	sa[i] = sa[i - 63] + sa[i - 62];
	sa[i - 63] = (sa[i - 63] >> 1) & 0x7FFFFFFF ;
    }
    for (j = 0; i < size; i++, j++) {
	sa[i] = sa[i - 63] + sa[i - 62];
	sa[i - 63] = (sa[i - 63] >> 1) & 0x7FFFFFFF ;
	sx[j] = sa[i];
    }
    for (; i < size + 63; i++) {
	sa[i - 63] = (sa[i - 63] >> 1) & 0x7FFFFFFF ;
    }
}

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif
