#ifndef __SHORTBASE_H__
#define __SHORTBASE_H__ 1

#include <stdbool.h>
#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>
#include "sfmt-cls.h"

NTL_CLIENT;

/* internal status */
struct IN_STATUS {
  bool zero;
  bool special;
  vec_GF2 next;
  unsigned int count;
  SFMT random;
};
typedef struct IN_STATUS in_status;
  
void dprintnext(in_status *st);
void dprintbase(char *file, int line, int num, in_status *st);
void set_bit_len(unsigned int len);
void set_special(in_status *st, unsigned int special_bit);
void set_normal(in_status *st, SFMT& sfmt);
void add_status(in_status *dist, in_status *src);
void get_next_state(in_status *st);
int get_shortest_base(unsigned int bit_len, SFMT& sfmt);
uint32_t get_shortest(bool dependents[], in_status bases[]);
bool get_dependent_trans(bool dependent[], vec_GF2 array[]);
bool dependent_rows(bool result[], mat_GF2& mat);
void convert(mat_GF2& mat, vec_GF2 array[], uint32_t bit_len);

#endif
