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
OPTI = -O9 -finline-functions -fomit-frame-pointer -DNDEBUG 
#STD = -std=c89 -pedantic
#STD = -std=c99 -pedantic
GCC = gcc $(OPTI) $(WARN) $(STD)
CC = $(GCC)

recover.eps: sfmtX78.count.txt sfmtP37.count.txt pmt32.count.txt \
	well19937c.count.txt
	gnuplot < count.plt

count1bit: count1bit.c
	${CC} -o $@ count1bit.c

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

%.count.txt: %.dat
	./count1bit $< > $@

.c.o:
	${CC} -c $<