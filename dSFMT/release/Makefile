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
WARN = -Wmissing-prototypes -Wall -W
OPTI = -O9 -finline-functions -fomit-frame-pointer -DNDEBUG -fno-strict-aliasing
#STD =
#STD = -std=c89 -pedantic
#STD = -std=c99 -pedantic
STD = -std=c99
CC = gcc
CCFLAGS = $(OPTI) $(WARN) $(STD)
STD_TARGET = test-std-M19937
ALL_STD_TARGET = test-std-M607 test-std-M2281 test-std-M4423 test-std-M11213 \
test-std-M19937 test-std-M44497 test-std-M86243 test-std-M132049
ALTI_TARGET = $(STD_TARGET) test-alti-M19937
ALL_ALTI_TARGET = test-alti-M607 test-alti-M2281 test-alti-M4423 \
test-alti-M11213 test-alti-M19937 test-alti-M44497 test-alti-M86243 \
test-alti-M132049
SSE2_TARGET = $(STD_TARGET) test-sse2-M19937
ALL_SSE2_TARGET = test-sse2-M607 test-sse2-M2281 test-sse2-M4423 \
test-sse2-M11213 test-sse2-M19937 test-sse2-M44497 test-sse2-M86243 \
test-sse2-M132049
# ======================================
# comment out or EDIT following settings
# ======================================
# --------------------
# for UNIX like system
# --------------------
#TARGET = $(STD_TARGET)
# -----------------
# for PowerPC
# -----------------
#CCFLAGS = $(CCFLAGS) -faltivec -maltivec -arch ppc
#TARGET = $(ALTI_TARGET)
# -----------------
# for Pentium M
# -----------------
#CCFLAGS = $(CCFLAGS) -march=prescott -msse2
#CC = icl /Wcheck /O3 /QxB /Qprefetch
#TARGET = $(SSE_TARGET)
# -----------------
# for Athlon
# -----------------
#CCFLAGS = $(CCFLAGS) -march=nocona -msse2
#TARGET = $(SSE_TARGET)
# -----------------
# for other sse2 CPU
# -----------------
#CCFLAGS = $(CCFLAGS) -msse2
#TARGET = $(SSE_TARGET)

.PHONY: std-check sse2-check alti-check

std: ${STD_TARGET}

sse2:
	${MAKE} CCFLAGS="${CCFLAGS} -msse2" ${SSE2_TARGET}

alti:
	${MAKE} CCFLAGS="${CCFLAGS} -faltivec" ${ALTI_TARGET}

std-check: ${ALL_STD_TARGET}
	./check.sh test-std

sse2-check: ${ALL_SSE2_TARGET}
	./check.sh test-sse2

alti-check: ${ALL_ALTI_TARGET}
	./check.sh test-alti

test-std-M607: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=607 -o $@ test.c

test-alti-M607: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -faltivec -DMEXP=607 -DALTIVEC -o $@ test.c

test-sse2-M607: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=607 -DSSE2 -o $@ test.c

test-std-M2281: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=2281 -o $@ test.c

test-alti-M2281: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -faltivec -DMEXP=2281 -DALTIVEC -o $@ test.c

test-sse2-M2281: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=2281 -DSSE2 -o $@ test.c

test-std-M4423: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=4423 -o $@ test.c

test-alti-M4423: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -faltivec -DMEXP=4423 -DALTIVEC -o $@ test.c

test-sse2-M4423: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=4423 -DSSE2 -o $@ test.c

test-std-M11213: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=11213 -o $@ test.c

test-alti-M11213: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -faltivec -DMEXP=11213 -DALTIVEC -o $@ test.c

test-sse2-M11213: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=11213 -DSSE2 -o $@ test.c

test-std-M19937: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=19937 -o $@ test.c

test-alti-M19937: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -faltivec -DMEXP=19937 -DALTIVEC -o $@ test.c

test-sse2-M19937: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=19937 -DSSE2 -o $@ test.c

test-std-M44497: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=44497 -o $@ test.c

test-alti-M44497: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -faltivec -DMEXP=44497 -DALTIVEC -o $@ test.c

test-sse2-M44497: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=44497 -DSSE2 -o $@ test.c

test-std-M86243: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=86243 -o $@ test.c

test-alti-M86243: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -faltivec -DMEXP=86243 -DALTIVEC -o $@ test.c

test-sse2-M86243: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=86243 -DSSE2 -o $@ test.c

test-std-M132049: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=132049 -o $@ test.c

test-alti-M132049: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -faltivec -DMEXP=132049 -DALTIVEC -o $@ test.c

test-sse2-M132049: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=132049 -DSSE2 -o $@ test.c

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -f *.o *~
