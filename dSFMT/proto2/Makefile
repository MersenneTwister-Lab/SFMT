#for GNU make

#DMEXP = -DMEXP=521
%521:MEXP=521
%607:MEXP=607
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

#DDEBUG = -O0 -g -UNDEBUG

DMEXP = -DMEXP=${MEXP}

CC = gcc -Wall -O9 -Wmissing-prototypes -I../util -L../util \
	$(DDEBUG) $(DMEXP) $(DPG)
CPP = g++ -Wall -O9  -DNDEBUG -I../util -L../util $(DDEBUG) $(DMEXP) $(DPG)

LINKOPT = -ldsfmt -lntl -lgmp

param-searchDPza-521: sfmtDPza.cpp search.o
	${CPP} -o $@ sfmtDPza.cpp search.o ${LINKOPT}

param-searchDPza-1279: sfmtDPza.cpp search.o
	${CPP} -o $@ sfmtDPza.cpp search.o ${LINKOPT}

shortest-baseDPza-521: sfmtDPza.cpp chk-shortest.o
	${CPP} -o $@ sfmtDPza.cpp chk-shortest.o ${LINKOPT}

shortest-baseDPza-1279: sfmtDPza.cpp chk-shortest.o
	${CPP} -o $@ sfmtDPza.cpp chk-shortest.o ${LINKOPT}

chk-charac-DPza521: sfmtDPza.cpp chk-charac.cpp
	${CPP} -o $@ sfmtDPza.cpp chk-charac.cpp ${LINKOPT}

fill-paramsDPza-521: sfmtDPza.cpp fill-params.cpp
	${CPP} -o $@ sfmtDPza.cpp fill-params.cpp ${LINKOPT}

chi-zero-521: sfmtDPza.cpp chi-zero.o
	${CPP} -o $@ sfmtDPza.cpp chi-zero.o ${LINKOPT}

chi-zero-19937: sfmtDPza.cpp chi-zero.o
	${CPP} -o $@ sfmtDPza.cpp chi-zero.o ${LINKOPT}

get-charactorDPza-521: sfmtDPza.cpp get-charactor.cpp
	${CPP} -o $@ sfmtDPza.cpp get-charactor.cpp ${LINKOPT}

get-fixDPza-521: sfmtDPza.cpp get-fix.cpp
	${CPP} -o $@ sfmtDPza.cpp get-fix.cpp ${LINKOPT}

get-charactorDPza-1279: sfmtDPza.cpp get-charactor.cpp
	${CPP} -o $@ sfmtDPza.cpp get-charactor.cpp ${LINKOPT}

get-charactorDPza-19937: sfmtDPza.cpp get-charactor.cpp
	${CPP} -o $@ sfmtDPza.cpp get-charactor.cpp ${LINKOPT}

get-parity-vectorDPza-19937: sfmtDPza.cpp search-parity-vector.cpp
	${CPP} -o $@ sfmtDPza.cpp search-parity-vector.cpp ${LINKOPT}

get-parity-vectorDPza-521: sfmtDPza.cpp search-parity-vector.cpp
	${CPP} -o $@ sfmtDPza.cpp search-parity-vector.cpp ${LINKOPT}

get-parity-vectorDPza-1279: sfmtDPza.cpp search-parity-vector.cpp
	${CPP} -o $@ sfmtDPza.cpp search-parity-vector.cpp ${LINKOPT}

get-charactorDPza-11213: sfmtDPza.cpp get-charactor.cpp
	${CPP} -o $@ sfmtDPza.cpp get-charactor.cpp ${LINKOPT}

get-parity-vectorDPza-11213: sfmtDPza.cpp search-parity-vector.cpp
	${CPP} -o $@ sfmtDPza.cpp search-parity-vector.cpp ${LINKOPT}

test-parityDPza-11213: sfmtDPza.cpp test-parity.cpp
	${CPP} -o $@ sfmtDPza.cpp test-parity.cpp ${LINKOPT}

get-charactorDPza-4253: sfmtDPza.cpp get-charactor.cpp
	${CPP} -o $@ sfmtDPza.cpp get-charactor.cpp ${LINKOPT}

get-parity-vectorDPza-4253: sfmtDPza.cpp search-parity-vector.cpp
	${CPP} -o $@ sfmtDPza.cpp search-parity-vector.cpp ${LINKOPT}

test-parityDPza-4253: sfmtDPza.cpp test-parity.cpp
	${CPP} -o $@ sfmtDPza.cpp test-parity.cpp ${LINKOPT}

get-charactorDPza-2203: sfmtDPza.cpp get-charactor.cpp
	${CPP} -o $@ sfmtDPza.cpp get-charactor.cpp ${LINKOPT}

get-parity-vectorDPza-2203: sfmtDPza.cpp search-parity-vector.cpp
	${CPP} -o $@ sfmtDPza.cpp search-parity-vector.cpp ${LINKOPT}

test-parityDPza-2203: sfmtDPza.cpp test-parity.cpp
	${CPP} -o $@ sfmtDPza.cpp test-parity.cpp ${LINKOPT}

test-parityDPza-521: sfmtDPza.cpp test-parity.cpp
	${CPP} -o $@ sfmtDPza.cpp test-parity.cpp ${LINKOPT}

test-parityDPza-1279: sfmtDPza.cpp test-parity.cpp
	${CPP} -o $@ sfmtDPza.cpp test-parity.cpp ${LINKOPT}

test-parityDPza-19937: sfmtDPza.cpp test-parity.cpp
	${CPP} -o $@ sfmtDPza.cpp test-parity.cpp ${LINKOPT}

test-parityDPza-44497: sfmtDPza.cpp test-parity.cpp
	${CPP} -o $@ sfmtDPza.cpp test-parity.cpp ${LINKOPT}

test-genDPza-521: sfmtDPza.cpp test-gen.cpp
	${CPP} -o $@ sfmtDPza.cpp test-gen.cpp ${LINKOPT}

test-genDPza-1279: sfmtDPza.cpp test-gen.cpp
	${CPP} -o $@ sfmtDPza.cpp test-gen.cpp ${LINKOPT}

test-genDPza-2203: sfmtDPza.cpp test-gen.cpp
	${CPP} -o $@ sfmtDPza.cpp test-gen.cpp ${LINKOPT}

test-genDPza-4253: sfmtDPza.cpp test-gen.cpp
	${CPP} -o $@ sfmtDPza.cpp test-gen.cpp ${LINKOPT}

test-genDPza-11213: sfmtDPza.cpp test-gen.cpp
	${CPP} -o $@ sfmtDPza.cpp test-gen.cpp ${LINKOPT}

test-genDPza-19937: sfmtDPza.cpp test-gen.cpp
	${CPP} -o $@ sfmtDPza.cpp test-gen.cpp ${LINKOPT}

test-minpolyDPza-86243: sfmtDPza.cpp test-minpoly.o
	${CPP} -o $@ sfmtDPza.cpp test-minpoly.o ${LINKOPT}

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
