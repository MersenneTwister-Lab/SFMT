#for GNU make

OPT = -O9 -finline-functions -fomit-frame-pointer
# for power PC
#CC = gcc -Wall $(OPT) -Wmissing-prototypes -arch ppc  $(DDEBUG) 
# for Pentium M
#CC = gcc -Wall $(OPT) -Wmissing-prototypes -march=prescott $(DDEBUG)
CC = icl /Wcheck /O3 /QxB /Qc99 /Qprefetch /Fa
# for Athlon
#CC = gcc -Wall $(OPT) -Wmissing-prototypes -march=nocona $(DDEBUG) 

#CC = gcc -Wall -O9 -faltivec -Wmissing-prototypes -arch ppc  -fprefetch-loop-arrays $(DDEBUG) $(DMEXP) $(DPG)

all:well_fastc_inline2 rand_fastc_inline2 rand48_fastc_inline2 \
	mrg_fastc_inline2 random256g2_fastc_inline2 xor3_fastc_inline2

well_fastc_inline2: well19937c.c test_time2_inline.c
	${CC} -o $@ well19937c.c

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

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
