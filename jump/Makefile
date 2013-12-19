#for GNU make

#DDEBUG = -O0 -g -ggdb -DDEBUG=1

# uncomment next line if you installed NTL with gf2x
#LIBGF2X = -lgf2x
# uncomment next line if you installed NTL with gmp
#LIBGMP = -lgmp

DEFINES = -D__STDC_CONSTANT_MACROS=1 -D__STDC_FORMAT_MACROS=1
CPP = g++
#CPP = icc
GPPFLAGS = -msse2
#ICCPPFLAGS = -mssse3

CC = gcc
#CC = icc -fast
GCCFLAGS = -msse2
#ICCFLAGS = -fast
OPTI = -O3 -finline-functions -fomit-frame-pointer
CCSTD = -std=c99
WARN = -Wall #-Winline
SSE2FLAGS = -msse2 -DHAVE_SSE2
CCFLAGS = -Wmissing-prototypes -I../ $(SSE2FLAGS) $(GCCFLAGS) $(ICCFLAGS) \
	$(OPTI) $(WARN) $(CCSTD) $(DDEBUG)
CPPFLAGS = -I../ $(SSE2FLAGS) $(OPTI) $(GPPFLAGS) $(ICCPPFLAGS) $(WARN) \
	$(DEFINES) $(DDEBUG)

LINKOPT = -lntl $(LIBGF2X) $(LIBGMP)

ALL_JUMP_TARGET = test-jump-M607 test-jump-M1279 test-jump-M2281 \
test-jump-M4253 test-jump-M11213 test-jump-M19937 test-jump-M44497 \
test-jump-M86243 test-jump-M132049 test-jump-M216091

all: calc-jump
check: jump-check
#
# jump
#

calc-jump: calc-jump.cpp SFMT-calc-jump.hpp
	${CPP} ${CPPFLAGS} -o $@ calc-jump.cpp ${LINKOPT}

jump-check: ${ALL_JUMP_TARGET}
	./check-jump.sh

sample1: sample1.c SFMT-jump.h
	${CC} ${CCFLAGS} -DHAVE_SSE2 -DSFMT_MEXP=1279 -o $@ \
	sample1.c ../SFMT.c SFMT-jump.c

sample2: sample2.c SFMT-jump.h
	${CC} ${CCFLAGS} -DHAVE_SSE2 -DSFMT_MEXP=607 -o $@ \
	sample2.c ../SFMT.c SFMT-jump.c

# --------------------
# START MEXP dependent
# --------------------
#test-jump-MXXX: test-jump.cpp ../SFMT.c SFMT-jump.c SFMT-calc-jump.hpp
#	${CPP} ${CPPFLAGS} -DSFMT_MEXP=XXX -DHAVE_SSE2 -o $@ \
#	test-jump.cpp ../SFMT.c SFMT-jump.c ${LINKOPT}
test-jump-M607: test-jump.cpp SFMT-jump.c SFMT-calc-jump.hpp
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=607 -DHAVE_SSE2 -o $@ \
	test-jump.cpp ../SFMT.c SFMT-jump.c ${LINKOPT}

test-jump-M1279: test-jump.cpp ../SFMT.c SFMT-jump.c SFMT-calc-jump.hpp
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=1279 -DHAVE_SSE2 -o $@ \
	test-jump.cpp ../SFMT.c SFMT-jump.c ${LINKOPT}

test-jump-M2281: test-jump.cpp ../SFMT.c SFMT-jump.c SFMT-calc-jump.hpp
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=2281 -DHAVE_SSE2 -o $@ \
	test-jump.cpp ../SFMT.c SFMT-jump.c ${LINKOPT}

test-jump-M4253: test-jump.cpp ../SFMT.c SFMT-jump.c SFMT-calc-jump.hpp
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=4253 -DHAVE_SSE2 -o $@ \
	test-jump.cpp ../SFMT.c SFMT-jump.c ${LINKOPT}

test-jump-M11213: test-jump.cpp ../SFMT.c SFMT-jump.c SFMT-calc-jump.hpp
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=11213 -DHAVE_SSE2 -o $@ \
	test-jump.cpp ../SFMT.c SFMT-jump.c ${LINKOPT}

test-jump-M19937: test-jump.cpp ../SFMT.c SFMT-jump.c SFMT-calc-jump.hpp
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=19937 -DHAVE_SSE2 -o $@ \
	test-jump.cpp ../SFMT.c SFMT-jump.c ${LINKOPT}

test-jump-M44497: test-jump.cpp ../SFMT.c SFMT-jump.c SFMT-calc-jump.hpp
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=44497 -DHAVE_SSE2 -o $@ \
	test-jump.cpp ../SFMT.c SFMT-jump.c ${LINKOPT}

test-jump-M86243: test-jump.cpp ../SFMT.c SFMT-jump.c SFMT-calc-jump.hpp
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=86243 -DHAVE_SSE2 -o $@ \
	test-jump.cpp ../SFMT.c SFMT-jump.c ${LINKOPT}

test-jump-M132049: test-jump.cpp ../SFMT.c SFMT-jump.c SFMT-calc-jump.hpp
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=132049 -DHAVE_SSE2 -o $@ \
	test-jump.cpp ../SFMT.c SFMT-jump.c ${LINKOPT}

test-jump-M216091: test-jump.cpp ../SFMT.c SFMT-jump.c SFMT-calc-jump.hpp
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=216091 -DHAVE_SSE2 -o $@ \
	test-jump.cpp ../SFMT.c SFMT-jump.c ${LINKOPT}

# --------------------
# END MEXP dependent
# --------------------
.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -rf *.o *~ *.dSYM

clean-all:
	rm -rf *.o *~ *.dSYM ${ALL_STD_TARGET} ${ALL_SSE2_TARGET} \
	${ALL_JUMP_TARGET}
