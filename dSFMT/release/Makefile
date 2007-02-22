# @file  Makefile
# @brief Makefile
#
# @author Mutsuo Saito (Hiroshima University)
# @author Makoto Matsumoto (Hiroshima University)
#
# Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
# University. All rights reserved.
#
# The new BSD License is applied to this software.
# see LICENSE.txt
#
# @note
# We could comple dSFMT-sse2.c using gcc 3.4.4 of cygwin.
# We could comple dSFMT-sse2.c using gcc 4.0.1 of Linux.
# We coundn't comple dSFMT-sse2.c using gcc 3.3.2 of Linux.
# We could comple dSFMT-alti32.c and dSFMT-alti64.c 
# using gcc 3.3 of osx.
# We could comple dSFMT-alti32.c and dSFMT-alti64.c 
# using gcc 4.0 of osx.

#WARN = -Wmissing-prototypes -Wall -Winline
WARN = -Wmissing-prototypes -Wall
OPTI = -O9 -finline-functions -fomit-frame-pointer -DNDEBUG -fno-strict-aliasing
#STD =
#STD = -std=c89 -pedantic
#STD = -std=c99 -pedantic
STD = -std=c99
CC = gcc
CCFLAGS = $(OPTI) $(WARN) $(STD)
STD_TARGET = test-std-M607
STD_CHECK_TARGET = check-std
ALTI_TARGET = $(STD_TARGET) test-alti-M607
ALTI_CHECK_TARGET = $(STD_CHECK_TARGET) check-alti
SSE_TARGET = $(STD_TARGET) test-sse2-M607
SSE_CHECK_TARGET = $(STD_CHECK_TARGET) check-sse2
# ======================================
# comment out or EDIT following settings
# ======================================
# --------------------
# for UNIX like system
# --------------------
#TARGET = $(STD_TARGET)
#CHECK_TARGET = $(STD_CHECK_TARGET)
# -----------------
# for PowerPC
# -----------------
#CCFLAGS = $(CCFLAGS) -faltivec -maltivec -arch ppc
#TARGET = $(ALTI_TARGET)
#CHECK_TARGET = $(ALTI_CHECK_TARGET)
# -----------------
# for Pentium M
# -----------------
#CCFLAGS = $(CCFLAGS) -march=prescott -msse2
#CC = icl /Wcheck /O3 /QxB /Qprefetch
#TARGET = $(SSE_TARGET)
#CHECK_TARGET = $(SSE_CHECK_TARGET)
# -----------------
# for Athlon
# -----------------
#CCFLAGS = $(CCFLAGS) -march=nocona -msse2
#TARGET = $(SSE_TARGET)
#CHECK_TARGET = $(SSE_CHECK_TARGET)
# -----------------
# for other sse2 CPU
# -----------------
#CCFLAGS = $(CCFLAGS) -msse2
#TARGET = $(SSE_TARGET)
#CHECK_TARGET = $(SSE_CHECK_TARGET)

.PHONY: std-check sse2-check alti-check

std: ${STD_TARGET}

sse2:
	${MAKE} CCFLAGS="${CCFLAGS} -msse2" ${SSE_TARGET}

alti:
	${MAKE} CCFLAGS="${CCFLAGS} -faltivec" ${ALTI_TARGET}

std-check:
	./check.sh test-std

sse2-check:
	CCFLAGS="${CCFLAGS} -msse2"
	./check.sh test-sse

alti-check:
	CCFLAGS="${CCFLAGS} -faltivec"
	./check.sh test-alti

test-ref32-M607: dSFMT-ref.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=607 -DMAIN -o $@ dSFMT-ref.c

test-std-M607: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=607 -o $@ test.c

test-alti-M607: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=607 -DALTIVEC -o $@ test.c

test-sse2-M607: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=607 -DSSE2 -o $@ test.c

test-ref-M2281: dSFMT-ref.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=2281 -DMAIN -o $@ dSFMT-ref.c

test-std-M2281: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=2281 -o $@ test.c

test-alti-M2281: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=2281 -DALTIVEC -o $@ test.c

test-sse2-M2281: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=2281 -DSSE2 -o $@ test.c

test-ref-M4253: dSFMT-ref.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=4253 -DMAIN -o $@ dSFMT-ref.c

test-std-M4253: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=4253 -o $@ test.c

test-alti-M4253: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=4253 -DALTIVEC -o $@ test.c

test-sse2-M4253: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=4253 -DSSE2 -o $@ test.c

test-ref-M11213: dSFMT-ref.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=11213 -DMAIN -o $@ dSFMT-ref.c

test-std-M11213: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=11213 -o $@ test.c

test-alti-M11213: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=11213 -DALTIVEC -o $@ test.c

test-sse2-M11213: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=11213 -DSSE2 -o $@ test.c

test-ref-M19937: dSFMT-ref.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=19937 -DMAIN -o $@ dSFMT-ref.c

test-std-M19937: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=19937 -o $@ test.c

test-alti-M19937: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=19937 -DALTIVEC -o $@ test.c

test-sse2-M19937: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=19937 -DSSE2 -o $@ test.c

test-ref-M44497: dSFMT-ref.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=44497 -DMAIN -o $@ dSFMT-ref.c

test-std-M44497: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=44497 -o $@ test.c

test-alti-M44497: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=44497 -DALTIVEC -o $@ test.c

test-sse2-M44497: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=44497 -DSSE2 -o $@ test.c

test-ref-M86243: dSFMT-ref.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=86243 -DMAIN -o $@ dSFMT-ref.c

test-std-M86243: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=86243 -o $@ test.c

test-alti-M86243: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=86243 -DALTIVEC -o $@ test.c

test-sse2-M86243: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=86243 -DSSE2 -o $@ test.c

test-ref-M132049: dSFMT-ref.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=132049 -DMAIN -o $@ dSFMT-ref.c

test-std-M132049: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=132049 -o $@ test.c

test-alti-M132049: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=132049 -DALTIVEC -o $@ test.c

test-sse2-M132049: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=132049 -DSSE2 -o $@ test.c

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -f *.o *~
