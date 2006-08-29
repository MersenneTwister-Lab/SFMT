#for GNU make

#DMEXP = -DMEXP=521
%107:MEXP=107
%127:MEXP=127
%521:MEXP=521
%607:MEXP=607
%19937:MEXP=19937
#%44497:MEXP=44497
#%86243:MEXP=86243
#%110503:MEXP=110503
#%132049:MEXP=132049
#%216091:MEXP=216091

#DDEBUG = -DDEBUG=1

DMEXP = -DMEXP=${MEXP}
OPTI = -O0 -g

CC = gcc -Wall $(OPTI) -Wmissing-prototypes  $(DDEBUG) $(DMEXP) $(DPG)
CPP = g++ -Wall $(OPTI) $(DDEBUG) $(DMEXP) $(DPG)

LINKOPT = -lntl -lgmp

test_gen521: sfmt.o test_gen.o
	${CC} -o $@ sfmt.o test_gen.o

#test-gen-st521: sfmt-st.o test-gen-st.o
#	${CC} -o $@ sfmt-st.o test-gen-st.o

test-gen-st521: sfmt-st.o test-gen-st-cpp.o
	${CPP} -o $@ sfmt-st.o test-gen-st-cpp.o ${LINKOPT}

param-search521: search128.o sfmt-st.o mt19937ar.o util.o shortbase128.o \
		sfmt-util.o
	${CPP} -o $@ search128.o sfmt-st.o mt19937ar.o util.o shortbase128.o \
		sfmt-util.o ${LINKOPT}

param88q1-search521: search128.o sfmt88q1-st.o mt19937ar.o util.o \
	shortbase128.o sfmt-util.o
	${CPP} -o $@ search128.o sfmt88q1-st.o mt19937ar.o util.o \
		shortbase128.o sfmt-util.o ${LINKOPT}

param32-search521: search32.o sfmt-st.o mt19937ar.o util.o
	${CPP} -o $@ search32.o sfmt-st.o mt19937ar.o util.o ${LINKOPT}

check-param521: chk-irrs-param.o sfmt.o util.o
	${CPP} -o $@ chk-irrs-param.o sfmt.o util.o ${LINKOPT}

search-dist521: search-dist.o sfmt-st.o mt19937ar.o util.o shortbase.o
	${CPP} -o $@ search-dist.o sfmt-st.o mt19937ar.o util.o shortbase.o \
		${LINKOPT}

shortest128-base521: chk-shortest.o shortbase128.o sfmt-st.o sfmt-util.o util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmt-st.o sfmt-util.o \
		util.o ${LINKOPT}

shortest128-88q1-base521: chk-shortest.o shortbase128.o sfmt-st88q1.o util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmt-st88q1.o util.o ${LINKOPT}

shortestX7-base607: chk-shortest.o shortbase128.o sfmtX7-st.o util.o sfmt-util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtX7-st.o util.o \
	sfmt-util.o ${LINKOPT}

shortestX78-base607: chk-shortest.o shortbase128.o sfmtX78-st.o util.o \
		sfmt-util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtX78-st.o util.o \
		sfmt-util.o ${LINKOPT}

ht-shortest128-base521: ht-chk-shortest.o ht-shortbase128.o ht-rnd-st.o util.o
	${CPP} -o $@ ht-chk-shortest.o ht-shortbase128.o ht-rnd-st.o \
		util.o ${LINKOPT}

search-initial-lung521: search-lung.o sfmt-st.o mt19937ar.o util.o
	${CPP} -o $@ search-lung.o sfmt-st.o mt19937ar.o util.o ${LINKOPT}

search-initial-lung19937: search-lung.o sfmt-st.o mt19937ar.o util.o
	${CPP} -o $@ search-lung.o sfmt-st.o mt19937ar.o util.o ${LINKOPT}

search-crypt-lung19937: search-lung.o sfmt-st88q1.o mt19937ar.o util.o
	${CPP} -o $@ search-lung.o sfmt-st88q1.o mt19937ar.o util.o ${LINKOPT}

searchX78-lung19937: search-lung.o sfmtX78-st.o mt19937ar.o util.o
	${CPP} -o $@ search-lung.o sfmtX78-st.o mt19937ar.o util.o ${LINKOPT}

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} -c $<

clean:
	rm -f *.o
