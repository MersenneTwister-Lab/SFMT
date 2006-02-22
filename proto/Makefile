#for GNU make

#DMEXP = -DMEXP=521
%107:MEXP=107
%127:MEXP=127
%521:MEXP=521
#%607:MEXP=607
%19937:MEXP=19937
#%44497:MEXP=44497
#%86243:MEXP=86243
#%110503:MEXP=110503
#%132049:MEXP=132049
#%216091:MEXP=216091

#DDEBUG = -DDEBUG=1

DMEXP = -DMEXP=${MEXP}

CC = gcc -Wall -O9 -Wmissing-prototypes  $(DDEBUG) $(DMEXP) $(DPG)
CPP = g++ -Wall -O9 $(DDEBUG) $(DMEXP) $(DPG)

LINKOPT = -lntl -lgmp

test_gen521: sfmt.o test_gen.o
	${CC} -o $@ sfmt.o test_gen.o

test-gen5-521: sfmt5-cls.o test_gen128.o
	${CPP} -o $@ sfmt5-cls.o test_gen128.o ${LINKOPT}

test-gen128-521: sfmt5-cls.o test_gen128.o
	${CC} -o $@ sfmt5-cls.o test_gen128.o

param-search521: search.o sfmt.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search2-521: search.o sfmt2.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt2.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search3-521: search.o sfmt3.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt3.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search4-521: search.o sfmt4.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt4.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search4a-521: search.o sfmt4a.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt4a.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search4a128-521: search.o sfmt4a128.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt4a128.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search5-128-521: search2.o sfmt5-cls.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search2.o sfmt5-cls.o mt19937ar.o util.o debug.o ${LINKOPT}

search-dist521: search-dist.o sfmt5-cls.o mt19937ar.o util.o shortbase.o
	${CPP} -o $@ search-dist.o sfmt5-cls.o mt19937ar.o util.o shortbase.o \
		${LINKOPT}

param-search4a-19937: search.o sfmt4a.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt4a.o mt19937ar.o util.o debug.o ${LINKOPT}

shortest-base521: chk-shortest.o shortbase.o sfmt4a-cls.o util.o
	${CPP} -o $@ chk-shortest.o shortbase.o sfmt4a-cls.o util.o ${LINKOPT}

shortest-base5-521: chk-shortest.o shortbase.o sfmt5-cls.o util.o
	${CPP} -o $@ chk-shortest.o shortbase.o sfmt5-cls.o util.o ${LINKOPT}

test_bm521: sfmt.o test_bm.o util.o debug.o
	${CPP} -o $@ test_bm.o sfmt.o util.o debug.o ${LINKOPT}

test_bm128-521: sfmt5.o test_bm.o util.o debug.o
	${CPP} -o $@ test_bm.o sfmt5.o util.o debug.o ${LINKOPT}

test_gauss: test_gauss.cpp
	g++ -Wall -O0 -g -o $@ test_gauss.cpp ${LINKOPT} 

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} -c $<
