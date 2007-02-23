#!/bin/sh
exps="607 2281 4423 11213 19937 44497 86243 132049"
for mexp in $exps; do
    gcc -DMEXP=${mexp} -DMAIN -o dSFMT-ref-M${mexp} dSFMT-ref.c 
    ./dSFMT-ref-M${mexp} > dSFMT.${mexp}.out.txt
done
exit 0
