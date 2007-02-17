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

SYSTEM = std_unix
#WARN = -Wmissing-prototypes -Wall -Winline
WARN = -Wmissing-prototypes -Wall
OPTI = -O9 -finline-functions -fomit-frame-pointer -DNDEBUG -fno-strict-aliasing
#STD =
#STD = -std=c89 -pedantic
#STD = -std=c99 -pedantic
STD = -std=c99
GCC = gcc $(OPTI) $(WARN) $(STD)
STD_TARGET = test-std32-M19937 test-std64-M19937
STD_CHECK_TARGET = check-std
BIG_TARGET = $(STD_TARGET) test-big64-M19937
BIG_CHECK_TARGET = $(STD_CHECK_TARGET) check-big
ALTI_TARGET = $(BIG_TARGET) test-alti32-M19937 test-alti64-M19937
ALTI_CHECK_TARGET = $(BIG_CHECK_TARGET) check-alti
SSE_TARGET = $(STD_TARGET) test-sse32-M19937 test-sse64-M19937
SSE_CHECK_TARGET = $(STD_CHECK_TARGET) check-sse
# ======================================
# comment out or EDIT following settings
# ======================================
# --------------------
# for UNIX like system
# --------------------
ifeq ($(SYSTEM), std_unix)
CC = $(GCC)
TARGET = $(STD_TARGET)
CHECK_TARGET = $(STD_CHECK_TARGET)
endif
# -----------------
# for PowerPC
# -----------------
ifeq ($(SYSTEM), ppc_osx)
CC = $(GCC) -faltivec -maltivec -arch ppc
TARGET = $(ALTI_TARGET)
CHECK_TARGET = $(ALTI_CHECK_TARGET)
endif
# -----------------
# for other sse2 CPU
# -----------------
ifeq ($(SYSTEM), sse2_unix)
CC = $(GCC) -msse2
TARGET = $(SSE_TARGET)
CHECK_TARGET = $(SSE_CHECK_TARGET)
endif
# -----------------
# for Pentium M
# -----------------
#CC = $(GCC) -march=prescott -msse2
#CC = icl /Wcheck /O3 /QxB /Qprefetch
#TARGET = $(SSE_TARGET)
#CHECK_TARGET = $(SSE_CHECK_TARGET)
# -----------------
# for Athlon
# -----------------
#CC = $(GCC) -march=nocona -msse2
#TARGET = $(SSE_TARGET)
#CHECK_TARGET = $(SSE_CHECK_TARGET)

all:$(TARGET)

.PHONY: check
check:${CHECK_TARGET}

test-ref32-M607: SFMTp-ref.c SFMTp.h
	${CC} -DMEXP=607 -DMAIN -o $@ SFMTp-ref.c

test-std32-M607: test32.c SFMTp.c SFMTp.h
	${CC} -DMEXP=607 -o $@ test32.c

test-alti32-M607: test32.c SFMTp-alti32.c SFMTp.h
	${CC} -DMEXP=607 -DALTIVEC -o $@ test32.c

test-std64-M607: test64.c SFMTp.c SFMTp.h
	${CC} -DMEXP=607 -o $@ test64.c

test-big64-M607: test64.c SFMTp-big64.c sFMTp.h
	${CC} -DMEXP=607 -DBIG -o $@ test64.c

test-alti64-M607: test64.c SFMTp-alti64.c SFMTp.h
	${CC} -DMEXP=607 -DALTIVEC -o $@ test64.c

test-sse32-M607: test32.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=607 -DSSE2 -o $@ test32.c

test-sse64-M607: test64.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=607 -DSSE2 -o $@ test64.c

test-ref32-M2281: SFMTp-ref.c SFMTp.h
	${CC} -DMEXP=2281 -DMAIN -o $@ SFMTp-ref.c

test-std32-M2281: test32.c SFMTp.c SFMTp.h
	${CC} -DMEXP=2281 -o $@ test32.c

test-alti32-M2281: test32.c SFMTp-alti32.c SFMTp.h
	${CC} -DMEXP=2281 -DALTIVEC -o $@ test32.c

test-std64-M2281: test64.c SFMTp.c SFMTp.h
	${CC} -DMEXP=2281 -o $@ test64.c

test-big64-M2281: test64.c SFMTp-big64.c sFMTp.h
	${CC} -DMEXP=2281 -DBIG -o $@ test64.c

test-alti64-M2281: test64.c SFMTp-alti64.c SFMTp.h
	${CC} -DMEXP=2281 -DALTIVEC -o $@ test64.c

test-sse32-M2281: test32.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=2281 -DSSE2 -o $@ test32.c

test-sse64-M2281: test64.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=2281 -DSSE2 -o $@ test64.c

test-ref32-M4253: SFMTp-ref.c SFMTp.h
	${CC} -DMEXP=4253 -DMAIN -o $@ SFMTp-ref.c

test-std32-M4253: test32.c SFMTp.c SFMTp.h
	${CC} -DMEXP=4253 -o $@ test32.c

test-alti32-M4253: test32.c SFMTp-alti32.c SFMTp.h
	${CC} -DMEXP=4253 -DALTIVEC -o $@ test32.c

test-std64-M4253: test64.c SFMTp.c SFMTp.h
	${CC} -DMEXP=4253 -o $@ test64.c

test-big64-M4253: test64.c SFMTp-big64.c sFMTp.h
	${CC} -DMEXP=4253 -DBIG -o $@ test64.c

test-alti64-M4253: test64.c SFMTp-alti64.c SFMTp.h
	${CC} -DMEXP=4253 -DALTIVEC -o $@ test64.c

test-sse32-M4253: test32.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=4253 -DSSE2 -o $@ test32.c

test-sse64-M4253: test64.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=4253 -DSSE2 -o $@ test64.c

test-ref32-M11213: SFMTp-ref.c SFMTp.h
	${CC} -DMEXP=11213 -DMAIN -o $@ SFMTp-ref.c

test-std32-M11213: test32.c SFMTp.c SFMTp.h
	${CC} -DMEXP=11213 -o $@ test32.c

test-alti32-M11213: test32.c SFMTp-alti32.c SFMTp.h
	${CC} -DMEXP=11213 -DALTIVEC -o $@ test32.c

test-std64-M11213: test64.c SFMTp.c SFMTp.h
	${CC} -DMEXP=11213 -o $@ test64.c

test-big64-M11213: test64.c SFMTp-big64.c sFMTp.h
	${CC} -DMEXP=11213 -DBIG -o $@ test64.c

test-alti64-M11213: test64.c SFMTp-alti64.c SFMTp.h
	${CC} -DMEXP=11213 -DALTIVEC -o $@ test64.c

test-sse32-M11213: test32.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=11213 -DSSE2 -o $@ test32.c

test-sse64-M11213: test64.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=11213 -DSSE2 -o $@ test64.c

test-ref32-M19937: SFMTp-ref.c SFMTp.h
	${CC} -DMEXP=19937 -DMAIN -o $@ SFMTp-ref.c

test-std32-M19937: test32.c SFMTp.c SFMTp.h
	${CC} -DMEXP=19937 -o $@ test32.c

test-alti32-M19937: test32.c SFMTp-alti32.c SFMTp.h
	${CC} -DMEXP=19937 -DALTIVEC -o $@ test32.c

test-std64-M19937: test64.c SFMTp.c SFMTp.h
	${CC} -DMEXP=19937 -o $@ test64.c

test-big64-M19937: test64.c SFMTp-big64.c sFMTp.h
	${CC} -DMEXP=19937 -DBIG -o $@ test64.c

test-alti64-M19937: test64.c SFMTp-alti64.c SFMTp.h
	${CC} -DMEXP=19937 -DALTIVEC -o $@ test64.c

test-sse32-M19937: test32.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=19937 -DSSE2 -o $@ test32.c

test-sse64-M19937: test64.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=19937 -DSSE2 -o $@ test64.c

test-ref32-M44497: SFMTp-ref.c SFMTp.h
	${CC} -DMEXP=44497 -DMAIN -o $@ SFMTp-ref.c

test-std32-M44497: test32.c SFMTp.c SFMTp.h
	${CC} -DMEXP=44497 -o $@ test32.c

test-alti32-M44497: test32.c SFMTp-alti32.c SFMTp.h
	${CC} -DMEXP=44497 -DALTIVEC -o $@ test32.c

test-std64-M44497: test64.c SFMTp.c SFMTp.h
	${CC} -DMEXP=44497 -o $@ test64.c

test-big64-M44497: test64.c SFMTp-big64.c sFMTp.h
	${CC} -DMEXP=44497 -DBIG -o $@ test64.c

test-alti64-M44497: test64.c SFMTp-alti64.c SFMTp.h
	${CC} -DMEXP=44497 -DALTIVEC -o $@ test64.c

test-sse32-M44497: test32.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=44497 -DSSE2 -o $@ test32.c

test-sse64-M44497: test64.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=44497 -DSSE2 -o $@ test64.c

test-ref32-M86243: SFMTp-ref.c SFMTp.h
	${CC} -DMEXP=86243 -DMAIN -o $@ SFMTp-ref.c

test-std32-M86243: test32.c SFMTp.c SFMTp.h
	${CC} -DMEXP=86243 -o $@ test32.c

test-alti32-M86243: test32.c SFMTp-alti32.c SFMTp.h
	${CC} -DMEXP=86243 -DALTIVEC -o $@ test32.c

test-std64-M86243: test64.c SFMTp.c SFMTp.h
	${CC} -DMEXP=86243 -o $@ test64.c

test-big64-M86243: test64.c SFMTp-big64.c sFMTp.h
	${CC} -DMEXP=86243 -DBIG -o $@ test64.c

test-alti64-M86243: test64.c SFMTp-alti64.c SFMTp.h
	${CC} -DMEXP=86243 -DALTIVEC -o $@ test64.c

test-sse32-M86243: test32.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=86243 -DSSE2 -o $@ test32.c

test-sse64-M86243: test64.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=86243 -DSSE2 -o $@ test64.c

test-ref32-M132049: SFMTp-ref.c SFMTp.h
	${CC} -DMEXP=132049 -DMAIN -o $@ SFMTp-ref.c

test-std32-M132049: test32.c SFMTp.c SFMTp.h
	${CC} -DMEXP=132049 -o $@ test32.c

test-alti32-M132049: test32.c SFMTp-alti32.c SFMTp.h
	${CC} -DMEXP=132049 -DALTIVEC -o $@ test32.c

test-std64-M132049: test64.c SFMTp.c SFMTp.h
	${CC} -DMEXP=132049 -o $@ test64.c

test-big64-M132049: test64.c SFMTp-big64.c sFMTp.h
	${CC} -DMEXP=132049 -DBIG -o $@ test64.c

test-alti64-M132049: test64.c SFMTp-alti64.c SFMTp.h
	${CC} -DMEXP=132049 -DALTIVEC -o $@ test64.c

test-sse32-M132049: test32.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=132049 -DSSE2 -o $@ test32.c

test-sse64-M132049: test64.c SFMTp-sse2.c SFMTp.h
	${CC} -DMEXP=132049 -DSSE2 -o $@ test64.c

check-std:
	./check.sh 32 test-std
	./check.sh 64 test-std

check-alti:
	./check.sh 32 test-alti
	./check.sh 64 test-alti

check-big:
	./check.sh 64 test-big

check-sse:
	./check.sh 32 test-sse
	./check.sh 64 test-sse

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
