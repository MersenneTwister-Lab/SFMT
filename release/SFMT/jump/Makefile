#for GNU make

#DDEBUG = -O0 -g -ggdb -DDEBUG=1

# uncomment next line if you installed NTL with gf2x
LIBGF2X = -lgf2x
# uncomment next line if you installed NTL with gmp
#LIBGMP = -lgmp

DEFINES = -D__STDC_CONSTANT_MACROS=1
CPP = g++
#CPP = icc
GPPFLAGS = -msse2

CC = gcc
#CC = icc -fast
GCCFLAGS = -msse2 --param max-inline-insns-single=1800
#ICCFLAGS = -fast
OPTI = -O3 -finline-functions -fomit-frame-pointer
CCSTD = -std=c99
WARN = -Wmissing-prototypes -Wall #-Winline
CCFLAGS = $(GCCFLAGS) $(ICCFLAGS) $(OPTI) $(WARN) $(CCSTD) $(DDEBUG)
CPPFLAGS = $(OPTI) $(GPPFLAGS) $(WARN) $(DEFINES) $(DDEBUG)

#LINKOPT = -lntl -lcrypto $(LIBGF2X) $(LIBGMP)
LINKOPT = -lntl $(LIBGF2X) $(LIBGMP)

all:calc-characteristic calc-jump test-string test-std

calc-characteristic: calc-characteristic.cpp SFMText.hpp SFMT-calc-jump.hpp
	${CPP} ${CPPFLAGS} -o $@ calc-characteristic.cpp ${LINKOPT}

test-string: test-string.cpp SFMT-calc-jump.hpp
	${CPP} ${CPPFLAGS} -o $@ test-string.cpp ${LINKOPT}

test-std-M607: test.c SFMTst.c SFMTst.h SFMT-params607.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=607 -o $@ test.c SFMTst.c

test-sse2-M607: test.c SFMTst.c SFMTst.h SFMT-params607.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=607 -DHAVE_SSE2 -o $@ test.c SFMTst.c

test-jump-M607: test-jump.cpp SFMTst.c SFMT-jump.c SFMT-calc-jump.hpp \
	SFMT-common.h
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=607 -o $@ test-jump.cpp SFMTst.c \
	SFMT-jump.c ${LINKOPT}

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -rf *.o *~ *.dSYM
