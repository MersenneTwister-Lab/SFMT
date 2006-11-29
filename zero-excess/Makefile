# @file  Makefile
# @brief Makefile
#
# @author Mutsuo Saito (Hiroshima University)
# @author Makoto Matsumoto (Hiroshima University)
#
# @date 2006-09-06
#
# Copyright (C) 2006 Mutsuo Saito, Makoto Matsumoto and Hiroshima
# University. All rights reserved.
#
# The new BSD License is applied to this software.
# see LICENSE.txt

WARN = -Wmissing-prototypes -Wall -Winline
OPTI = -O9 -finline-functions -fomit-frame-pointer
#STD = -std=c89 -pedantic
#STD = -std=c99 -pedantic
GCC = gcc $(OPTI) $(WARN) $(STD)
CC = $(GCC)
CPP = g++ $(OPTI) -Wall
LINKOPT = -lntl -lgmp

recover.eps: sfmtX78.count.txt sfmtP37.count.txt pmt32.count.txt \
	well19937c.count.txt
	gnuplot < count.plt

count1bit: count1bit.c
	${CC} -o $@ count1bit.c

countMT: countMT.o mt19937ar.o
	${CC} -o $@ countMT.o mt19937ar.o

countWELL: countWELL.o well19937a.o
	${CC} -o $@ countWELL.o well19937a.o

countWELL1024: countWELL1024.o well1024a.o
	${CC} -o $@ countWELL1024.o well1024a.o

countPMT19937: countPMT19937.o pmt32.c
	${CC} -DMEXP=19937 -c pmt32.c 
	${CC} -o $@ countPMT19937.o pmt32.o

countPMT19937L: countPMT19937L.o pmt32L.c
	${CC} -DMEXP=19937 -c pmt32L.c 
	${CC} -o $@ countPMT19937L.o pmt32L.o

countPMT1279: countPMT1279.o pmt32.c
	${CC} -DMEXP=1279 -c pmt32.c 
	${CC} -o $@ countPMT1279.o pmt32.o

countPMT1279L: countPMT1279L.o pmt32L.c
	${CC} -DMEXP=1279 -c pmt32L.c 
	${CC} -o $@ countPMT1279L.o pmt32L.o

countHT800: countHT800.o ht800.o
	${CC} -o $@ countHT800.o ht800.o

countXS800: countXS800.o xor3.o
	${CC} -o $@ countXS800.o xor3.o

countTT800: countTT800.o tt800.o
	${CC} -o $@ countTT800.o tt800.o

countX78: countX78.o sfmt19937.o
	${CC} -DMEXP=19937 -o $@ countX78.o sfmt19937.o

countP37: countP37.o sfmtP37.o
	${CC} -DMEXP=19937 -o $@ countP37.o sfmtP37.o

bmX78: bmX78.o sfmt19937.o
	${CPP} -o $@ bmX78.o sfmt19937.o ${LINKOPT}

dumpMT: dumpMT.o mt19937ar.o
	${CC} -DMEXP=19937 -o $@ dumpMT.o mt19937ar.o

dumpPMT: dumpPMT.o pmt32-st.o
	${CC} -DMEXP=19937 -o $@ dumpPMT.o pmt32-st.o

dumpP37: dumpP37.o sfmtP37-st.o
	${CC} -DMEXP=19937 -o $@ dumpP37.o sfmtP37-st.o

dumpX78: dumpX78.o sfmtX78-st.o
	${CC} -DMEXP=19937 -o $@ dumpX78.o sfmtX78-st.o

dumpWELL: dumpWELL.o well19937a.o
	${CC} -DMEXP=19937 -o $@ dumpWELL.o well19937a.o

pmt32.dat: dumpPMT
	./dumpPMT 11000 pmt32.dat

sfmtP37.dat: dumpP37
	./dumpP37 11000 sfmtP37.dat

sfmtX78.dat: dumpX78
	./dumpX78 11000 sfmtX78.dat

well19937c.dat: dumpWELL
	./dumpWELL 11000 well19937c.dat

clean:
	rm -f *.o *~

.c.o:
	${CC} -c $<

.cpp.o:
	${CPP} -c $<