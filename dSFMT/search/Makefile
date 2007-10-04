#for GNU make

#DMEXP = -DMEXP=521
%521:MEXP=521
%1279:MEXP=1279
%2203:MEXP=2203
#%2281:MEXP=2281
%4253:MEXP=4253
#%4423:MEXP=4423
%11213:MEXP=11213
%19937:MEXP=19937
%44497:MEXP=44497
%86243:MEXP=86243
%132049:MEXP=132049
%216091:MEXP=216091

DDEBUG = -O0 -g

DMEXP = -DMEXP=${MEXP}

CC = gcc -Wall -O9 -Wmissing-prototypes $(DDEBUG) $(DMEXP) $(DPG)
CPP = g++ -Wall -O9 $(DDEBUG) $(DMEXP) $(DPG)

LINKOPT = -lntl -lgmp

param-searchDPf-521: searchL.o sfmtDPf-st.o mt19937blk.o util.o dsfmtL-util.o 
	${CPP} -o $@ searchL.o sfmtDPf-st.o mt19937blk.o util.o dsfmtL-util.o \
		${LINKOPT}

shortest-baseDPf-521: chk-shortestL.o shortbase128.o sfmtDPf-st.o util.o \
			dsfmtL-util.o mt19937blk.o
	${CPP} -o $@ chk-shortestL.o shortbase128.o sfmtDPf-st.o util.o \
			dsfmtL-util.o mt19937blk.o ${LINKOPT}

get-weightDPf-521: get-weightL.o sfmtDPf-st.o util.o \
			dsfmtL-util.o mt19937blk.o
	${CPP} -o $@ get-weightL.o sfmtDPf-st.o util.o \
			dsfmtL-util.o mt19937blk.o ${LINKOPT}

get-weightDPf-1279: get-weightL.o sfmtDPf-st.o util.o \
			dsfmtL-util.o mt19937blk.o
	${CPP} -o $@ get-weightL.o sfmtDPf-st.o util.o \
			dsfmtL-util.o mt19937blk.o ${LINKOPT}

searchDPf-parity521: search-parity-vector.o sfmtD11-st.o util.o
	${CPP} -o $@ search-parity-vector.o sfmtD11-st.o util.o \
		${LINKOPT}

testDPf-parity521:test-parity.o dSFMT-ref-st.o util.o dSFMT-ref-st.h
	 ${CPP} -o $@ test-parity.o dSFMT-ref-st.o util.o ${LINKOPT}

calc-fix-mat: calc-fix-mat.o
	${CPP} -o $@ calc-fix-mat.o ${LINKOPT}

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
