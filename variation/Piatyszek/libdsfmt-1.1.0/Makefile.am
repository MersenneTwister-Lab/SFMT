AUTOMAKE_OPTIONS = dist-bzip2

SUBDIRS = html

EXTRA_DIST = \
	autogen.sh \
	CHANGE-LOG.txt \
	check.sh \
	dSFMT-ref.c \
	dSFMT.607.out.txt \
	dSFMT.1279.out.txt \
	dSFMT.2281.out.txt \
	dSFMT.4423.out.txt \
	dSFMT.11213.out.txt \
	dSFMT.19937.out.txt \
	dSFMT.44497.out.txt \
	dSFMT.86243.out.txt \
	dSFMT.132049.out.txt \
	FILES.txt \
	LICENSE.txt \
	Makefile.orig \
	README.txt \
	test.c

lib_LTLIBRARIES = libdsfmt.la
libdsfmt_la_SOURCES = \
	dSFMT.c \
	dSFMT.h \
	dSFMT-params.h \
	dSFMT-params607.h \
	dSFMT-params1279.h \
	dSFMT-params2281.h \
	dSFMT-params4423.h \
	dSFMT-params11213.h \
	dSFMT-params19937.h \
	dSFMT-params44497.h \
	dSFMT-params86243.h \
	dSFMT-params132049.h \
	dSFMT-alti.h \
	dSFMT-sse2.h

include_HEADERS = \
	dSFMT.h
