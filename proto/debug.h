/* This file includes debugging utility */
/* only available for GCC */
#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <stdarg.h>
#include "sfmt.h"

void dprint(char *file, int line, char *fmt, ...);
//void dprintpoly(char *file, int line, char *s, uint64_t *poly);
//void dprintseq(char *file, int line, char *s, ht_rand *ht, unsigned int bitpos);

#ifdef DEBUG
#define DPRINT(fmt, ...) dprint(__FILE__, __LINE__, fmt, __VA_ARGS__)
//#define DPRINTPOLY(s, poly) dprintpoly(__FILE__, __LINE__, s, poly)
//#define DPRINTSEQ(s, r, bitpos) dprintseq(__FILE__, __LINE__, s, r, bitpos)
//#define DPRINTBASE(i, base) dprintbase(__FILE__, __LINE__, (i), (base))
//#define DPRINTHT(s, ht) dprint_ht(__FILE__, __LINE__, (s), (ht))
#else
#define DPRINT(fmt, ...)   /*  */
//#define DPRINTPOLY(s, poly)  /* */
//#define DPRINTSEQ(s, r, bitpos) /* */
//#define DPRINTBASE(i, base) /* */
//#define DPRINTHT(s, ht) /* */
#endif

#endif
