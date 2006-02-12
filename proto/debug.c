/* This file includes debugging utility */
/* only available for GCC */

#include "debug.h"

void dprint(char *file, int line, char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "%s:%d ", file, line);
  vfprintf(stderr, fmt, ap);
  fputc('\n', stderr);
  va_end(ap);
}
