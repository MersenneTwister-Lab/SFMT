#for GNU make

WARN = -Wmissing-prototypes -Wall -Winline
OPT = -O1 -finline-functions -fomit-frame-pointer -DNDEBUG 
STD = -std=c89 -pedantic 
# for standard unix
#CC = gcc $(OPT) $(STD)
# for power PC
CC = gcc $(WARN) $(OPT) -faltivec -maltivec -arch ppc
# for Pentium M
#CC = gcc $(OPT) $(STD) -march=prescott
#CC = icl /Wcheck /O3 /QxB /Qc99 /Qprefetch /Fa
# for Athlon
#CC = gcc $(OPT) $(STD) -march=nocona

sfmt-std: test.o sfmt.o
	${CC} ${STD} -o $@ test.o sfmt.o

sfmt-alti32: test_alti32.c sfmt_alti.c Makefile
	${CC} -o $@ test_alti32.c

sfmt-sse2: test.o sfmt-sse2.o
	${CC} -o $@ test.o sfmt-sse2.o

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
