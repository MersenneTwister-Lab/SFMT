#for GNU make

#DDEBUG = -O0 -g -DDEBUG=1

# uncomment next line if you installed NTL with gf2x
LIBGF2X = -lgf2x
# uncomment next line if you installed NTL with gmp
#LIBGMP = -lgmp

DEFINES = -D__STDC_CONSTANT_MACROS=1
CPP = g++ -Wall -Wextra -O3 -I../include -msse3 $(DEFINES) $(DDEBUG)

LINKOPT = -lntl -lcrypto $(LIBGF2X) $(LIBGMP)

calc-characteristic: calc-characteristic.o SFMText.hpp
	${CPP} -o $@ calc-characteristic.o ${LINKOPT}

.cpp.o:
	${CPP} -c $<

clean:
	rm -f *.o *~
