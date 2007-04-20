#SVN: $Id:$

test: test.c SFMT.o
	gcc -O3 $^ -o $@ 

#----------------------------rules-----------------------

SFMT.o: %.o: SFMT.c SFMT.h SFMT-params.h SFMT-sse2.c SFMT-std.c SFMT-alti.c
	gcc -O3 -DMEXP=19937 -c -msse2 $< -o $@

