#!/bin/sh

(libtoolize --version) < /dev/null > /dev/null 2>&1 || {
	echo "You must have libtool installed to compile IT++";
	echo;
	exit;
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
	echo "You must have automake installed to compile IT++";
	echo;
	exit;
}

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo "You must have autoconf installed to compile IT++";
	echo;
	exit;
}

aclocal || exit;
libtoolize --copy --force --automake || exit;
aclocal || exit;
autoconf || exit;
autoheader || exit;
automake --add-missing --copy || exit;

