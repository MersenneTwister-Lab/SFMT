#for GNU make

#DDEBUG = -O0 -g

CC = gcc -Wall -O9 -Wmissing-prototypes -I/opt/local/include \
	-L. $(DDEBUG) $(DPG)
CPP = g++ -Wall -O9 -I/opt/local/include -L. $(DDEBUG) $(DPG)

LINKOPT = -ldsfmt -lntl -lgmp

param-searchDPz2-607: sfmtDPz2.o search.o libdsfmt.a
	${CPP} -o $@ sfmtDPz2.o search.o ${LINKOPT}

shortest-baseDPz2-607: sfmtDPz2.o chk-shortest.o libdsfmt.a
	${CPP} -o $@ sfmtDPz2.o chk-shortest.o ${LINKOPT}

libdsfmt.a: mt19937blk.o util.o dsfmt-util.o shortbase104.o
	ar -rLsv libdsfmt.a mt19937blk.o util.o dsfmt-util.o shortbase104.o 

test-util: util.cpp test-util.cpp
	${CPP} -o $@ util.cpp test-util.cpp ${LINKOPT}

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
