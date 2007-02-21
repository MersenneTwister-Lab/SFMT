#ifndef __C99_H__
#define __C99_H__

#if ! defined(__ppc__)
  #include <emmintrin.h>
#endif

#if defined(__GNUC__)

#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#define INLINE inline

#else /* not GNU C */

typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
#define UINT64_MAX ULLONG_MAX
#define PRIu32 "u"
#define PRIx32 "x"
#define PRIu64 "I64u"
#define PRIx64 "I64x"
typedef unsigned int bool;
#define true 1
#define false 0
#define INLINE

#endif /* end of GNU C */

#endif
