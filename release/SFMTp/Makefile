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

# -----------------
# start GNU make
# -----------------
%607:MEXP=607
%2281:MEXP=2281
%4253:MEXP=4253
%11213:MEXP=11213
%19937:MEXP=19937
%44497:MEXP=44497
%86243:MEXP=86243
%132049:MEXP=132049
#%216091:MEXP=216091
# -----------------
# end GNU make
# -----------------

all:$(TARGET)

test-ref32: SFMTp-ref.c SFMTp.h
	${CC} -DMEXP=19937 -DMAIN -o $@ SFMTp-ref.c

test-std32: test32.c SFMTp.c SFMTp.h
	${CC} -DMEXP=19937 -o $@ test32.c

test-alti32: test32.c SFMTp-alti32.c SFMTp.h
	${CC} -DMEXP=19937 -DALTIVEC -o $@ test32.c

test-std64: test64.c SFMTp.c SFMTp.h
	${CC} -DMEXP=19937 -o $@ test64.c

test-big64: test64.c SFMTp-big64.c sFMTp.h
	${CC} -DMEXP=19937 -DBIG -o $@ test64.c

test-alti64: test64.c SFMTp-alti64.c SFMTp.h
	${CC} -DMEXP=19937 -DALTIVEC -o $@ test64.c

test-sse32: test32.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=19937 -o $@ test32.c

test-sse64: test64.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=19937 -o $@ test64.c

# -----------------
# start GNU make
# -----------------
test-ref32-M%: SFMTp-ref.c SFMTp.h
	${CC} -DMEXP=${MEXP} -DMAIN -o $@ SFMTp-ref.c

test-std32-M%: test32.c SFMTp.c SFMTp.h
	${CC} -DMEXP=${MEXP} -o $@ test32.c

test-alti32-M%: test32.c SFMTp-alti32.c SFMTp.h
	${CC} -DMEXP=${MEXP} -DALTIVEC -o $@ test32.c

test-std64-M%: test64.c SFMTp.c SFMTp.h
	${CC} -DMEXP=${MEXP} -o $@ test64.c

test-big64-M%: test64.c SFMTp-big64.c sFMTp.h
	${CC} -DMEXP=${MEXP} -DBIG -o $@ test64.c

test-alti64-M%: test64.c SFMTp-alti64.c SFMTp.h
	${CC} -DMEXP=${MEXP} -DALTIVEC -o $@ test64.c

test-sse32-M%: test32.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=${MEXP} -o $@ test32.c

test-sse64-M%: test64.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=${MEXP} -o $@ test64.c
# -----------------
# end GNU make
# -----------------

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
