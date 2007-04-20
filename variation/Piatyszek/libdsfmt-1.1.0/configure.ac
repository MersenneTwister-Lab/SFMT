#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([dSFMT],[1.1.0],[ediap@users.sourceforge.net],[libdsfmt])
AC_CONFIG_SRCDIR([dSFMT.c])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC

AC_ARG_ENABLE(sse2,
  [AS_HELP_STRING([--enable-sse2], [enable SSE2 optimizations])], 
  have_sse2=$enableval, have_sse2=no)

if test "$have_sse2" = "yes"; then
  AC_DEFINE(HAVE_SSE2, 1, [Define to enable SSE2 optimizations])
  CFLAGS="$CFLAGS -msse2"
fi

AC_ARG_ENABLE(altivec, 
  [AS_HELP_STRING([--enable-altivec], [enable Altivec optimizations])],
  have_altivec=$enableval, have_altivec=no)

if test "$have_altivec" = "yes"; then
  AC_DEFINE(HAVE_ALTIVEC, 1, [Define to enable Altivec optimizations.])
  CFLAGS="$CFLAGS -faltivec -maltivec"
fi

AC_ARG_WITH(mexp, 
  [AS_HELP_STRING([--with-mexp=19937], 
    [set generator period exponent: 607, 1279, 2281, 4253, 11213, 19937,
      44497, 86243, 132049])])
case $with_mexp in
  607 | 1279 | 2281 | 4253 | 11213 | 19937 | 44497 | 86243 | 132049) 
    mexp="$with_mexp" ;;
  "") mexp=19937 ;;
  *) mexp=19937; 
    AC_MSG_WARN([Generator period exponent mexp="$with_mexp" not supported.
      Using the default: 19937.]) ;;
esac
AC_DEFINE_UNQUOTED(SFMT_MEXP, $mexp, [Define generator period.])

AC_CONFIG_FILES([Makefile html/Makefile])
AC_OUTPUT
