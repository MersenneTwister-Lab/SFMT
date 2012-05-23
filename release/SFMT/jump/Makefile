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
#ICCPPFLAGS = -mssse3

CC = gcc
#CC = icc -fast
GCCFLAGS = -msse2 --param max-inline-insns-single=1800
#ICCFLAGS = -fast
OPTI = -O3 -finline-functions -fomit-frame-pointer
CCSTD = -std=c99
WARN = -Wmissing-prototypes -Wall #-Winline
CCFLAGS = $(GCCFLAGS) $(ICCFLAGS) $(OPTI) $(WARN) $(CCSTD) $(DDEBUG)
CPPFLAGS = $(OPTI) $(GPPFLAGS) $(ICCPPFLAGS) $(WARN) $(DEFINES) $(DDEBUG)

LINKOPT = -lntl $(LIBGF2X) $(LIBGMP)

SSE2FLAGS = -msse2 -DHAVE_SSE2
STD_TARGET = test-std-M19937
ALL_STD_TARGET = test-std-M607 test-std-M1279 test-std-M2281 test-std-M4253 \
test-std-M11213 test-std-M19937 test-std-M44497 test-std-M86243 \
test-std-M132049 test-std-M216091
SSE2_TARGET = test-sse2-M19937
ALL_SSE2_TARGET = test-sse2-M607 test-sse2-M1279 test-sse2-M2281 \
test-sse2-M4253 test-sse2-M11213 test-sse2-M19937 test-sse2-M44497 \
test-sse2-M86243 test-sse2-M132049 test-sse2-M216091
ALL_JUMP_TARGET = test-jump-M607 test-jump-M1279 test-jump-M2281 \
test-jump-M4253 test-jump-M11213 test-jump-M19937 test-jump-M44497 \
test-jump-M86243 test-jump-M132049 test-jump-M216091
.PHONY: std-check sse2-check alti-check

#
# jump
#
all-jump:calc-characteristic test-jump test-string

calc-characteristic: calc-characteristic.cpp SFMText.hpp SFMT-calc-jump.hpp
	${CPP} ${CPPFLAGS} -o $@ calc-characteristic.cpp ${LINKOPT}

calc-jump: calc-jump.cpp SFMT-calc-jump.hpp
	${CPP} ${CPPFLAGS} -o $@ calc-jump.cpp ${LINKOPT}

test-string: test-string.cpp SFMT-calc-jump.hpp
	${CPP} ${CPPFLAGS} -o $@ test-string.cpp ${LINKOPT}

sample: sample.c SFMT-jump.h SFMTst.h SFMTst.c
	${CC} ${CCFLAGS} -DHAVE_SSE2 -DSFMT_MEXP=607 -o sample \
	sample.c SFMTst.c SFMT-jump.c

jump-check: ${ALL_JUMP_TARGET}
	./check-jump.sh 32 test-jump

#
# SFMT
#
std: ${STD_TARGET}

sse2:${SSE2_TARGET}

std-check: ${ALL_STD_TARGET}
	./check.sh 32 test-std

sse2-check: ${ALL_SSE2_TARGET}
	./check.sh 32 test-sse2

# --------------------
# START MEXP dependent
# --------------------
#test-std-MXXX: test.c SFMTst.c SFMTst.h SFMT-paramsXXX.h SFMT-common.h
#	${CC} ${CCFLAGS} -DSFMT_MEXP=XXX -o $@ test.c SFMTst.c
#
#test-sse2-MXXX: test.c SFMTst.c SFMTst.h SFMT-paramsXXX.h SFMT-common.h
#	${CC} ${CCFLAGS} -DSFMT_MEXP=XXX -DHAVE_SSE2 -o $@ test.c SFMTst.c
#
#test-jump-MXXX: test-jump.cpp SFMTst.c SFMT-jump.c SFMT-calc-jump.hpp \
#	SFMT-common.h
#	${CPP} ${CPPFLAGS} -DSFMT_MEXP=XXX -DHAVE_SSE2 -o $@ \
#	test-jump.cpp SFMTst.c SFMT-jump.c ${LINKOPT}
test-std-M607: test.c SFMTst.c SFMTst.h SFMT-params607.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=607 -o $@ test.c SFMTst.c

test-sse2-M607: test.c SFMTst.c SFMTst.h SFMT-params607.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=607 -DHAVE_SSE2 -o $@ test.c SFMTst.c

test-jump-M607: test-jump.cpp SFMTst.c SFMT-jump.c SFMT-calc-jump.hpp \
	SFMT-common.h
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=607 -DHAVE_SSE2 -o $@ \
	test-jump.cpp SFMTst.c SFMT-jump.c ${LINKOPT}

test-std-M1279: test.c SFMTst.c SFMTst.h SFMT-params1279.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=1279 -o $@ test.c SFMTst.c

test-sse2-M1279: test.c SFMTst.c SFMTst.h SFMT-params1279.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=1279 -DHAVE_SSE2 -o $@ test.c SFMTst.c

test-jump-M1279: test-jump.cpp SFMTst.c SFMT-jump.c SFMT-calc-jump.hpp \
	SFMT-common.h
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=1279 -DHAVE_SSE2 -o $@ \
	test-jump.cpp SFMTst.c SFMT-jump.c ${LINKOPT}

test-std-M2281: test.c SFMTst.c SFMTst.h SFMT-params2281.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=2281 -o $@ test.c SFMTst.c

test-sse2-M2281: test.c SFMTst.c SFMTst.h SFMT-params2281.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=2281 -DHAVE_SSE2 -o $@ test.c SFMTst.c

test-jump-M2281: test-jump.cpp SFMTst.c SFMT-jump.c SFMT-calc-jump.hpp \
	SFMT-common.h
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=2281 -DHAVE_SSE2 -o $@ \
	test-jump.cpp SFMTst.c SFMT-jump.c ${LINKOPT}

test-std-M4253: test.c SFMTst.c SFMTst.h SFMT-params4253.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=4253 -o $@ test.c SFMTst.c

test-sse2-M4253: test.c SFMTst.c SFMTst.h SFMT-params4253.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=4253 -DHAVE_SSE2 -o $@ test.c SFMTst.c

test-jump-M4253: test-jump.cpp SFMTst.c SFMT-jump.c SFMT-calc-jump.hpp \
	SFMT-common.h
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=4253 -DHAVE_SSE2 -o $@ \
	test-jump.cpp SFMTst.c SFMT-jump.c ${LINKOPT}

test-std-M11213: test.c SFMTst.c SFMTst.h SFMT-params11213.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=11213 -o $@ test.c SFMTst.c

test-sse2-M11213: test.c SFMTst.c SFMTst.h SFMT-params11213.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=11213 -DHAVE_SSE2 -o $@ test.c SFMTst.c

test-jump-M11213: test-jump.cpp SFMTst.c SFMT-jump.c SFMT-calc-jump.hpp \
	SFMT-common.h
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=11213 -DHAVE_SSE2 -o $@ \
	test-jump.cpp SFMTst.c SFMT-jump.c ${LINKOPT}

test-std-M19937: test.c SFMTst.c SFMTst.h SFMT-params19937.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=19937 -o $@ test.c SFMTst.c

test-sse2-M19937: test.c SFMTst.c SFMTst.h SFMT-params19937.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=19937 -DHAVE_SSE2 -o $@ test.c SFMTst.c

test-jump-M19937: test-jump.cpp SFMTst.c SFMT-jump.c SFMT-calc-jump.hpp \
	SFMT-common.h
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=19937 -DHAVE_SSE2 -o $@ \
	test-jump.cpp SFMTst.c SFMT-jump.c ${LINKOPT}

test-std-M44497: test.c SFMTst.c SFMTst.h SFMT-params44497.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=44497 -o $@ test.c SFMTst.c

test-sse2-M44497: test.c SFMTst.c SFMTst.h SFMT-params44497.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=44497 -DHAVE_SSE2 -o $@ test.c SFMTst.c

test-jump-M44497: test-jump.cpp SFMTst.c SFMT-jump.c SFMT-calc-jump.hpp \
	SFMT-common.h
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=44497 -DHAVE_SSE2 -o $@ \
	test-jump.cpp SFMTst.c SFMT-jump.c ${LINKOPT}

test-std-M86243: test.c SFMTst.c SFMTst.h SFMT-params86243.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=86243 -o $@ test.c SFMTst.c

test-sse2-M86243: test.c SFMTst.c SFMTst.h SFMT-params86243.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=86243 -DHAVE_SSE2 -o $@ test.c SFMTst.c

test-jump-M86243: test-jump.cpp SFMTst.c SFMT-jump.c SFMT-calc-jump.hpp \
	SFMT-common.h
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=86243 -DHAVE_SSE2 -o $@ \
	test-jump.cpp SFMTst.c SFMT-jump.c ${LINKOPT}

test-std-M132049: test.c SFMTst.c SFMTst.h SFMT-params132049.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=132049 -o $@ test.c SFMTst.c

test-sse2-M132049: test.c SFMTst.c SFMTst.h SFMT-params132049.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=132049 -DHAVE_SSE2 -o $@ test.c SFMTst.c

test-jump-M132049: test-jump.cpp SFMTst.c SFMT-jump.c SFMT-calc-jump.hpp \
	SFMT-common.h
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=132049 -DHAVE_SSE2 -o $@ \
	test-jump.cpp SFMTst.c SFMT-jump.c ${LINKOPT}

test-std-M216091: test.c SFMTst.c SFMTst.h SFMT-params216091.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=216091 -o $@ test.c SFMTst.c

test-sse2-M216091: test.c SFMTst.c SFMTst.h SFMT-params216091.h SFMT-common.h
	${CC} ${CCFLAGS} -DSFMT_MEXP=216091 -DHAVE_SSE2 -o $@ test.c SFMTst.c

test-jump-M216091: test-jump.cpp SFMTst.c SFMT-jump.c SFMT-calc-jump.hpp \
	SFMT-common.h
	${CPP} ${CPPFLAGS} -DSFMT_MEXP=216091 -DHAVE_SSE2 -o $@ \
	test-jump.cpp SFMTst.c SFMT-jump.c ${LINKOPT}

# ------------------
# END MEXP dependent
# ------------------
.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -rf *.o *~ *.dSYM

clean-all:
	rm -rf *.o *~ *.dSYM ${ALL_STD_TARGET} ${ALL_SSE2_TARGET} \
	${ALL_JUMP_TARGET}
