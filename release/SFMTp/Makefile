# @file  Makefile
# @brief Makefile
#
# @author Mutsuo Saito (Hiroshima University)
# @author Makoto Matsumoto (Hiroshima University)
#
# @date 2007-01-11
#
# Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
# University. All rights reserved.
#
# The new BSD License is applied to this software.
# see LICENSE.txt
#
# @note
# We could comple sfmt19937-sse2.c using gcc 3.4.4 of cygwin.
# We could comple sfmt19937-sse2.c using gcc 4.0.1 of Linux.
# We coundn't comple sfmt19937-sse2.c using gcc 3.3.2 of Linux.
# We could comple sfmt19937-alti32.c and sfmt19937-alti64.c 
# using gcc 3.3 of osx.
# We could comple sfmt19937-alti32.c and sfmt19937-alti64.c 
# using gcc 4.0 of osx.

#WARN = -Wmissing-prototypes -Wall -Winline
WARN = -Wmissing-prototypes -Wall
OPTI = -O9 -finline-functions -fomit-frame-pointer -DNDEBUG 
#STD =
#STD = -std=c89 -pedantic
STD = -std=c99 -pedantic
GCC = gcc $(OPTI) $(WARN) $(STD)
STD_TARGET = test-std32 test-std64
BIG_TARGET = $(STD_TARGET) test-big64
ALTI_TARGET = $(BIG_TARGET) test-alti32 test-alti64
SSE_TARGET = $(STD_TARGET) test-sse32 test-sse64
# --------------------
# for UNIX like system
# --------------------
#CC = $(GCC)
#TARGET = $(STD_TARGET)
# -----------------
# for PowerPC
# -----------------
CC = $(GCC) -faltivec -maltivec -arch ppc
TARGET = $(ALTI_TARGET)
# -----------------
# for Pentium M
# -----------------
#CC = $(GCC) -march=prescott -msse2
#CC = icl /Wcheck /O3 /QxB /Qprefetch
#TARGET = $(SSE_TARGET)
# -----------------
# for Athlon
# -----------------
#CC = $(GCC) -march=nocona -msse2
#TARGET = $(SSE_TARGET)
# -----------------
# for other sse2 CPU
# -----------------
#CC = $(GCC) -msse2
#TARGET = $(SSE_TARGET)

all:$(TARGET)

test-ref32: SFMTp-ref.c SFMTp.h
	${CC} -DMAIN -o $@ SFMTp-ref.c

test-std32: test32.c SFMTp.c SFMTp.h
	${CC} -o $@ test32.c

test-std64: test64.c sfmt19937.c sfmt19937.h Makefile
	${CC} -o $@ test64.c

test-big64: test_big64.c sfmt19937-big64.c sfmt19937.h Makefile
	${CC} -o $@ test_big64.c

test-alti32: test_alti32.c sfmt19937-alti32.c sfmt19937.h Makefile
	${CC} -o $@ test_alti32.c

test-alti64: test_alti64.c sfmt19937-alti64.c sfmt19937.h Makefile
	${CC} -o $@ test_alti64.c

test-sse32: test_sse32.c sfmt19937-sse2.c sfmt19937.h Makefile
	${CC} -o $@ test_sse32.c

test-sse64: test_sse64.c sfmt19937-sse2.c sfmt19937.h Makefile
	${CC} -o $@ test_sse64.c

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
