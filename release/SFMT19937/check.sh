#!/bin/sh
tmp=tmp.$$
./$1 > $tmp
if diff -q -w $tmp $2; then
    echo $1 output check OK
    rm -f $tmp
else
    echo $1 output check NG!
    rm -f $tmp
    exit 1
fi