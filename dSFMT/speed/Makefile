#for GNU make
#DDEBUG = -O0 -g -DDEBUG

CC = gcc -Wall -O9 -faltivec -Wmissing-prototypes -Winline -maltivec -arch ppc  -DNDEBUG $(DDEBUG) $(DMEXP) $(DPG)
#CC = gcc -Wall -O9 -faltivec -Wmissing-prototypes -arch ppc  -fprefetch-loop-arrays $(DDEBUG) $(DMEXP) $(DPG)
CPP = g++ -Wall -fast $(DDEBUG) $(DMEXP) $(DPG)

LINKOPT = -lntl -lgmp

inline_all:sfmt4a_fastc_inline pmt_fastc_inline mt_fastc_inline

##### ALTIVEC ####
sfmtD01_alti: sfmtD01_alti.c test_time.c  paramsD01.h
	${CC} -o $@ sfmtD01_alti.c

#### NORMAL C ####
sfmtD01_fastc: sfmtD01_fastc.c test_time.c paramsD01.h
	${CC} -o $@ sfmtD01_fastc.c

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
