#for GNU make

OPT = -O9 -finline-functions -fomit-frame-pointer
STD = -Wall -std=c89 -pedantic -Wmissing-prototypes
# for standard unix
CC = gcc $(OPT) $(STD) $(DDEBUG) 
# for power PC
#CC = gcc $(OPT) $(STD) -arch ppc  $(DDEBUG) 
# for Pentium M
#CC = gcc $(OPT) $(STD) -march=prescott $(DDEBUG)
#CC = icl /Wcheck /O3 /QxB /Qc99 /Qprefetch /Fa
# for Athlon
#CC = gcc $(OPT) $(STD) -march=nocona $(DDEBUG) 

sfmt-std: test.o sfmt.o
	${CC} -o $@ test.o sfmt.o

sfmt-alti: test.o sfmt-alti.o
	${CC} -o $@ test.o sfmt-alti.o

sfmt-sse2: test.o sfmt-sse2.o
	${CC} -o $@ test.o sfmt-sse2.o

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
