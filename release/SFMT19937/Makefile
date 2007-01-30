# @file  Makefile
# @brief Makefile
#
# @author Mutsuo Saito (Hiroshima University)
# @author Makoto Matsumoto (Hiroshima University)
#
# Copyright (C) 2006,2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
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

WARN = -Wmissing-prototypes -Wall
OPTI = -O9 -finline-functions -fomit-frame-pointer -fno-strict-aliasing \
	-DNDEBUG 
#STD =
#STD = -std=c89 -pedantic
#STD = -std=c99 -pedantic
STD = -std=c99
CC = gcc
CCFLAGS = $(OPTI) $(WARN) $(STD)
STD_TARGET = test-std32 test-std64
STD_CHECK_TARGET = check-std32 check-std64
BIG_TARGET = $(STD_TARGET) test-big64
BIG_CHECK_TARGET = $(STD_CHECK_TARGET) check-big64
ALTI_TARGET = $(BIG_TARGET) test-alti32 test-alti64
ALTI_CHECK_TARGET = $(BIG_CHECK_TARGET) check-alti32 check-alti64
SSE_TARGET = $(STD_TARGET) test-sse32 test-sse64
SSE_CHECK_TARGET = $(STD_CHECK_TARGET) check-sse32 check-sse64
CHECK32_FILE = sfmt19937-32.out.txt
CHECK64_FILE = sfmt19937-64.out.txt
# ======================================
# SYSTEM dependent settings
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
# for other sse2 CPU
# -----------------
#CCFLAGS = $(CCFLAGS) -msse2
#TARGET = $(SSE_TARGET)
#CHECK_TARGET = $(SSE_CHECK_TARGET)
# -----------------
# for Pentium M
# -----------------
#CCFLAGS = $(CCFLAGS) -march=prescott -msse2
#CC = icl /Wcheck /O3 /QxB 
#TARGET = $(SSE_TARGET)
#CHECK_TARGET = $(SSE_CHECK_TARGET)
# -----------------
# for Athlon
# -----------------
#CCFLAGS = $(CCFLAGS) -march=nocona -msse2
#TARGET = $(SSE_TARGET)
#CHECK_TARGET = $(SSE_CHECK_TARGET)

.PHONY: std-check sse2-check alti-check

std: ${STD_TARGET}

sse2:
	${MAKE} CCFLAGS="${CCFLAGS} -msse2" ${SSE_TARGET}

alti:
	${MAKE} CCFLAGS="${CCFLAGS} -faltivec" ${ALTI_TARGET}

std-check:${STD_CHECK_TARGET}

sse2-check:
	${MAKE} CCFLAGS="${CCFLAGS} -msse2" ${SSE_CHECK_TARGET}

alti-check:
	${MAKE} CCFLAGS="${CCFLAGS} -faltivec" ${ALTI_CHECK_TARGET}

test-std32: test32.c sfmt19937.c sfmt19937.h
	${CC} ${CCFLAGS} -o $@ test32.c

test-alti32: test32.c sfmt19937-alti32.c sfmt19937.h
	${CC} ${CCFLAGS} -DALTIVEC=1 -o $@ test32.c

test-sse32: test32.c sfmt19937-sse2.c sfmt19937.h
	${CC} ${CCFLAGS} -DSSE2=1 -o $@ test32.c

check-std32: test-std32
	./check.sh test-std32 ${CHECK32_FILE}

check-alti32: test-alti32
	./check.sh test-alti32 ${CHECK32_FILE}

check-sse32: test-sse32
	./check.sh test-sse32 ${CHECK32_FILE}

test-std64: test64.c sfmt19937.c sfmt19937.h
	${CC} ${CCFLAGS} -o $@ test64.c

test-big64: test64.c sfmt19937-big64.c sfmt19937.h
	${CC} ${CCFLAGS} -DBIG=1 -o $@ test64.c

test-alti64: test64.c sfmt19937-alti64.c sfmt19937.h
	${CC} ${CCFLAGS} -DALTIVEC=1 -o $@ test64.c

test-sse64: test64.c sfmt19937-sse2.c sfmt19937.h
	${CC} ${CCFLAGS} -DSSE2=1 -o $@ test64.c

check-std64: test-std64
	./check.sh test-std64 ${CHECK64_FILE}

check-big64: test-big64
	./check.sh test-big64 ${CHECK64_FILE}

check-alti64: test-alti64
	./check.sh test-alti64 ${CHECK64_FILE}

check-sse64: test-sse64
	./check.sh test-sse64 ${CHECK64_FILE}

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -f *.o *~
