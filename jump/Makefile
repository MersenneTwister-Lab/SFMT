#for GNU make

#DDEBUG = -O0 -g -DDEBUG=1

# uncomment next line if you installed NTL with gf2x
LIBGF2X = -lgf2x
# uncomment next line if you installed NTL with gmp
#LIBGMP = -lgmp

DEFINES = -D__STDC_CONSTANT_MACROS=1
#CPP = g++ -Wall -Wextra -O3 -I../include -msse3 $(DEFINES) $(DDEBUG)
CPP = icc -Wall -Wextra -O3 -I../include -msse3 $(DEFINES) $(DDEBUG)

#CC = gcc
CC = icc
OPTI = -O3 -finline-functions -fomit-frame-pointer -DNDEBUG \
-fno-strict-aliasing
STD = -std=c99
WARN = -Wmissing-prototypes -Wall #-Winline
CCFLAGS = $(OPTI) $(WARN) $(STD) $(DDEBUG)

LINKOPT = -lntl -lcrypto $(LIBGF2X) $(LIBGMP)

calc-characteristic: calc-characteristic.o SFMText.hpp string-poly.hpp
	${CPP} -o $@ calc-characteristic.o ${LINKOPT}

calc-jump: calc-jump.o string-poly.hpp
	${CPP} -o $@ calc-jump.o ${LINKOPT}

test-std: test.c SFMTst.c SFMTst.h SFMT-params607.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=607 -o $@ test.c SFMTst.c

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -rf *.o *~ *.dSYM
