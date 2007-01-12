#!/bin/sh
bit_len=$1
prefix=$2
tmp=tmp.$$
tmp64=tmp64.$$
exps="607 2281 4253 11213 19937 44497 86243 132049"
for mexp in $exps; do
    if [ $bit_len == "64" ]; then
	if make test-std64-M${mexp}; then
	    :;
	else
	    exit 1
	fi
	./test-std64-M${mexp} > $tmp64
	compare=$tmp64
    else
	compare=SFMTp.${mexp}.out.txt
    fi
    command=${prefix}${bit_len}-M${mexp}
    if make $command; then
	:;
    else
	if [ -n "$tmp64" ] && [ -e $tmp64 ]; then
	    rm -f $tmp64
	fi
	exit 1
    fi
    ./$command > $tmp
    if diff -q -w $tmp $compare; then
	echo $command output check OK
	rm -f $tmp
    else
	echo $command output check NG!
	rm -f $tmp
	if [ -n "$tmp64" ] && [ -e $tmp64 ]; then
	    rm -f $tmp64
	fi
	exit 1
    fi
done
if [ -n "$tmp64" ] && [ -e $tmp64 ]; then
    rm -f $tmp64
fi
exit 0
