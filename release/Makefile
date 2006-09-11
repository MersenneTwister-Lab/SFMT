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
STD = 
# -----------------
# for standard unix
# -----------------
#CC = gcc $(OPTI) $(STD)

# -----------------
# for power PC
# -----------------
CC = gcc $(WARN) $(OPTI) $(STD) -faltivec -maltivec -arch ppc

# -----------------
# for Pentium M
# -----------------
#CC = gcc $(OPTI) $(STD) -march=prescott
#CC = icl /Wcheck /O3 /QxB /Qprefetch

# -----------------
# for Athlon
# -----------------
#CC = gcc $(OPTI) $(STD) -march=nocona

sse2: test-std32 test-std64 test-sse32 test-sse64
altivec: test-std32 test-std64 test-alti32 test-alti64

test-std32: test32.c sfmt19937.c sfmt19937.h Makefile
	${CC} -o $@ test32.c

test-std64: test64.c sfmt19937.c sfmt19937.h Makefile
	${CC} -o $@ test64.c

test-alti32: test_alti32.c sfmt19937-alti32.c sfmt19937.h Makefile
	${CC} -o $@ test_alti32.c

test-alti64: test_alti64.c sfmt19937-alti64.c sfmt19937.h Makefile
	${CC} -o $@ test_alti64.c

test-sse32: test_sse32.c sfmt19937-sse2.c sfmt19937.h Makefile
	${CC} -o $@ test_sse32.c

test-sse64: test_sse64.c sfmt19937-sse2.c sfmt19937.h Makefile
	${CC} -o $@ test_sse64.c

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
