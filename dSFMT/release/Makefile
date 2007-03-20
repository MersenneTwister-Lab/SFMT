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
# We could comple test-sse2-Mxxx using gcc 3.4.4 of cygwin.
# We could comple test-sse2-Mxxx using gcc 4.0.1 of Linux.
# We coundn't comple test-sse2-Mxxx using gcc 3.3.2 of Linux.
# We could comple test-alti-Mxxx using gcc 3.3 of osx.
# We could comple test-alti-Mxxx using gcc 4.0 of osx.

WARN = -Wmissing-prototypes -Wall #-Winline 
#WARN = -Wmissing-prototypes -Wall -W
OPTI = -O9 -finline-functions -fomit-frame-pointer -DNDEBUG \
-fno-strict-aliasing --param max-inline-insns-single=1800 
#--param inline-unit-growth=500 --param large-function-growth=900 #for gcc 4
#STD =
#STD = -std=c89 -pedantic
#STD = -std=c99 -pedantic
STD = -std=c99
CC = gcc
CCFLAGS = $(OPTI) $(WARN) $(STD)
STD_TARGET = test-std-M19937
ALL_STD_TARGET = test-std-M607 test-std-M1279 test-std-M2281 test-std-M4423 \
test-std-M11213 test-std-M19937 test-std-M44497 test-std-M86243
ALTI_TARGET = $(STD_TARGET) test-alti-M19937
ALL_ALTI_TARGET = test-alti-M607 test-alti-M1279 test-alti-M2281 \
test-alti-M4423 test-alti-M11213 test-alti-M19937 test-alti-M44497 \
test-alti-M86243
SSE2_TARGET = $(STD_TARGET) test-sse2-M19937
ALL_SSE2_TARGET = test-sse2-M607 test-sse2-M1279 test-sse2-M2281 \
test-sse2-M4423 test-sse2-M11213 test-sse2-M19937 test-sse2-M44497 \
test-sse2-M86243
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
	${CC} ${CCFLAGS} -faltivec -maltivec -DMEXP=607 -DHAVE_ALTIVEC \
	-o $@ test.c

test-sse2-M607: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=607 -DHAVE_SSE2 -o $@ test.c

test-std-M1279: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=1279 -o $@ test.c

test-alti-M1279: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -faltivec -maltivec -DMEXP=1279 -DHAVE_ALTIVEC \
	-o $@ test.c

test-sse2-M1279: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=1279 -DHAVE_SSE2 -o $@ test.c

test-std-M2281: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=2281 -o $@ test.c

test-alti-M2281: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -faltivec -maltivec -DMEXP=2281 -DHAVE_ALTIVEC \
	-o $@ test.c

test-sse2-M2281: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=2281 -DHAVE_SSE2 -o $@ test.c

test-std-M4423: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=4423 -o $@ test.c

test-alti-M4423: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -faltivec -maltivec -DMEXP=4423 -DHAVE_ALTIVEC \
	-o $@ test.c

test-sse2-M4423: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=4423 -DHAVE_SSE2 -o $@ test.c

test-std-M11213: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=11213 -o $@ test.c

test-alti-M11213: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -faltivec -maltivec -DMEXP=11213 -DHAVE_ALTIVEC \
	-o $@ test.c

test-sse2-M11213: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=11213 -DHAVE_SSE2 -o $@ test.c

test-std-M19937: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=19937 -o $@ test.c

test-alti-M19937: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -faltivec -maltivec -DMEXP=19937 -DHAVE_ALTIVEC \
	-o $@ test.c

test-sse2-M19937: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=19937 -DHAVE_SSE2 -o $@ test.c

test-std-M44497: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=44497 -o $@ test.c

test-alti-M44497: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -faltivec -maltivec -DMEXP=44497 -DHAVE_ALTIVEC \
	-o $@ test.c

test-sse2-M44497: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=44497 -DHAVE_SSE2 -o $@ test.c

test-std-M86243: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=86243 -o $@ test.c

test-alti-M86243: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -faltivec -maltivec -DMEXP=86243 -DHAVE_ALTIVEC \
	-o $@ test.c

test-sse2-M86243: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=86243 -DHAVE_SSE2 -o $@ test.c

test-std-M132049: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -DMEXP=132049 -o $@ test.c

test-alti-M132049: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -faltivec -maltivec -DMEXP=132049 -DHAVE_ALTIVEC \
	-o $@ test.c

test-sse2-M132049: test.c dSFMT.c dSFMT.h
	${CC} ${CCFLAGS} -msse2 -DMEXP=132049 -DHAVE_SSE2 -o $@ test.c

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -f *.o *~
