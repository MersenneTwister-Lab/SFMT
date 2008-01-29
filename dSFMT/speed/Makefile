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

STD_TARGET = sfmtD11_fastc sfmtX78_fastc sfmtX78_msk_fastc \
	sfmtX78_org_fastc mt_msk_fastc mt19937ar_fastc mt64_msk_fastc \
	sfmtDN2_fastca
ALTI_TARGET = sfmtD11_alti sfmtX78_alti sfmtX78_org_alti mt_msk_alti
SSE2_TARGET = sfmtD11_sse2 sfmtX78_sse2 sfmtX78_org_sse2 mt_msk_sse2 
SRC = sfmtD11_fastc.c sfmtD11_alti.c sfmtD11_sse2.c \
sfmtX78_fastc.c sfmtX78_alti.c sfmtX78_sse2.c \
mt19937ar.c mt_msk_fastc.c mt19937-64.c mt_msk_alti.c mt_msk_sse2.c \
test_time.c test_time2.c test_time3.c test_time4.c \
random.h c99.h mt19937ar.h speed.h paramsD11.h paramsD11-alti.h \
mt64.h do.sh Makefile

std: ${STD_TARGET}

athlon:
	${MAKE} CCFLAGS="${CCFLAGS} ${OPT_ATHLON}" ${STD_TARGET} ${SSE2_TARGET}

intel:
	${MAKE} CC=icl CCFLAGS="${CCFLAGS_INTEL}" ${STD_TARGET} ${SSE2_TARGET}

alti:
	${MAKE} CCFLAGS="${CCFLAGS} ${OPT_PPC}" ${STD_TARGET} \
	${ALTI_TARGET}

tar.gz: ${SRC}
	tar czvf speed.tar.gz ${SRC}

##### ALTIVEC ####
sfmtD11_alti: sfmtD11_alti.c test_time3.c  paramsD11.h
	${CC} -faltivec -maltivec ${CCFLAGS} -o $@ sfmtD11_alti.c

sfmtDPf_alti: sfmtDPf_alti.c test_time3.c  paramsDPf.h
	${CC} -faltivec -maltivec ${CCFLAGS} -o $@ sfmtDPf_alti.c

sfmtX78_alti: sfmtX78_alti.c test_time3.c
	${CC} -faltivec -maltivec ${CCFLAGS} -o $@ sfmtX78_alti.c

sfmtX78_org_alti: sfmtX78_alti.c test_time4.c
	${CC} -faltivec -maltivec ${CCFLAGS} -DORIGINAL -o $@ sfmtX78_alti.c

mt_msk_alti: mt_msk_alti.c test_time3.c
	${CC} -faltivec -maltivec ${CCFLAGS} -o $@ mt_msk_alti.c

##### SSE2 ####
sfmtD01_sse2: sfmtD01_sse2.c test_time.c  paramsD01.h
	${CC} ${CCFLAGS} -o $@ sfmtD01_sse2.c

sfmtD11_sse2: sfmtD11_sse2.c test_time3.c  paramsD11.h
	${CC} ${CCFLAGS} -o $@ sfmtD11_sse2.c

sfmtDPz1_sse2: sfmtDPz1_sse2.c test_time3.c  paramsDPz1.h
	${CC} ${CCFLAGS} -o $@ sfmtDPz1_sse2.c

sfmtDY1_sse2: sfmtDY1_sse2.c test_time3.c  paramsDY1.h
	${CC} ${CCFLAGS} -o $@ sfmtDY1_sse2.c

sfmtDYZ_sse2: sfmtDZ1_sse2.c test_time3.c  paramsDZ1.h
	${CC} ${CCFLAGS} -o $@ sfmtDZ1_sse2.c

sfmtP37_sse2: sfmtP37_sse2.c test_time.c
	${CC} ${CCFLAGS} -o $@ sfmtP37_sse2.c

sfmtX78_sse2: sfmtX78_sse2.c test_time3.c
	${CC} ${CCFLAGS} -o $@ sfmtX78_sse2.c

sfmtX78_org_sse2: sfmtX78_sse2.c test_time4.c
	${CC} ${CCFLAGS} -DORIGINAL -o $@ sfmtX78_sse2.c

mt_msk_sse2: mt_msk_sse2.c test_time3.c
	${CC} ${CCFLAGS} -o $@ mt_msk_sse2.c

#### NORMAL C ####
sfmtD01_fastc: sfmtD01_fastc.c test_time.c paramsD01.h
	${CC} ${CCFLAGS} -o $@ sfmtD01_fastc.c

sfmtD11_fastc: sfmtD11_fastc.c test_time3.c paramsD11.h
	${CC} ${CCFLAGS} -o $@ sfmtD11_fastc.c

sfmtDPf_fastc: sfmtDPf_fastc.c test_time3.c paramsDPf.h
	${CC} ${CCFLAGS} -o $@ sfmtDPf_fastc.c

sfmtDN2_fastc: sfmtDN2_fastc.c test_time3.c paramsDN2.h
	${CC} ${CCFLAGS} -o $@ sfmtDN2_fastc.c

sfmtDPz1_fastc: sfmtDPz1_fastc.c test_time3.c paramsDPz1.h
	${CC} ${CCFLAGS} -o $@ sfmtDPz1_fastc.c

sfmtDPz2_fastc: sfmtDPz2.c test_time3.c paramsDPz2.h
	${CC} ${CCFLAGS} -o $@ sfmtDPz2.c

sfmtDN2_fastca: sfmtDN2_fastca.c test_time3.c paramsDN2.h
	${CC} ${CCFLAGS} -o $@ sfmtDN2_fastca.c

sfmtP37_fastc: sfmtP37_fastc.c test_time2.c
	${CC} ${CCFLAGS} -o $@ sfmtP37_fastc.c

sfmtX78_fastc: sfmtX78_fastc.c test_time2.c
	${CC} ${CCFLAGS} -o $@ sfmtX78_fastc.c

sfmtX78_msk_fastc: sfmtX78_fastc.c test_time3.c
	${CC} ${CCFLAGS} -DBLOCK -o $@ sfmtX78_fastc.c

sfmtX78_org_fastc: sfmtX78_fastc.c test_time4.c
	${CC} ${CCFLAGS} -DORIGINAL -o $@ sfmtX78_fastc.c

mt19937ar_fastc: mt19937ar.c test_time2.c
	${CC} ${CCFLAGS} -o $@ mt19937ar.c

mt_msk_fastc: mt_msk_fastc.c test_time3.c
	${CC} ${CCFLAGS} -o $@ mt_msk_fastc.c

mt64_msk_fastc: mt19937-64.c test_time.c
	${CC} ${CCFLAGS} -o $@ mt19937-64.c

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -f *.o *~
