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

DDEBUG = -O0 -g

DMEXP = -DMEXP=${MEXP}

CC = gcc -Wall -O9 -Wmissing-prototypes -I/opt/local/include \
	$(DDEBUG) $(DMEXP) $(DPG)
CPP = g++ -Wall -O9 -I/opt/local/include $(DDEBUG) $(DMEXP) $(DPG)

LINKOPT = -ldsfmt -lntl -lgmp

param-searchDPz2-607: sfmtDPz2.o search.o libdsfmt.a
	${CPP} -o $@ sfmtDPz2.o search.o ${LINKOPT}

shortest-baseDPz2-607: sfmtDPz2.o chk-shortest.o libdsfmt.a
	${CPP} -o $@ sfmtDPz2.o chk-shortest.o ${LINKOPT}

libdsfmt.a: mt19937blk.o util.o dsfmt-util.o shortbase104.o
	ar -rsv libdsfmt.a mt19937blk.o util.o dsfmt-util.o shortbase104.o 

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
