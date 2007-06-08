#for GNU make

#DMEXP = -DMEXP=521
%107:MEXP=107
%127:MEXP=127
%521:MEXP=521
%607:MEXP=607
%1279:MEXP=1279
#%2203:MEXP=2203
%2281:MEXP=2281
%4423:MEXP=4423
%11213:MEXP=11213
%19937:MEXP=19937
%44497:MEXP=44497
%86243:MEXP=86243
%110503:MEXP=110503
%132049:MEXP=132049
%216091:MEXP=216091

#DDEBUG = -O0 -g

DMEXP = -DMEXP=${MEXP}

CC = gcc -Wall -O9 -Wmissing-prototypes $(DDEBUG) $(DMEXP) $(DPG)
CPP = g++ -Wall -O9 $(DDEBUG) $(DMEXP) $(DPG)

LINKOPT = -lntl -lgmp

param-searchD11-607: search.o sfmtD11-st.o mt19937blk.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD11-st.o mt19937blk.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchDX1-607: search.o sfmtDX1-st.o mt19937blk.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtDX1-st.o mt19937blk.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchDY1-607: search.o sfmtDY1-st.o mt19937blk.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtDY1-st.o mt19937blk.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchDY2-607: search.o sfmtDY2-st.o mt19937blk.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtDY2-st.o mt19937blk.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchDY3-607: search.o sfmtDY3-st.o mt19937blk.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtDY3-st.o mt19937blk.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchDY4-607: searchL.o sfmtDY4-st.o mt19937blk.o util.o dsfmtL-util.o 
	${CPP} -o $@ searchL.o sfmtDY4-st.o mt19937blk.o util.o dsfmtL-util.o \
		${LINKOPT}

param-searchDY5-607: searchL.o sfmtDY5-st.o mt19937blk.o util.o dsfmtL-util.o 
	${CPP} -o $@ searchL.o sfmtDY5-st.o mt19937blk.o util.o dsfmtL-util.o \
		${LINKOPT}

param-searchDY6-607: searchL.o sfmtDY6-st.o mt19937blk.o util.o dsfmtL-util.o 
	${CPP} -o $@ searchL.o sfmtDY6-st.o mt19937blk.o util.o dsfmtL-util.o \
		${LINKOPT}

param-searchDY7-607: searchL.o sfmtDY7-st.o mt19937blk.o util.o dsfmtL-util.o 
	${CPP} -o $@ searchL.o sfmtDY7-st.o mt19937blk.o util.o dsfmtL-util.o \
		${LINKOPT}

param-searchDZ1-607: searchL2.o sfmtDZ1-st.o mt19937blk.o util.o dsfmtL2-util.o 
	${CPP} -o $@ searchL2.o sfmtDZ1-st.o mt19937blk.o util.o \
		dsfmtL2-util.o ${LINKOPT}

param-searchDP1-607: searchL.o sfmtDP1-st.o mt19937blk.o util.o dsfmtL-util.o 
	${CPP} -o $@ searchL.o sfmtDP1-st.o mt19937blk.o util.o dsfmtL-util.o \
		${LINKOPT}

param-searchDP2-607: searchL.o sfmtDP2-st.o mt19937blk.o util.o dsfmtL-util.o 
	${CPP} -o $@ searchL.o sfmtDP2-st.o mt19937blk.o util.o dsfmtL-util.o \
		${LINKOPT}

param-searchDP3-607: searchL.o sfmtDP3-st.o mt19937blk.o util.o dsfmtL-util.o 
	${CPP} -o $@ searchL.o sfmtDP3-st.o mt19937blk.o util.o dsfmtL-util.o \
		${LINKOPT}

param-searchDP6-607: searchL.o sfmtDP6-st.o mt19937blk.o util.o dsfmtL-util.o 
	${CPP} -o $@ searchL.o sfmtDP6-st.o mt19937blk.o util.o dsfmtL-util.o \
		${LINKOPT}

param-searchD11-1279: search.o sfmtD11-st.o mt19937blk.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD11-st.o mt19937blk.o util.o dsfmt-util.o \
		${LINKOPT}

shortest-baseD11-607: chk-shortest.o shortbase128.o sfmtD11-st.o util.o \
			dsfmt-util.o mt19937blk.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtD11-st.o util.o \
			dsfmt-util.o mt19937blk.o ${LINKOPT}

shortest-baseDY1-607: chk-shortest.o shortbase128.o sfmtDY1-st.o util.o \
			dsfmt-util.o mt19937blk.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtDY1-st.o util.o \
			dsfmt-util.o mt19937blk.o ${LINKOPT}

shortest-baseDY2-607: chk-shortest.o shortbase128.o sfmtDY2-st.o util.o \
			dsfmt-util.o mt19937blk.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtDY2-st.o util.o \
			dsfmt-util.o mt19937blk.o ${LINKOPT}

shortest-baseDY3-607: chk-shortest.o shortbase128.o sfmtDY3-st.o util.o \
			dsfmt-util.o mt19937blk.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtDY3-st.o util.o \
			dsfmt-util.o mt19937blk.o ${LINKOPT}

shortest-baseDY4-607: chk-shortest.o shortbase128.o sfmtDY4-st.o util.o \
			dsfmt-util.o mt19937blk.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtDY4-st.o util.o \
			dsfmt-util.o mt19937blk.o ${LINKOPT}

shortest-baseDP1-607: chk-shortest.o shortbase128.o sfmtDP1-st.o util.o \
			dsfmt-util.o mt19937blk.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtDP1-st.o util.o \
			dsfmt-util.o mt19937blk.o ${LINKOPT}

searchD11-parity607: search-parity-vector.o sfmtD11-st.o util.o
	${CPP} -o $@ search-parity-vector.o sfmtD11-st.o util.o \
		${LINKOPT}

searchD11-parity1279: search-parity-vector.o sfmtD11-st.o util.o
	${CPP} -o $@ search-parity-vector.o sfmtD11-st.o util.o \
		${LINKOPT}

searchD11-parity2281: search-parity-vector.o sfmtD11-st.o util.o
	${CPP} -o $@ search-parity-vector.o sfmtD11-st.o util.o \
		${LINKOPT}

searchD11-parity4423: search-parity-vector.o sfmtD11-st.o util.o
	${CPP} -o $@ search-parity-vector.o sfmtD11-st.o util.o \
		${LINKOPT}

searchD11-parity11213: search-parity-vector.o sfmtD11-st.o util.o
	${CPP} -o $@ search-parity-vector.o sfmtD11-st.o  util.o \
		${LINKOPT}

searchD11-parity19937: search-parity-vector.o sfmtD11-st.o  util.o
	${CPP} -o $@ search-parity-vector.o sfmtD11-st.o  util.o \
		${LINKOPT}

searchD11-parity44497: search-parity-vector.o sfmtD11-st.o util.o
	${CPP} -o $@ search-parity-vector.o sfmtD11-st.o util.o \
		${LINKOPT}

searchD11-parity86243: search-parity-vector.o sfmtD11-st.o util.o
	${CPP} -o $@ search-parity-vector.o sfmtD11-st.o util.o \
		${LINKOPT}

searchD11-parity132049: search-parity-vector.o sfmtD11-st.o util.o
	${CPP} -o $@ search-parity-vector.o sfmtD11-st.o util.o \
		${LINKOPT}

check-affin-bm607: chk-bm.o sfmtD11-afst.o util.o dsfmt-util.o
	${CPP} -o $@ chk-bm.o sfmtD11-afst.o util.o dsfmt-util.o ${LINKOPT}

testD11-parity607:test-parity.o dSFMT-ref-st.o util.o dSFMT-ref-st.h
	 ${CPP} -o $@ test-parity.o dSFMT-ref-st.o util.o ${LINKOPT}

testD11-parity1279:test-parity.o dSFMT-ref-st.o util.o dSFMT-ref-st.h
	 ${CPP} -o $@ test-parity.o dSFMT-ref-st.o util.o ${LINKOPT}

testD11-parity2281:test-parity.o dSFMT-ref-st.o util.o dSFMT-ref-st.h
	 ${CPP} -o $@ test-parity.o dSFMT-ref-st.o util.o ${LINKOPT}

testD11-parity4423:test-parity.o dSFMT-ref-st.o util.o dSFMT-ref-st.h
	 ${CPP} -o $@ test-parity.o dSFMT-ref-st.o util.o ${LINKOPT}

testD11-parity11213:test-parity.o dSFMT-ref-st.o util.o dSFMT-ref-st.h
	 ${CPP} -o $@ test-parity.o dSFMT-ref-st.o util.o ${LINKOPT}

testD11-parity19937:test-parity.o dSFMT-ref-st.o util.o dSFMT-ref-st.h
	 ${CPP} -o $@ test-parity.o dSFMT-ref-st.o util.o ${LINKOPT}

testD11-parity44497:test-parity.o dSFMT-ref-st.o util.o dSFMT-ref-st.h
	 ${CPP} -o $@ test-parity.o dSFMT-ref-st.o util.o ${LINKOPT}

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
