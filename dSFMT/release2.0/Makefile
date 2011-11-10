# @file  Makefile
# @brief Makefile
#
# @author Mutsuo Saito (Hiroshima University)
# @author Makoto Matsumoto (Hiroshima University)
#
# Copyright (C) 2007, 2008 Mutsuo Saito, Makoto Matsumoto and
# Hiroshima University. All rights reserved.
#
# The new BSD License is applied to this software.
# see LICENSE.txt
#
# @note
# We could comple test-sse2-Mxxx using gcc 3.4.4 of cygwin.
# We could comple test-sse2-Mxxx using gcc 4.0.1 of Linux.
# We coundn't comple test-sse2-Mxxx using gcc 3.3.2 of Linux.
# We could comple test-alti-Mxxx using gcc 3.3 of osx.
# We could comple test-alti-Mxxx using gcc 4.0 of osx.

WARN = -Wmissing-prototypes -Wall #-Winline 
#WARN = -Wmissing-prototypes -Wall -W
OPTI = -O3 -finline-functions -fomit-frame-pointer -DNDEBUG \
-fno-strict-aliasing --param max-inline-insns-single=1800 
#--param inline-unit-growth=500 --param large-function-growth=900 #for gcc 4
#STD =
#STD = -std=c89 -pedantic
#STD = -std=c99 -pedantic
STD = -std=c99
CC = gcc
CCFLAGS = $(OPTI) $(WARN) $(STD)
ALTIFLAGS = -mabi=altivec -maltivec -DHAVE_ALTIVEC
OSXALTIFLAGS = -faltivec -maltivec -DHAVE_ALTIVEC
SSE2FLAGS = -msse2 -DHAVE_SSE2
STD_TARGET = test-std-M19937
ALL_STD_TARGET = test-std-M521 test-std-M1279 test-std-M2203 test-std-M4253 \
test-std-M11213 test-std-M19937 test-std-M44497 test-std-M86243 \
test-std-M132049 test-std-M216091
ALTI_TARGET = $(STD_TARGET) test-alti-M19937
ALL_ALTI_TARGET = test-alti-M521 test-alti-M1279 test-alti-M2203 \
test-alti-M4253 test-alti-M11213 test-alti-M19937 test-alti-M44497 \
test-alti-M86243 test-alti-M132049 test-alti-M216091
SSE2_TARGET = $(STD_TARGET) test-sse2-M19937
ALL_SSE2_TARGET = test-sse2-M521 test-sse2-M1279 test-sse2-M2203 \
test-sse2-M4253 test-sse2-M11213 test-sse2-M19937 test-sse2-M44497 \
test-sse2-M86243 test-sse2-M132049 test-sse2-M216091
# ==========================================================
# comment out or EDIT following lines to get max performance
# ==========================================================
# --------------------
# for gcc 4
# --------------------
#CCFLAGS += --param inline-unit-growth=500 \
#--param large-function-growth=900
# --------------------
# for icl
# --------------------
#CC = icl /Wcheck /O3 /QxB /Qprefetch
# --------------------
# for icc
# --------------------
#CC = icc 
#OPTI = -O3 -finline-functions -fomit-frame-pointer -DNDEBUG \
#-fno-strict-aliasing
#CCFLAGS = $(OPTI) $(WARN) $(STD)
# -----------------
# for PowerPC
# -----------------
#CCFLAGS += -arch ppc
# -----------------
# for Pentium M
# -----------------
#CCFLAGS += -march=prescott
# -----------------
# for Athlon 64
# -----------------
#CCFLAGS += -march=athlon64

.PHONY: std-check sse2-check alti-check

std: ${STD_TARGET}

sse2: ${SSE2_TARGET}

alti: ${ALTI_TARGET}

osx-alti:
	${MAKE} "ALTIFLAGS=${OSXALTIFLAGS}" alti

std-check: ${ALL_STD_TARGET}
	./check.sh test-std

sse2-check: ${ALL_SSE2_TARGET}
	./check.sh test-sse2

alti-check: ${ALL_ALTI_TARGET}
	./check.sh test-alti

osx-alti-check: 
	make "ALTIFLAGS=${OSXALTIFLAGS}" alti-check

test-std-M521: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DDSFMT_MEXP=521 -o $@ dSFMT.c test.c

test-alti-M521: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${ALTIFLAGS} -DDSFMT_MEXP=521 -o $@ dSFMT.c test.c

test-sse2-M521: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${SSE2FLAGS} -DDSFMT_MEXP=521 -o $@ dSFMT.c test.c

test-std-M1279: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DDSFMT_MEXP=1279 -o $@ dSFMT.c test.c

test-alti-M1279: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${ALTIFLAGS} -DDSFMT_MEXP=1279 -o $@ dSFMT.c test.c

test-sse2-M1279: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${SSE2FLAGS} -DDSFMT_MEXP=1279 -o $@ dSFMT.c test.c

test-std-M2203: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DDSFMT_MEXP=2203 -o $@ dSFMT.c test.c

test-alti-M2203: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${ALTIFLAGS} -DDSFMT_MEXP=2203 -o $@ dSFMT.c test.c

test-sse2-M2203: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${SSE2FLAGS} -DDSFMT_MEXP=2203 -o $@ dSFMT.c test.c

test-std-M4253: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DDSFMT_MEXP=4253 -o $@ dSFMT.c test.c

test-alti-M4253: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${ALTIFLAGS} -DDSFMT_MEXP=4253 -o $@ dSFMT.c test.c

test-sse2-M4253: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${SSE2FLAGS} -DDSFMT_MEXP=4253 -o $@ dSFMT.c test.c

test-std-M11213: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DDSFMT_MEXP=11213 -o $@ dSFMT.c test.c

test-alti-M11213: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${ALTIFLAGS} -DDSFMT_MEXP=11213 -o $@ dSFMT.c test.c

test-sse2-M11213: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${SSE2FLAGS} -DDSFMT_MEXP=11213 -o $@ dSFMT.c test.c

test-std-M19937: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DDSFMT_MEXP=19937 -o $@ dSFMT.c test.c

test-alti-M19937: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${ALTIFLAGS} -DDSFMT_MEXP=19937 -o $@ dSFMT.c test.c

test-sse2-M19937: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${SSE2FLAGS} -DDSFMT_MEXP=19937 -o $@ dSFMT.c test.c

test-std-M44497: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DDSFMT_MEXP=44497 -o $@ dSFMT.c test.c

test-alti-M44497: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${ALTIFLAGS} -DDSFMT_MEXP=44497 -o $@ dSFMT.c test.c

test-sse2-M44497: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${SSE2FLAGS} -DDSFMT_MEXP=44497 -o $@ dSFMT.c test.c

test-std-M86243: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DDSFMT_MEXP=86243 -o $@ dSFMT.c test.c

test-alti-M86243: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${ALTIFLAGS} -DDSFMT_MEXP=86243 -o $@ dSFMT.c test.c

test-sse2-M86243: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${SSE2FLAGS} -DDSFMT_MEXP=86243 -o $@ dSFMT.c test.c

test-std-M132049: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DDSFMT_MEXP=132049 -o $@ dSFMT.c test.c

test-alti-M132049: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${ALTIFLAGS} -DDSFMT_MEXP=132049 -o $@ dSFMT.c test.c

test-sse2-M132049: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${SSE2FLAGS} -DDSFMT_MEXP=132049 -o $@ dSFMT.c test.c

test-std-M216091: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DDSFMT_MEXP=216091 -o $@ dSFMT.c test.c

test-alti-M216091: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${ALTIFLAGS} -DDSFMT_MEXP=216091 -o $@ dSFMT.c test.c

test-sse2-M216091: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} ${SSE2FLAGS} -DDSFMT_MEXP=216091 -o $@ dSFMT.c test.c

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -f *.o *~ test-*-M*
