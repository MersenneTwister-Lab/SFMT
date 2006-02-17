#ifndef __SHORTBASE_H__
#define __SHORTBASE_H__ 1

#include <stdbool.h>
#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>

NTL_CLIENT

/* internal status */
struct IN_STATUS {
  bool zero;
  bool special;
  unsigned int next;
  unsigned int count;
  ht_rand random;
};
typedef struct IN_STATUS in_status;
  
void dprintnext(in_status *st);
void dprintbase(char *file, int line, int num, in_status *st);
void set_bit_len(unsigned int len);
void set_special(in_status *st, unsigned int special_bit);
void set_normal(in_status *st, ht_rand *ht);
void add_status(in_status *dist, in_status *src);
void get_next_state(in_status *st);
int get_shortest_base(unsigned int bit_len, ht_rand *ht);
uint32_t get_shortest(bool dependents[], in_status bases[]);
int get_dependent_trans(bool dependent[], unsigned int array[]);
int dependent_rows(bool result[], mat_GF2& mat);
void convert(mat_GF2& mat, unsigned int array[], uint32_t bit_len);

#endif
