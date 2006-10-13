#for GNU make

#DMEXP = -DMEXP=521
%107:MEXP=107
%127:MEXP=127
%521:MEXP=521
%607:MEXP=607
%1279:MEXP=1279
%2281:MEXP=2281
%19937:MEXP=19937
#%44497:MEXP=44497
#%86243:MEXP=86243
#%110503:MEXP=110503
#%132049:MEXP=132049
#%216091:MEXP=216091

DDEBUG = -O0 -g

DMEXP = -DMEXP=${MEXP}

CC = gcc -Wall -O9 -Wmissing-prototypes  $(DDEBUG) $(DMEXP) $(DPG)
CPP = g++ -Wall -O9 $(DDEBUG) $(DMEXP) $(DPG)

LINKOPT = -lntl -lgmp

param-searchD01-607: search.o sfmtD01-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD01-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

shortest-baseD01-607: chk-shortest.o shortbase128.o sfmtD01-st.o util.o \
			dsfmt-util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtD01-st.o util.o \
			dsfmt-util.o ${LINKOPT}

searchX78-lung607: search-lung.o sfmtX78-st.o mt19937ar.o util.o
	${CPP} -o $@ search-lung.o sfmtX78-st.o mt19937ar.o util.o ${LINKOPT}

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
