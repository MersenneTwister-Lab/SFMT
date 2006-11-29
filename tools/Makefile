#for GNU make

CC = gcc -Wall -O9 -Wmissing-prototypes  $(DDEBUG) $(DPG)
CPP = g++ -Wall -O9 $(DDEBUG) $(DPG)

LINKOPT = -lntl -lgmp

kiyaku: kiyaku.o
	${CPP} -o $@ kiyaku.o ${LINKOPT}

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
