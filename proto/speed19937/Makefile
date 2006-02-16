#for GNU make

CC = gcc -Wall -O9 -faltivec -Wmissing-prototypes -arch ppc  $(DDEBUG) $(DMEXP) $(DPG)
CPP = g++ -Wall -fast $(DDEBUG) $(DMEXP) $(DPG)

LINKOPT = -lntl -lgmp

inline_all:sfmt4a_fastc_inline pmt_fastc_inline mt_fastc_inline

##### ALTIVEC ####
sfmt4a_sse2_inline: sfmt4a_sse2_inline.h test_time_inline.c
	${CC} -DRANDOM=sfmt4a_sse2_inline.h -o $@ test_time_inline.c

sfmt4a_sse2_inline2: sfmt4a_sse2_inline.h test_time2_inline.c
	${CC} -DRANDOM=sfmt4a_sse2_inline.h -o $@ test_time2_inline.c

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

sfmt4a_alti44497_inline: test_time_inline.c sfmt4a_alti44497_inline.h
	${CC} -DRANDOM=sfmt4a_alti44497_inline.h -o $@ test_time_inline.c

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

sfmt4a_std: sfmt4a_std.o test_time.o
	${CC} -o $@ sfmt4a_std.o test_time.o

pmt_fastc: ht-rnd.o test_time.o
	${CC} -o $@ ht-rnd.o test_time.o

pmt_fastc_inline: test_time_inline.c pmt_fastc_inline.h
	${CC} -DRANDOM=pmt_fastc_inline.h -o $@ test_time_inline.c

pmt_fastc_inline2: test_time2_inline.c pmt_fastc_inline.h
	${CC} -DRANDOM=pmt_fastc_inline.h -o $@ test_time2_inline.c

mt_fastc: mt19937ar.o test_time.o
	${CC} -o $@ mt19937ar.o test_time.o

mt_fastc_inline: mt19937ar_inline.h test_time_inline.c
	${CC} -DRANDOM=mt19937ar_inline.h -o $@ test_time_inline.c

mt_fastc_inline2: mt19937ar_inline.h test_time2_inline.c
	${CC} -DRANDOM=mt19937ar_inline.h -o $@ test_time2_inline.c

.c.o:
	${CC} -c $<
