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

gen-ref607: dSFMT-ref-st.c gen-ref.c
	${CC} -o $@ dSFMT-ref-st.c gen-ref.c

genD11-607: sfmtD11-st.c genD11.c
	${CC} -o $@ sfmtD11-st.c genD11.c

gen-ref1279: dSFMT-ref-st.c gen-ref.c
	${CC} -o $@ dSFMT-ref-st.c gen-ref.c

genD11-1279: sfmtD11-st.c genD11.c
	${CC} -o $@ sfmtD11-st.c genD11.c

gen-ref2281: dSFMT-ref-st.c gen-ref.c
	${CC} -o $@ dSFMT-ref-st.c gen-ref.c

genD11-2281: sfmtD11-st.c genD11.c
	${CC} -o $@ sfmtD11-st.c genD11.c

gen-ref4423: dSFMT-ref-st.c gen-ref.c
	${CC} -o $@ dSFMT-ref-st.c gen-ref.c

genD11-4423: sfmtD11-st.c genD11.c
	${CC} -o $@ sfmtD11-st.c genD11.c

gen-ref11213: dSFMT-ref-st.c gen-ref.c
	${CC} -o $@ dSFMT-ref-st.c gen-ref.c

genD11-11213: sfmtD11-st.c genD11.c
	${CC} -o $@ sfmtD11-st.c genD11.c

gen-ref19937: dSFMT-ref-st.c gen-ref.c
	${CC} -o $@ dSFMT-ref-st.c gen-ref.c

genD11-19937: sfmtD11-st.c genD11.c
	${CC} -o $@ sfmtD11-st.c genD11.c

gen-ref44497: dSFMT-ref-st.c gen-ref.c
	${CC} -o $@ dSFMT-ref-st.c gen-ref.c

genD11-44497: sfmtD11-st.c genD11.c
	${CC} -o $@ sfmtD11-st.c genD11.c

gen-ref86243: dSFMT-ref-st.c gen-ref.c
	${CC} -o $@ dSFMT-ref-st.c gen-ref.c

genD11-86243: sfmtD11-st.c genD11.c
	${CC} -o $@ sfmtD11-st.c genD11.c

gen-ref132049: dSFMT-ref-st.c gen-ref.c
	${CC} -o $@ dSFMT-ref-st.c gen-ref.c

genD11-132049: sfmtD11-st.c genD11.c
	${CC} -o $@ sfmtD11-st.c genD11.c

test-ref-parity607:test-ref-parity.o dsfmt-ref-st.o util.o dsfmt-ref-st.h
	 ${CPP} -o $@ test-ref-parity.o dsfmt-ref-st.o util.o ${LINKOPT}

test-ref2-parity607:test-ref-parity.o dsfmt-ref-st2.o util.o dsfmt-ref-st.h
	 ${CPP} -o $@ test-ref-parity.o dsfmt-ref-st2.o util.o ${LINKOPT}

test-ref-parity1279:test-ref-parity.o dsfmt-ref-st.o util.o dsfmt-ref-st.h
	 ${CPP} -o $@ test-ref-parity.o dsfmt-ref-st.o util.o ${LINKOPT}

test-ref2-parity1279:test-ref-parity.o dsfmt-ref-st2.o util.o dsfmt-ref-st.h
	 ${CPP} -o $@ test-ref-parity.o dsfmt-ref-st2.o util.o ${LINKOPT}

test-ref-parity2281:test-ref-parity.o dsfmt-ref-st.o util.o dsfmt-ref-st.h
	 ${CPP} -o $@ test-ref-parity.o dsfmt-ref-st.o util.o ${LINKOPT}

test-ref-parity4423:test-ref-parity.o dsfmt-ref-st.o util.o dsfmt-ref-st.h
	 ${CPP} -o $@ test-ref-parity.o dsfmt-ref-st.o util.o ${LINKOPT}

test-ref-parity11213:test-ref-parity.o dsfmt-ref-st.o util.o dsfmt-ref-st.h
	 ${CPP} -o $@ test-ref-parity.o dsfmt-ref-st.o util.o ${LINKOPT}

test-ref-parity19937:test-ref-parity.o dsfmt-ref-st.o util.o dsfmt-ref-st.h
	 ${CPP} -o $@ test-ref-parity.o dsfmt-ref-st.o util.o ${LINKOPT}

test-ref-parity44497:test-ref-parity.o dsfmt-ref-st.o util.o dsfmt-ref-st.h
	 ${CPP} -o $@ test-ref-parity.o dsfmt-ref-st.o util.o ${LINKOPT}

test-ref-parity86243:test-ref-parity.o dsfmt-ref-st.o util.o dsfmt-ref-st.h
	 ${CPP} -o $@ test-ref-parity.o dsfmt-ref-st.o util.o ${LINKOPT}

test-ref-parity132049:test-ref-parity.o dsfmt-ref-st.o util.o dsfmt-ref-st.h
	 ${CPP} -o $@ test-ref-parity.o dsfmt-ref-st.o util.o ${LINKOPT}

test-ref2-parity216091:test-ref-parity.o dsfmt-ref-st2.o util.o dsfmt-ref-st.h
	 ${CPP} -o $@ test-ref-parity.o dsfmt-ref-st2.o util.o ${LINKOPT}

test-d11-parity607:test-d11-parity.o sfmtD11-st.o util.o
	 ${CPP} -o $@ test-d11-parity.o sfmtD11-st.o util.o ${LINKOPT}

test-d11-parity1279:test-d11-parity.o sfmtD11-st.o util.o
	 ${CPP} -o $@ test-d11-parity.o sfmtD11-st.o util.o ${LINKOPT}

test-d11-spar607:test-104spar.o sfmtD11-st.o
	 ${CPP} -o $@ test-104spar.o sfmtD11-st.o ${LINKOPT}

shortest-baseD11-607: chk-shortest.o shortbase128.o sfmtD11-st.o util.o \
			dsfmt-util.o mt19937blk.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtD11-st.o util.o \
			dsfmt-util.o mt19937blk.o ${LINKOPT}

check-affin-bm607: chk-bm.o sfmtD11-afst.o util.o dsfmt-util.o
	${CPP} -o $@ chk-bm.o sfmtD11-afst.o util.o dsfmt-util.o ${LINKOPT}

fix-za: fix-za.o
	${CPP} -o $@ fix-za.o ${LINKOPT}

lin-za: linear-solv-za.cpp
	${CPP} -o $@ linear-solv-za.cpp ${LINKOPT}

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
