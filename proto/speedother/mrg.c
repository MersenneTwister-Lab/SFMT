#include <string.h>
#if defined(__GNUC__)
  #include <stdint.h>
#endif
#include <stdio.h>
#include "random-inline.h"

static const int32_t m = 2147483647;
static const int32_t a1 = 107374182, q1 = 20, r1 = 7;
static const int32_t a5 = 104480, q5 = 20554, r5 = 1727;

static int32_t x1, x2, x3, x4, x5;

INLINE uint32_t
gen_rand (void)
{
  int32_t p1, h1, p5, h5;

  h5 = x5 / q5;
  p5 = a5 * (x5 - h5 * q5) - h5 * r5;
  if (p5 > 0)
    p5 -= m;

  h1 = x1 / q1;
  p1 = a1 * (x1 - h1 * q1) - h1 * r1;
  if (p1 < 0)
    p1 += m;

  x5 = x4;
  x4 = x3;
  x3 = x2;
  x2 = x1;

  x1 = p1 + p5;

  if (x1 < 0)
    x1 += m;

  return x1;
}

INLINE void
init_gen_rand (uint32_t s)
{
  /* An entirely adhoc way of seeding! This does **not** come from
     L'Ecuyer et al */

  if (s == 0)
    s = 1;      /* default seed is 1 */

#define LCG(n) ((69069 * n) & 0xffffffffUL)
  s = LCG (s);
  x1 = s % m;
  s = LCG (s);
  x2 = s % m;
  s = LCG (s);
  x3 = s % m;
  s = LCG (s);
  x4 = s % m;
  s = LCG (s);
  x5 = s % m;

  /* "warm it up" with at least 5 calls to go through
     all the x values */

  gen_rand ();
  gen_rand ();
  gen_rand ();
  gen_rand ();
  gen_rand ();
  gen_rand ();

  return;
}

INLINE unsigned int get_onetime_rnds(void) {
    return 624;
}

INLINE void fill_array_block(uint32_t array[], uint32_t block_num)
{
    int i;
    for (i = 0; i < 624 * block_num; i++) {
	array[i] = gen_rand();
    }
}

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif
