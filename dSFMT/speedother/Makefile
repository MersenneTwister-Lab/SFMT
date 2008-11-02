#for GNU make
#DDEBUG = -O0 -g -DDEBUG
CC = gcc
DMEXP = -DMEXP=19937
OPT_PPC = -faltivec -maltivec -arch ppc 
OPT_ATHLON = -march=athlon64 
CCFLAGS_INTEL = /Wcheck /O3 /QxB /Qc99 /Qprefetch /Qip /Fa -DNDEBUG
OPT = --param max-inline-insns-single=1800 --param inline-unit-growth=500 \
--param large-function-growth=900 
CCFLAGS = -Wall -O9 -Wmissing-prototypes -Winline $(OPT) -DNDEBUG $(DDEBUG) \
$(DMEXP) $(DPG)

STD_TARGET = well1024 well19937 xor128 mt19937
SRC = sfmtD11_fastc.c sfmtD11_alti.c sfmtD11_sse2.c \
sfmtX78_fastc.c sfmtX78_alti.c sfmtX78_sse2.c \
mt19937ar.c mt_msk_fastc.c mt19937-64.c mt_msk_alti.c mt_msk_sse2.c \
test_time.c test_time2.c test_time3.c test_time4.c \
random.h c99.h mt19937ar.h speed.h paramsD11.h paramsD11-alti.h \
mt64.h do.sh Makefile

std: ${STD_TARGET}

athlon:
	${MAKE} CCFLAGS="${CCFLAGS} ${OPT_ATHLON}" ${STD_TARGET}

intel:
	${MAKE} CC=icl CCFLAGS="${CCFLAGS_INTEL}" ${STD_TARGET}

alti:
	${MAKE} CCFLAGS="${CCFLAGS} ${OPT_PPC}" ${STD_TARGET}

tar.gz: ${SRC}
	tar czvf speed.tar.gz ${SRC}

#### NORMAL C ####
well1024: well1024a.c test_time2.c
	${CC} ${CCFLAGS} -o $@ well1024a.c

well19937: well19937a.c test_time2.c
	${CC} ${CCFLAGS} -o $@ well19937a.c

xor128: xorshift128.c test_time2.c
	${CC} ${CCFLAGS} -o $@ xorshift128.c

mt19937: mt19937ar.c test_time2.c
	${CC} ${CCFLAGS} -o $@ mt19937ar.c


.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -f *.o *~
