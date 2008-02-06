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

#DDEBUG = -O0 -g

DMEXP = -DMEXP=${MEXP}

CC = gcc -Wall -O9 -Wmissing-prototypes -I../util -L../util \
	$(DDEBUG) $(DMEXP) $(DPG)
CPP = g++ -Wall -O9  -I../util -L../util $(DDEBUG) $(DMEXP) $(DPG)

LINKOPT = -ldsfmt -lntl -lgmp

param-searchDPz3-521: sfmtDPz3.o search.o
	${CPP} -o $@ sfmtDPz3.o search.o ${LINKOPT}

param-searchDPz4-521: sfmtDPz4.o search.o
	${CPP} -o $@ sfmtDPz4.o search.o ${LINKOPT}

param-searchDPz5-521: sfmtDPz5.o search.o
	${CPP} -o $@ sfmtDPz5.o search.o ${LINKOPT}

param-searchDPz6-521: sfmtDPz6.o search.o
	${CPP} -o $@ sfmtDPz6.o search.o ${LINKOPT}

param-searchDPz7-521: sfmtDPz7.o search.o
	${CPP} -o $@ sfmtDPz7.o search.o ${LINKOPT}

param-searchDPz7-1279: sfmtDPz7.cpp search.o
	${CPP} -o $@ sfmtDPz7.cpp search.o ${LINKOPT}

param-searchDPz8-521: sfmtDPz8.o search.o
	${CPP} -o $@ sfmtDPz8.o search.o ${LINKOPT}

param-searchDPz8-1279: sfmtDPz8.o search.o
	${CPP} -o $@ sfmtDPz8.o search.o ${LINKOPT}

param-searchDPz9-521: sfmtDPz9.cpp search.o
	${CPP} -o $@ sfmtDPz9.cpp search.o ${LINKOPT}

param-searchDPz9-1279: sfmtDPz9.cpp search.o
	${CPP} -o $@ sfmtDPz9.cpp search.o ${LINKOPT}

param-searchDPza-521: sfmtDPza.cpp search.o
	${CPP} -o $@ sfmtDPza.cpp search.o ${LINKOPT}

param-searchDPzb-521: sfmtDPzb.cpp search.o
	${CPP} -o $@ sfmtDPzb.cpp search.o ${LINKOPT}

param-searchDPza-1279: sfmtDPza.cpp search.o
	${CPP} -o $@ sfmtDPza.cpp search.o ${LINKOPT}

param-searchDPzb-1279: sfmtDPzb.cpp search.o
	${CPP} -o $@ sfmtDPzb.cpp search.o ${LINKOPT}

shortest-baseDPz3-521: sfmtDPz3.o chk-shortest.o
	${CPP} -o $@ sfmtDPz3.o chk-shortest.o ${LINKOPT}

shortest-baseDPz4-521: sfmtDPz4.o chk-shortest.o
	${CPP} -o $@ sfmtDPz4.o chk-shortest.o ${LINKOPT}

shortest-baseDPz5-521: sfmtDPz5.o chk-shortest.o
	${CPP} -o $@ sfmtDPz5.o chk-shortest.o ${LINKOPT}

shortest-baseDPz6-521: sfmtDPz6.o chk-shortest.o
	${CPP} -o $@ sfmtDPz6.o chk-shortest.o ${LINKOPT}

shortest-baseDPz7-521: sfmtDPz7.o chk-shortest.o
	${CPP} -o $@ sfmtDPz7.o chk-shortest.o ${LINKOPT}

shortest-baseDPz8-521: sfmtDPz8.o chk-shortest.o
	${CPP} -o $@ sfmtDPz8.o chk-shortest.o ${LINKOPT}

shortest-baseDPz9-521: sfmtDPz9.cpp chk-shortest.o
	${CPP} -o $@ sfmtDPz9.cpp chk-shortest.o ${LINKOPT}

shortest-baseDPza-521: sfmtDPza.cpp chk-shortest.o
	${CPP} -o $@ sfmtDPza.cpp chk-shortest.o ${LINKOPT}

shortest-baseDPzb-521: sfmtDPzb.cpp chk-shortest.o
	${CPP} -o $@ sfmtDPzb.cpp chk-shortest.o ${LINKOPT}

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
