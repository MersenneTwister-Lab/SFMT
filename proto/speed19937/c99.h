#ifndef __C99_H__
#define __C99_H__

#if ! defined(__ppc__)
  #include <emmintrin.h>
#endif

#ifdef __GNUC__

#include <stdint.h>
#include <stdbool.h>
#define INLINE inline
#else /* not GNU C */

typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
#define UINT64_MAX ULLONG_MAX
typedef unsigned int bool;
#define true 1
#define false 0
#define INLINE

#endif /* end of GNU C */

#endif
