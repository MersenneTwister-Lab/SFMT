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

param-search521: search.o sfmt.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt.o mt19937ar.o util.o debug.o ${LINKOPT}

search-dist521: search-dist.o sfmt5-cls.o mt19937ar.o util.o shortbase.o
	${CPP} -o $@ search-dist.o sfmt5-cls.o mt19937ar.o util.o shortbase.o \
		${LINKOPT}

shortest-base521: chk-shortest.o shortbase.o sfmt4a-cls.o util.o
	${CPP} -o $@ chk-shortest.o shortbase.o sfmt4a-cls.o util.o ${LINKOPT}

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} -c $<
