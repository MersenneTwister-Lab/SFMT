#for GNU make

CC = gcc -Wall -O9 -faltivec -Wmissing-prototypes -arch ppc  $(DDEBUG) $(DMEXP) $(DPG)
#CC = gcc -Wall -O9 -faltivec -Wmissing-prototypes -arch ppc  -fprefetch-loop-arrays $(DDEBUG) $(DMEXP) $(DPG)
CPP = g++ -Wall -fast $(DDEBUG) $(DMEXP) $(DPG)

LINKOPT = -lntl -lgmp

inline_all:sfmt4a_fastc_inline pmt_fastc_inline mt_fastc_inline

##### ALTIVEC ####
sfmt4a_alti: sfmt4a_alti.o test_time.o
	${CC} -o $@ sfmt4a_alti.o test_time.o

sfmt4a_alti_inline: test_time_inline.c sfmt4a_alti_inline.h
	${CC} -DRANDOM=sfmt4a_alti_inline.h -o $@ test_time_inline.c

sfmt4a_alti_inline2: test_time2_inline.c sfmt4a_alti_inline.h
	${CC} -DRANDOM=sfmt4a_alti_inline.h -o $@ test_time2_inline.c

sfmt4b_alti_inline: test_time_inline.c sfmt4b_alti_inline.h
	${CC} -DRANDOM=sfmt4b_alti_inline.h -o $@ test_time_inline.c

sfmt4b_alti_inline2: test_time2_inline.c sfmt4b_alti_inline.h
	${CC} -DRANDOM=sfmt4b_alti_inline.h -o $@ test_time2_inline.c

sfmt5_alti_inline: test_time_inline.c sfmt5_alti_inline.h
	${CC} -DRANDOM=sfmt5_alti_inline.h -o $@ test_time_inline.c

sfmt5_alti_inline2: test_time2_inline.c sfmt5_alti_inline.h
	${CC} -DRANDOM=sfmt5_alti_inline.h -o $@ test_time2_inline.c

sfmt6_alti_inline: test_time_inline.c sfmt6_alti_inline.h
	${CC} -DRANDOM=sfmt6_alti_inline.h -o $@ test_time_inline.c

sfmt6_alti2_inline: test_time_inline.c sfmt6_alti2_inline.h
	${CC} -DRANDOM=sfmt6_alti2_inline.h -o $@ test_time_inline.c

sfmt6_alti_inline2: test_time2_inline.c sfmt6_alti_inline.h
	${CC} -DRANDOM=sfmt6_alti_inline.h -o $@ test_time2_inline.c

sfmt7_alti_inline: test_time_inline.c sfmt7_alti_inline.c
	${CC} -DTICK=1 -o $@ sfmt7_alti_inline.c

sfmt7x1_alti_inline: test_time_inline.c sfmt7x1_alti_inline.c
	${CC} -DTICK=1 -o $@ sfmt7x1_alti_inline.c

sfmt7x1_alti_inline2: test_time_inline.c sfmt7x1_alti_inline.c
	${CC} -o $@ sfmt7x1_alti_inline.c

sfmt7x1b_alti_inline: test_time_inline.c sfmt7x1b_alti_inline.c
	${CC} -DTICK=1 -o $@ sfmt7x1b_alti_inline.c

sfmt7x1b_alti_inline2: test_time_inline.c sfmt7x1b_alti_inline.c
	${CC} -o $@ sfmt7x1b_alti_inline.c

sfmt7x2_alti_inline: test_time_inline.c sfmt7x2_alti_inline.c
	${CC} -DTICK=1 -o $@ sfmt7x2_alti_inline.c

sfmt7x2_alti_inline2: test_time_inline.c sfmt7x2_alti_inline.c
	${CC} -o $@ sfmt7x2_alti_inline.c

sfmt7x4_alti_inline: test_time_inline.c sfmt7x4_alti_inline.c
	${CC} -DTICK=1 -o $@ sfmt7x2_alti_inline.c

sfmt7x4_alti_inline2: test_time_inline.c sfmt7x4_alti_inline.c
	${CC} -o $@ sfmt7x2_alti_inline.c

sfmt7x7_alti_inline: test_time_inline.c sfmt7x7_alti_inline.c
	${CC} -DTICK=1 -o $@ sfmt7x7_alti_inline.c

sfmt7x7_alti_inline2: test_time_inline.c sfmt7x7_alti_inline.c
	${CC} -o $@ sfmt7x7_alti_inline.c

sfmt7x8_alti_inline: test_time_inline.c sfmt7x8_alti_inline.c
	${CC} -DTICK=1 -o $@ sfmt7x2_alti_inline.c

sfmt7x8_alti_inline2: test_time_inline.c sfmt7x8_alti_inline.c
	${CC} -o $@ sfmt7x2_alti_inline.c

sfmtX2_alti_inline: test_time_inline.c sfmtX2_alti_inline.c
	${CC} -DTICK=1 -o $@ sfmtX2_alti_inline.c

sfmtX2_alti_inline2: test_time_inline.c sfmtX2_alti_inline.c
	${CC} -o $@ sfmtX2_alti_inline.c

sfmtX7_alti_inline: test_time_inline.c sfmtX7_alti_inline.c
	${CC} -DTICK=1 -o $@ sfmtX7_alti_inline.c

sfmtX7_alti_inline2: test_time_inline.c sfmtX7_alti_inline.c
	${CC} -o $@ sfmtX7_alti_inline.c

sfmtX74_alti_inline: test_time_inline.c sfmtX74_alti_inline.c
	${CC} -DTICK=1 -o $@ sfmtX74_alti_inline.c

sfmtX74_alti_inline2: test_time2_inline.c sfmtX74_alti_inline.c
	${CC} -o $@ sfmtX74_alti_inline.c

sfmtX75_alti_inline: test_time_inline.c sfmtX75_alti_inline.c
	${CC} -DTICK=1 -o $@ sfmtX75_alti_inline.c

sfmtX75_alti_inline2: test_time2_inline.c sfmtX75_alti_inline.c
	${CC} -o $@ sfmtX75_alti_inline.c

sfmtX78_alti_inline: test_time_inline.c sfmtX78_alti_inline.c
	${CC} -DTICK=1 -o $@ sfmtX78_alti_inline.c

sfmtX78_alti_inline2: test_time2_inline.c sfmtX78_alti_inline.c
	${CC} -o $@ sfmtX78_alti_inline.c

sfmtX78_alti_funcall2: test_time2_funcall.o sfmtX78_alti_funcall.o
	${CC} -o $@ sfmtX78_alti_funcall.o test_time2_funcall.o

sfmt4a_alti44497_inline: test_time_inline.c sfmt4a_alti44497_inline.h
	${CC} -DRANDOM=sfmt4a_alti44497_inline.h -o $@ test_time_inline.c

sfmt9_alti_inline: test_time_inline.c sfmt9_alti_inline.h
	${CC} -DRANDOM=sfmt9_alti_inline.h -o $@ test_time_inline.c

sfmt9_alti_inline2: test_time2_inline.c sfmt9_alti_inline.h
	${CC} -DRANDOM=sfmt9_alti_inline.h -o $@ test_time2_inline.c

sfmt9b_alti_inline: test_time_inline.c sfmt9b_alti_inline.h
	${CC} -DRANDOM=sfmt9b_alti_inline.h -o $@ test_time_inline.c

sfmt9b_alti_inline2: test_time2_inline.c sfmt9b_alti_inline.h
	${CC} -DRANDOM=sfmt9b_alti_inline.h -o $@ test_time2_inline.c

mt19937ar_alti_inline: test_time_inline.c mt19937ar_alti_inline.c
	${CC} -DTICK=1 -o $@ mt19937ar_alti_inline.c

mt19937ar_alti_inline2: test_time2_inline.c mt19937ar_alti_inline.c
	${CC} -o $@ mt19937ar_alti_inline.c 

mt19937ar_alti_funcall2: test_time2_funcall.o mt19937_alti_funcall.o
	${CC} -o $@ mt19937_alti_funcall.o test_time2_funcall.o

testalti: test_alti.o
	${CC} -o $@ test_alti.o

#### NORMAL C ####
sfmt_fastc: sfmt.o test_time.o
	${CC} -o $@ sfmt.o test_time.o

sfmt2_fastc: sfmt2.o test_time.o
	${CC} -o $@ sfmt2.o test_time.o

sfmt4a_fastc: sfmt4a.o test_time.o
	${CC} -o $@ sfmt4a.o test_time.o

sfmt4a_fastc_inline: sfmt4a_inline.h test_time_inline.c
	${CC} -DRANDOM=sfmt4a_inline.h -o $@ test_time_inline.c

sfmt4a_fastc44497_inline: sfmt4a_44497_inline.h test_time_inline.c
	${CC} -DRANDOM=sfmt4a_44497_inline.h -o $@ test_time_inline.c

sfmt4a_fastc_inlineb: sfmt4a_inlineb.h test_time_inline.c
	${CC} -DRANDOM=sfmt4a_inlineb.h -o $@ test_time_inline.c

sfmt4a_fastc_inlinec: sfmt4a_inlinec.h test_time_inline.c
	${CC} -DRANDOM=sfmt4a_inlinec.h -o $@ test_time_inline.c

sfmtX78_fastc_inline: sfmtX78_fastc_inline.c test_time_inline.c
	${CC} -DTICK=1 -o $@ sfmtX78_fastc_inline.c

sfmtX78_fastc_inline2: sfmtX78_fastc_inline.c test_time2_inline.c
	${CC} -o $@ sfmtX78_fastc_inline.c

sfmtX78_fastc_funcall2: sfmtX78_fastc_funcall.o test_time2_funcall.o
	${CC} -o $@ sfmtX78_fastc_funcall.o test_time2_funcall.o

sfmt4a_std: sfmt4a_std.o test_time.o
	${CC} -o $@ sfmt4a_std.o test_time.o

sfmt6_std: sfmt6_std.o test_time.o
	${CC} -o $@ sfmt6_std.o test_time.o

sfmt7_std: sfmt7_std.o test_time.o
	${CC} -o $@ sfmt7_std.o test_time.o

sfmtX7_std: sfmtX7_std.o test_time.o
	${CC} -o $@ sfmtX7_std.o test_time.o

pmt_fastc: ht-rnd.o test_time.o
	${CC} -o $@ ht-rnd.o test_time.o

pmt_fastc_inline: test_time_inline.c pmt_fastc_inline.h
	${CC} -DRANDOM=pmt_fastc_inline.h -o $@ test_time_inline.c

pmt_fastc_inline2: test_time2_inline.c pmt_fastc_inline.h
	${CC} -DRANDOM=pmt_fastc_inline.h -o $@ test_time2_inline.c

mt_fastc: mt19937ar.o test_time.o
	${CC} -o $@ mt19937ar.o test_time.o

mt_fastc_inline: mt19937ar_inline.h test_time_inline.c
	${CC} -DTICK=1 -o $@ mt19937ar_inline.c

mt_fastc_inline2: mt19937ar_inline.c test_time2_inline.c
	${CC} -o $@ mt19937ar_inline.c

mt_fastc_funcall2: mt19937ar_funcall.o test_time2_funcall.o
	${CC} -o $@ mt19937ar_funcall.o test_time2_funcall.o

well_fastc_inline2: well19937c.c test_time2_inline.c
	${CC} -o $@ well19937c.c

.c.o:
	${CC} -c $<

clean:
	rm -f *.o *~
