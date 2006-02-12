#ifndef __SHORTBASE_H__
#define __SHORTBASE_H__ 1

#include <stdbool.h>
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
int get_shortest(uint8_t dependents[], in_status bases[]);
int get_dependent_trans(uint8_t dependent[], unsigned int array[]);
int dependent_rows(uint8_t result[], uint8_t ar[][32]);
void add_column(uint8_t ar[][32], int src, int dist);
void exchange_column(uint8_t ar[][32], int s, int t);
void convert(uint8_t ar[][32], unsigned int array[], int bit_len);

#endif
