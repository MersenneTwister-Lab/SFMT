#for GNU make

OPT = -O9 -finline-functions -fomit-frame-pointer -DNDEBUG
# for power PC
CC = gcc -Wall $(OPT) -Wmissing-prototypes -arch ppc  $(DDEBUG)
# for Pentium M
#CC = gcc -Wall $(OPT) -Wmissing-prototypes -march=prescott $(DDEBUG)
#CC = icl /Wcheck /O3 /QxB /Qc99 /Qprefetch /Fa
# for Athlon
#CC = gcc -Wall $(OPT) -Wmissing-prototypes -march=nocona $(DDEBUG)

#CC = gcc -Wall -O9 -faltivec -Wmissing-prototypes -arch ppc  -fprefetch-loop-arrays $(DDEBUG) $(DMEXP) $(DPG)

all:well_fastc_inline2 rand_fastc_inline2 rand48_fastc_inline2 \
	mrg_fastc_inline2 random256g2_fastc_inline2 xor3_fastc_inline2

well_fastc_inline2: well19937c_fast.c test_time2_inline.c
	${CC} -o $@ well19937c_fast.c

well1024_inline2: well1024a.c test_time2_inline.c
	${CC} -o $@ well1024a.c

well1024_funcall: well1024a.c test_time2_funcall.o
	${CC} -o $@ -DFUNCALL well1024a.c test_time2_funcall.o

well1024_double: well1024a_double.c test_time2_double.o
	${CC} -o $@ -DFUNCALL well1024a_double.c test_time2_double.o

mt19937ar_inline2: mt19937ar_inline.c test_time2_inline.c
	${CC} -o $@ mt19937ar_inline.c

mt19937ar_funcall: mt19937ar_inline.c test_time2_funcall.o
	${CC} -o $@ -DFUNCALL mt19937ar_inline.c test_time2_funcall.o

mt19937ar_double: mt19937ar_double.c test_time2_double.o
	${CC} -o $@ -DFUNCALL mt19937ar_double.c test_time2_double.o

rand_fastc_inline2: rand.c test_time2_inline.c
	${CC} -o $@ rand.c

rand48_fastc_inline2: rand48.c test_time2_inline.c
	${CC} -o $@ rand48.c

mrg_fastc_inline2: mrg.c test_time2_inline.c
	${CC} -o $@ mrg.c

random256g2_fastc_inline2: random256g2.c test_time2_inline.c
	${CC} -o $@ random256g2.c

xor3_fastc_inline2: xor3.c test_time2_inline.c
	${CC} -o $@ xor3.c

xor3tbl_fastc_inline2: xor3tbl.c test_time2_inline.c
	${CC} -o $@ xor3.c

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
