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

CC = gcc -Wall -O9 -Wmissing-prototypes  $(DDEBUG) $(DMEXP) $(DPG)
CPP = g++ -Wall -O9 $(DDEBUG) $(DMEXP) $(DPG)

LINKOPT = -lntl -lgmp

test_gen521: sfmt.o test_gen.o
	${CC} -o $@ sfmt.o test_gen.o

test_gen5_all521: sfmt5.o test_gen.o
	${CC} -o $@ sfmt5.o test_gen.o

test_gen5_seq521: sfmt5-seq.o test_gen.o
	${CC} -o $@ sfmt5-seq.o test_gen.o

test_gen5_cls521: sfmt5-cls.o test_gen_cls.o
	${CPP} -o $@ sfmt5-cls.o test_gen_cls.o ${LINKOPT}

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

param-search6-128-521: search.o sfmt6-128.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt6-128.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search6-32-521: search.o sfmt6.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt6.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search6a-128-521: search.o sfmt6a-128.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt6a-128.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search6x521: search.o sfmt6x.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt6x.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search6y521: search.o sfmt6y.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt6y.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search9-521: search.o sfmt9.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt9.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search7-521: search.o sfmt7.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt7.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search7x-521: search.o sfmt7x.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt7x.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search7x1-521: search.o sfmt7x1.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt7x1.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search7x1b-521: search.o sfmt7x1b.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt7x1b.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search7x2-521: search.o sfmt7x2.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt7x2.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search7x3-521: search.o sfmt7x3.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt7x3.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search7x4-521: search.o sfmt7x4.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt7x4.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search7x5-521: search.o sfmt7x5.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt7x5.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search7x6-521: search.o sfmt7x6.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt7x6.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search7x7-521: search.o sfmt7x7.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt7x7.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search7x8-521: search.o sfmt7x8.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt7x8.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search7y-521: search.o sfmt7y.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt7y.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search8-521: search.o sfmt8.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt8.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search8x-521: search.o sfmt8x.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt8x.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search8y-521: search.o sfmt8y.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt8y.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search8z-521: search.o sfmt8z.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt8z.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search8v-521: search.o sfmt8v.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt8v.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search8t-521: search.o sfmt8t.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt8t.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search8p1-521: search.o sfmt8p1.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt8p1.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search8q1-521: search.o sfmt8q1.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt8q1.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search8q1-607: search.o sfmt8q1.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt8q1.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search8q2-521: search.o sfmt8q2.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt8q2.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search88q1-521: search.o sfmt88q1.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt88q1.o mt19937ar.o util.o debug.o ${LINKOPT}

param-search88q1-607: search.o sfmt88q1.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt88q1.o mt19937ar.o util.o debug.o ${LINKOPT}

param-searchX1-521: search.o sfmtX1.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmtX1.o mt19937ar.o util.o debug.o ${LINKOPT}

param-searchX1-607: search.o sfmtX1.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmtX1.o mt19937ar.o util.o debug.o ${LINKOPT}

param-searchX2-607: search.o sfmtX2.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmtX2.o mt19937ar.o util.o debug.o ${LINKOPT}

param-searchX3-607: search.o sfmtX3.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmtX3.o mt19937ar.o util.o debug.o ${LINKOPT}

search9-all521: search9-all.o sfmt9-st.o mt19937ar.o util.o shortbase9.o
	${CPP} -o $@ search9-all.o shortbase9.o sfmt9-st.o mt19937ar.o util.o  \
		${LINKOPT}

search9b-all521: search9-all.o sfmt9b-st.o mt19937ar.o util.o shortbase9.o
	${CPP} -o $@ search9-all.o shortbase9.o sfmt9b-st.o mt19937ar.o util.o \
		${LINKOPT}

search-dist521: search-dist.o sfmt5-cls.o mt19937ar.o util.o shortbase.o
	${CPP} -o $@ search-dist.o sfmt5-cls.o mt19937ar.o util.o shortbase.o \
		${LINKOPT}

param-search4a-19937: search.o sfmt4a.o mt19937ar.o util.o debug.o
	${CPP} -o $@ search.o sfmt4a.o mt19937ar.o util.o debug.o ${LINKOPT}

shortest-base521: chk-shortest.o shortbase.o sfmt4a-cls.o util.o
	${CPP} -o $@ chk-shortest.o shortbase.o sfmt4a-cls.o util.o ${LINKOPT}

shortest-base5-521: chk-shortest.o shortbase.o sfmt5-cls.o util.o
	${CPP} -o $@ chk-shortest.o shortbase.o sfmt5-cls.o util.o ${LINKOPT}

shortest-baseX1-521: chk-shortest.o shortbase128.o sfmtX1-st.o util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtX1-st.o util.o ${LINKOPT}

shortest-baseX1-607: chk-shortest.o shortbase128.o sfmtX1-st.o util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtX1-st.o util.o ${LINKOPT}

shortest-baseX2-607: chk-shortest.o shortbase128.o sfmtX2-st.o util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtX2-st.o util.o ${LINKOPT}

shortest-baseX3-607: chk-shortest.o shortbase128.o sfmtX3-st.o util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmtX3-st.o util.o ${LINKOPT}

shortest-base88q1-607: chk-shortest.o shortbase128.o sfmt88q1-st.o util.o
	${CPP} -o $@ chk-shortest.o shortbase128.o sfmt88q1-st.o util.o ${LINKOPT}

shortest32-base5-521: chk-shortest32.o shortbase.o sfmt5-cls.o util.o
	${CPP} -o $@ chk-shortest32.o shortbase.o sfmt5-cls.o util.o \
		${LINKOPT}

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

clean:
	rm -f *.o
