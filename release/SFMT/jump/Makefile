#for GNU make

#DDEBUG = -O0 -g -ggdb -DDEBUG=1

# uncomment next line if you installed NTL with gf2x
LIBGF2X = -lgf2x
# uncomment next line if you installed NTL with gmp
#LIBGMP = -lgmp

DEFINES = -D__STDC_CONSTANT_MACROS=1
CPP = g++ -Wall -Wextra -O3 -I../include -msse3 $(DEFINES) $(DDEBUG)
#CPP = icc -Wall -Wextra -O3 -I../include $(DEFINES) $(DDEBUG)

#CC = gcc
CC = icc -fast
OPTI = -O3 -finline-functions -fomit-frame-pointer -DNDEBUG \
-fno-strict-aliasing --param max-inline-insns-single=1800
STD = -std=c99
WARN = -Wmissing-prototypes -Wall #-Winline
CCFLAGS = $(OPTI) $(WARN) $(STD) $(DDEBUG)

#LINKOPT = -lntl -lcrypto $(LIBGF2X) $(LIBGMP)
LINKOPT = -lntl $(LIBGF2X) $(LIBGMP)

all:calc-characteristic calc-jump test-string test-std

calc-characteristic: calc-characteristic.cpp SFMText.hpp SFMT-calc-jump.hpp
	${CPP} -o $@ calc-characteristic.cpp ${LINKOPT}

calc-jump: calc-jump.cpp SFMTst.c SFMT-jump.c SFMT-calc-jump.hpp
	${CPP} -DSFMT_MEXP=607 -o $@ calc-jump.cpp SFMTst.c SFMT-jump.c \
	${LINKOPT}

test-string: test-string.cpp SFMT-calc-jump.hpp
	${CPP} -o $@ test-string.cpp ${LINKOPT}

test-std: test.c SFMTst.c SFMTst.h SFMT-params607.h SFMT-hardware.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=607 -o $@ test.c SFMTst.c

test-sse2: test.c SFMTst.c SFMTst.h SFMT-params607.h SFMT-hardware.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=607 -DHAVE_SSE2 -o $@ test.c SFMTst.c

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -rf *.o *~ *.dSYM
