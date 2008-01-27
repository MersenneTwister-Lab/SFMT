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

CC = gcc -Wall -O9 -Wmissing-prototypes $(DDEBUG) $(DMEXP) $(DPG)
CPP = g++ -Wall -O9 $(DDEBUG) $(DMEXP) $(DPG)

LINKOPT = -lntl -lgmp

param-searchDPz1-607: searchNL.o sfmtDPz1-st.o mt19937blk.o util.o \
		dsfmtNL-util.o 
	${CPP} -o $@ searchNL.o sfmtDPz1-st.o mt19937blk.o util.o \
		dsfmtNL-util.o ${LINKOPT}

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
