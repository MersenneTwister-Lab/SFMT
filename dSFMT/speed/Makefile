#for GNU make
#DDEBUG = -O0 -g -DDEBUG
CC = gcc
DMEXP = -DMEXP=19937
OPT_PPC = -faltivec -maltivec -arch ppc 
OPT_ATHLON = -march=athlon64 
CCFLAGS_INTEL = /Wcheck /O3 /QxB /Qc99 /Qprefetch /Qip /Fa -DNDEBUG
OPT = --param max-inline-insns-single=1800 --param inline-unit-growth=300 \
--param large-function-growth=900 
CCFLAGS = -Wall -O9 -Wmissing-prototypes -Winline $(OPT) -DNDEBUG $(DDEBUG) \
$(DMEXP) $(DPG)
STD_TARGET = sfmtD11_fastc sfmtX78_fastc sfmtX78_msk_fastc mt_msk_fastc \
	mt19937ar_fastc
ALTI_TARGET = sfmtD11_alti sfmtX78_alti mt_msk_alti
SSE2_TARGET = sfmtD11_sse2 sfmtX78_sse2 mt_msk_sse2

std: ${STD_TARGET}

athlon:
	${MAKE} CCFLAGS="${CCFLAGS} ${OPT_ATHLON}" ${STD_TARGET} ${SSE2_TARGET}

intel:
	${MAKE} CC=icl CCFLAGS="${CCFLAGS_INTEL}" ${STD_TARGET} ${SSE2_TARGET}

alti:
	${MAKE} CCFLAGS="${CCFLAGS} ${OPT_PPC} -DBIG_ENDIAN" ${STD_TARGET} \
	${ALTI_TARGET}

##### ALTIVEC ####
sfmtD01_alti: sfmtD01_alti.c test_time.c  paramsD01.h
	${CC} ${CCFLAGS} -o $@ sfmtD01_alti.c

sfmtD11_alti: sfmtD11_alti.c test_time.c  paramsD11.h
	${CC} ${CCFLAGS} -o $@ sfmtD11_alti.c

sfmtP37_alti: sfmtP37_alti.c test_time.c
	${CC} ${CCFLAGS} -o $@ sfmtP37_alti.c

sfmtX78_alti: sfmtX78_alti.c test_time.c
	${CC} ${CCFLAGS} -o $@ sfmtX78_alti.c

mt_msk_alti: mt_msk_alti.c test_time.c
	${CC} ${CCFLAGS} -o $@ mt_msk_alti.c

##### SSE2 ####
sfmtD01_sse2: sfmtD01_sse2.c test_time.c  paramsD01.h
	${CC} ${CCFLAGS} -o $@ sfmtD01_sse2.c

sfmtD11_sse2: sfmtD11_sse2.c test_time.c  paramsD11.h
	${CC} ${CCFLAGS} -o $@ sfmtD11_sse2.c

sfmtP37_sse2: sfmtP37_sse2.c test_time.c
	${CC} ${CCFLAGS} -o $@ sfmtP37_sse2.c

sfmtX78_sse2: sfmtX78_sse2.c test_time.c
	${CC} ${CCFLAGS} -o $@ sfmtX78_sse2.c

mt_msk_sse2: mt_msk_sse2.c test_time.c
	${CC} ${CCFLAGS} -o $@ mt_msk_sse2.c

#### NORMAL C ####
sfmtD01_fastc: sfmtD01_fastc.c test_time.c paramsD01.h
	${CC} ${CCFLAGS} -o $@ sfmtD01_fastc.c

sfmtD11_fastc: sfmtD11_fastc.c test_time.c paramsD11.h
	${CC} ${CCFLAGS} -o $@ sfmtD11_fastc.c

sfmtP37_fastc: sfmtP37_fastc.c test_time2.c
	${CC} ${CCFLAGS} -o $@ sfmtP37_fastc.c

sfmtX78_fastc: sfmtX78_fastc.c test_time2.c
	${CC} ${CCFLAGS} -o $@ sfmtX78_fastc.c

sfmtX78_msk_fastc: sfmtX78_fastc.c test_time.c
	${CC} ${CCFLAGS} -DBLOCK -o $@ sfmtX78_fastc.c

mt19937ar_fastc: mt19937ar.c test_time2.c
	${CC} ${CCFLAGS} -o $@ mt19937ar.c

mt_msk_fastc: mt_msk_fastc.c test_time.c
	${CC} ${CCFLAGS} -DBIG_ENDIAN -o $@ mt_msk_fastc.c

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -f *.o *~
