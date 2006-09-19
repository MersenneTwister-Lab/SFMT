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


count1bit: count1bit.c
	${CC} -o $@ count1bit.c

dumpPMT: dumpPMT.o pmt32-st.o
	${CC} -o $@ dumpPMT.o pmt32-st.o

dumpP37: dumpP37.o sfmtP37-st.o
	${CC} -o $@ dumpP37.o sfmtP37-st.o

dumpX78: dumpX78.o sfmtX78-st.o
	${CC} -o $@ dumpX78.o sfmtX78-st.o

dumpPMT.dat: dumpPMT
	./dumpPMT 11000 dumpPMT.dat

dumpP37.dat: dumpP37
	./dumpP37 11000 dumpP37.dat

dumpX78.dat: dumpX78
	./dumpX78 11000 dumpX78.dat

clean:
	rm -f *.o *~

%.count.txt: %.dat
	./count1bit.c $< > $@

.c.o:
	${CC} -c $<