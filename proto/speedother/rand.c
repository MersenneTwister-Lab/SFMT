#include <string.h>
#if defined(__GNUC__)
  #include <stdint.h>
#endif
#include <stdio.h>
#include "random-inline.h"

unsigned long int rand_get (void);

static uint32_t rand_x;

INLINE void init_gen_rand(uint32_t seed)
{
    rand_x = seed;
}

INLINE unsigned int get_onetime_rnds(void) {
    return 624;
}

INLINE uint32_t gen_rand(void)
{
  rand_x = (1103515245 * rand_x + 12345) & 0x7fffffffUL;

  return rand_x;
}

INLINE void fill_array_block(uint32_t array[], uint32_t block_num)
{
    int i;
    uint32_t r = rand_x;
    for (i = 0; i < 624 * block_num; i++) {
	r = (1103515245 * r + 12345) & 0x7fffffffUL;
	array[i] = r;
    }
    rand_x = r;
}

/*
int main() {
  rand_state_t rand;
  int i;
  unsigned int r = 0;
  
  rand_set(&rand, 4923858);
  for (i = 0; i < 1000000000; i++) {
    r ^= rand_get(&rand);
  }
  return r;

}
*/
  
#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif
