#!/bin/sh
exps="607 1279 2281 4253 11213 19937 44497 86243 132049 216091"
for mexp in $exps; do

    if ./test-jump-M${mexp} -c > /dev/null
    then
	echo jump in mexp = $mexp OK.
    else
	exit 1
    fi
done
exit 0
