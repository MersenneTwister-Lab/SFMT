#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "ht-st.h"
#include "shortbase.h"
#include "debug.h"

#ifndef INV_DIST
static uint32_t mask_tab[] = { 
  0x80000000, 0xC0000000, 0xE0000000, 0xF0000000, 
  0xF8000000, 0xFC000000, 0xFE000000, 0xFF000000, 
  0xFF800000, 0xFFC00000, 0xFFE00000, 0xFFF00000,
  0xFFF80000, 0xFFFC0000, 0xFFFE0000, 0xFFFF0000, 
  0xFFFF8000, 0xFFFFC000, 0xFFFFE000, 0xFFFFF000, 
  0xFFFFF800, 0xFFFFFC00, 0xFFFFFE00, 0xFFFFFF00, 
  0xFFFFFF80, 0xFFFFFFC0, 0xFFFFFFE0, 0xFFFFFFF0, 
  0xFFFFFFF8, 0xFFFFFFFC, 0xFFFFFFFE, 0xFFFFFFFF};
#define NTH_BIT(i) (0x80000000U >> (i))
#define NTH_BIT2(i) (1U << (31 - (i)))
#else
static uint32_t mask_tab[] = {
  0x00000001, 0x00000003, 0x00000007, 0x0000000F,
  0x0000001F, 0x0000003F, 0x0000007F, 0x000000FF,
  0x000001FF, 0x000003FF, 0x000007FF, 0x00000FFF,
  0x00001FFF, 0x00003FFF, 0x00007FFF, 0x0000FFFF,
  0x0001FFFF, 0x0003FFFF, 0x0007FFFF, 0x000FFFFF,
  0x001FFFFF, 0x003FFFFF, 0x007FFFFF, 0x00FFFFFF,
  0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF, 0x0FFFFFFF,
  0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF};
#define NTH_BIT(i) (0x01U << ((bit_len - 1) - (i)))
#define NTH_BIT2(i) (1U << ((bit_len - 1) - (i)))
#endif

static unsigned int bit_len;
static unsigned int mask;

void dprintnext(in_status *st) {
  unsigned int i;

  for (i = 0; i < bit_len; i++) {
    //if ((st->next & (0x80000000U >> i)) != 0) {
    if ((st->next & NTH_BIT(i)) != 0) {
      fprintf(stderr, "1");
    } else {
      fprintf(stderr, "0");
    }
  }
}

void dprintbase(char *file, int line, int num, in_status *st) {
  fprintf(stderr, "%s:%d SB[", file, line);
  fprintf(stderr, "special:%c ", st->special ? 'S' : 'N');
  fprintf(stderr, "next:");
  dprintnext(st);
  fprintf(stderr, " count:%u ", st->count);
  fprintf(stderr, "bitlen:%u]\n", bit_len);
}

void set_bit_len(unsigned int len) {
  if ((len <= 0) || (len > 32)) {
    printf("bitLength error\n");
    exit(1);
  }
  bit_len = len;
  mask = mask_tab[bit_len - 1];
}

void set_special(in_status *st, unsigned int special_bit) {
  /*st->random = NULL;*/
  st->special = true;
  st->zero = false;
  st->count = 0;
  //st->next = 0x80000000U >> special_bit;
  st->next = NTH_BIT(special_bit);
}

void set_normal(in_status *st, ht_rand *ht) {
  int zero_count = 0;
  DPRINTHT("ht:", ht);
  st->random = *ht;
  st->special = false;
  st->zero = false;
  st->count = 0;
  st->next = gen_rand(&(st->random)) & mask;
  st->count++;
  while (st->next == 0) {
    zero_count++;
    if (zero_count > MAXDEGREE) {
      st->zero = true;
      break;
    }
    st->next = gen_rand(&(st->random)) & mask;
    st->count++;
  }
  DPRINTHT("ht:", &(st->random));
}

void add_status(in_status *dist, in_status *src) {
  unsigned int c;
  unsigned int n;

  c = dist->count;
  n = dist->next;
  if (dist->special && (!src->special)) {
    DPRINT("before copy", NULL);
    DPRINTHT("dist", &(dist->random));
    DPRINTHT("src ", &(src->random));
    *dist = *src;
    DPRINT("after copy", NULL);
    DPRINTHT("dist", &(dist->random));
  } else if ((! dist->special) && (! src->special)) {
    DPRINT("before add", NULL);
    DPRINTHT("dist", &(dist->random));
    DPRINTHT("src ", &(src->random));
    add(&(dist->random), &(src->random));
    DPRINT("after add", NULL);
    DPRINTHT("dist", &(dist->random));
  }
  dist->next = n ^ (src->next);
  dist->count = MIN(c, src->count);
}

void get_next_state(in_status *st) {
  int zero_count = 0;

  DPRINTHT("before next", &(st->random));
  if (st->special) {
    printf("Request next to special\n");
    exit(1);
  }
  if (st->zero) {
    return;
  }
  st->next = gen_rand(&(st->random)) & mask;
  st->count++;
  while (st->next == 0) {
    zero_count++;
    if (zero_count > MAXDEGREE) {
      st->zero = true;
      break;
    }
    st->next = gen_rand(&(st->random)) & mask;
    st->count++;
  }
  DPRINTHT("after next", &(st->random));
}
  
int get_shortest_base(unsigned int bit_len, ht_rand *ht) {
  static in_status bases[32 + 1];
  unsigned int next[bit_len + 1];
  uint8_t dependents[bit_len + 1];
  unsigned int shortest;
  unsigned int i;
  int ret;

  DPRINT("in get_shortest_base bit_len:%u", bit_len);
  set_bit_len(bit_len);
  for (i = 0; i < bit_len; i++) {
    set_special(&(bases[i]), i);
  }
  set_normal(&(bases[bit_len]), ht);
  for (;;) {
#ifdef DEBUG
    DPRINT("base", NULL);
    for (i = 0; i <= bit_len; i++) {
      DPRINTBASE(i, &(bases[i]));
    }
#endif
    for (i = 0; i <= bit_len; i++) {
      next[i] = bases[i].next;
    }
    ret = get_dependent_trans(dependents, next);
    if (ret < 0) {
      break;
    }
#ifdef DEBUG
    fprintf(stderr, "dependents:");
    for (i = 0; i <= bit_len; i++) {
      fprintf(stderr, "%1d", dependents[i]);
    }
    fprintf(stderr, "\n");
#endif
    shortest = get_shortest(dependents, bases);
    DPRINT("shortest:%u", shortest);
    for (i = 0; i <= bit_len; i++) {
      if (i == shortest) {
	continue;
      }
      if (dependents[i] != 0) {
	add_status(&(bases[shortest]), &(bases[i]));
      }
    }
    if (bases[shortest].next == 0) {
      get_next_state(&(bases[shortest]));
    } else {
      fprintf(stderr, "next is not zero\n");
    }
  }
  shortest = __INT_MAX__;
  for (i = 0; i <= bit_len; i++) {
    if (!bases[i].zero) {
      if (bases[i].count < shortest) {
	shortest = bases[i].count;
      }
    }
  }
  return shortest;
}

int get_shortest(uint8_t dependents[], in_status bases[]) {
  int index = 0;
  int min = __INT_MAX__;
  int i;

  for (i = 0; i <= bit_len; i++) {
    if (dependents[i] != 0) {
      if (bases[i].count < min) {
	min = bases[i].count;
	index = i;
      }
    }
  }
  return index;
}

int get_dependent_trans(uint8_t dependent[], unsigned int array[]) {
  uint8_t ar[bit_len + 1][32];
  int found;
  int q;
  int i;
  int j;

  convert(ar, array, bit_len);
  for (i = 0, q = 0; i < bit_len; i++, q++) {
    found = false;
    while ((q <= bit_len) && (!found)) {
      if (ar[q][i] != 0) {
	found = true;
      } else {
	for (j = i + 1; j < bit_len; j++) {
	  if (ar[q][j] != 0) {
	    exchange_column(ar, i, j);
	    found = true;
	    break;
	  }
	}
      }
      if (!found) {
	q++;
      }
    }
    for (j = 0; j < bit_len; j++) {
      if (j == i) {
	continue;
      }
      if (ar[q][j] != 0) {
	add_column(ar, i, j);
      }
    }
  }
  return dependent_rows(dependent, ar);
}

int dependent_rows(uint8_t result[], uint8_t ar[][32]) {
  int count;
  int dependent = false;
  int i, j, k;

  memset(result, 0, sizeof(char)*(bit_len +1));
  for (i = 0; i < bit_len; i++) {
    count = 0;
    for (j = 0; j <= bit_len; j++) {
      count += ar[j][i];
    }
    if (count > 1) {
      for (j = 0; j <= bit_len; j++) {
	if (ar[j][i] != 0) {
	  result[i] = 1;
	  dependent = true;
	}
      }
      break;
    }
  }
  if (!dependent) {
    return -1;
  }
  for (i = 0; i < bit_len; i++) {
    for (j = 0; j <= bit_len; j++) {
      if ((ar[j][i] != 0) && (result[j] != 0)) {
	for (k = 0; k <= bit_len; k++) {
	  if (ar[k][i] != 0) {
	    result[k] = 1;
	  }
	}
      }
    }
  }
  return 0;
}

void add_column(uint8_t ar[][32], int src, int dist) {
  int i;

  for (i = 0; i <= bit_len; i++) {
    ar[i][dist] ^= ar[i][src];
  }
}

void exchange_column(uint8_t ar[][32], int s, int t) {
  int i;
  uint8_t tmp;

  for (i = 0; i <= bit_len; i++) {
    tmp = ar[i][s];
    ar[i][s] = ar[i][t];
    ar[i][t] = tmp;
  }
}

void convert(uint8_t ar[][32], uint32_t array[], int bit_len) {
  uint32_t msk;
  int i, j;

  for (i = 0; i <= bit_len; i++) {
    for (j = 0; j < bit_len; j++) {
      //msk = 1U << (31 - j);
      msk = NTH_BIT2(j);
      if ((array[i] & msk) != 0) {
	ar[i][j] = 1;
      } else {
	ar[i][j] = 0;
      }
    }
  }
}
