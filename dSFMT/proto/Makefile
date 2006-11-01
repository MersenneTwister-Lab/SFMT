#for GNU make

#DMEXP = -DMEXP=521
%107:MEXP=107
%127:MEXP=127
%521:MEXP=521
%607:MEXP=607
%1279:MEXP=1279
%2203:MEXP=2203
%2281:MEXP=2281
%19937:MEXP=19937
#%44497:MEXP=44497
#%86243:MEXP=86243
#%110503:MEXP=110503
#%132049:MEXP=132049
#%216091:MEXP=216091

#DDEBUG = -O0 -g

DMEXP = -DMEXP=${MEXP}

CC = gcc -Wall -O9 -Wmissing-prototypes  $(DDEBUG) $(DMEXP) $(DPG)
CPP = g++ -Wall -O9 $(DDEBUG) $(DMEXP) $(DPG)

LINKOPT = -lntl -lgmp

param-searchD01-521: search.o sfmtD01-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD01-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchD01-607: search.o sfmtD01-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD01-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchD01-1279: search.o sfmtD01-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD01-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchD01-2203: search.o sfmtD01-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD01-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchD02-521: search.o sfmtD02-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD02-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchD02-607: search.o sfmtD02-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD02-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchD02-1279: search.o sfmtD02-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD02-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchD03-521: search.o sfmtD03-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD03-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchD04-521: search.o sfmtD04-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD04-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchD05-607: search.o sfmtD05-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD05-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchD06-607: search.o sfmtD06-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD06-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchD07-607: search.o sfmtD07-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD07-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchD08-607: search.o sfmtD08-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD08-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchD09-607: search.o sfmtD09-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD09-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchD10-607: search.o sfmtD10-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD10-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

param-searchD11-607: search.o sfmtD11-st.o mt19937ar.o util.o dsfmt-util.o 
	${CPP} -o $@ search.o sfmtD11-st.o mt19937ar.o util.o dsfmt-util.o \
		${LINKOPT}

shortest-baseD01-607: chk-shortest.o shortbase128.o sfmtD01-st.o util.o \
			dsfmt-util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtD01-st.o util.o \
			dsfmt-util.o ${LINKOPT}

shortest-baseD01-521: chk-shortest.o shortbase128.o sfmtD01-st.o util.o \
			dsfmt-util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtD01-st.o util.o \
			dsfmt-util.o ${LINKOPT}

shortest-baseD04-521: chk-shortest.o shortbase128.o sfmtD04-st.o util.o \
			dsfmt-util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtD04-st.o util.o \
			dsfmt-util.o ${LINKOPT}

shortest-baseD05-607: chk-shortest.o shortbase128.o sfmtD05-st.o util.o \
			dsfmt-util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtD05-st.o util.o \
			dsfmt-util.o ${LINKOPT}

shortest-baseD06-607: chk-shortest.o shortbase128.o sfmtD06-st.o util.o \
			dsfmt-util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtD06-st.o util.o \
			dsfmt-util.o ${LINKOPT}

shortest-baseD07-607: chk-shortest.o shortbase128.o sfmtD07-st.o util.o \
			dsfmt-util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtD07-st.o util.o \
			dsfmt-util.o ${LINKOPT}

shortest-baseD08-607: chk-shortest.o shortbase128.o sfmtD08-st.o util.o \
			dsfmt-util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtD08-st.o util.o \
			dsfmt-util.o ${LINKOPT}

shortest-baseD09-607: chk-shortest.o shortbase128.o sfmtD09-st.o util.o \
			dsfmt-util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtD09-st.o util.o \
			dsfmt-util.o ${LINKOPT}

shortest-baseD10-607: chk-shortest.o shortbase128.o sfmtD10-st.o util.o \
			dsfmt-util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtD10-st.o util.o \
			dsfmt-util.o ${LINKOPT}

shortest-baseD11-607: chk-shortest.o shortbase128.o sfmtD11-st.o util.o \
			dsfmt-util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtD11-st.o util.o \
			dsfmt-util.o ${LINKOPT}

searchX78-lung607: search-lung.o sfmtX78-st.o mt19937ar.o util.o
	${CPP} -o $@ search-lung.o sfmtX78-st.o mt19937ar.o util.o ${LINKOPT}

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
