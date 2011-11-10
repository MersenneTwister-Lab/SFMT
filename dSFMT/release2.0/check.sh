#!/bin/sh
prefix=$1
tmp=tmp.$$
exps="521 1279 2203 4253 11213 19937 44497 86243 132049 216091"
for mexp in $exps; do
    compare=dSFMT.${mexp}.out.txt
    command=${prefix}-M${mexp}
    ./$command -v > $tmp
    if diff -q -w $tmp $compare; then
	echo $command output check OK
	rm -f $tmp
    else
	echo $command output check NG!
	rm -f $tmp
	exit 1
    fi
done
exit 0
