#!/bin/sh
exps="607 1279 2281 4423 11213 19937 44497 86243 132049 216091"
for mexp in $exps; do
    gcc -DSFMT_MEXP=${mexp} -DMAIN -o dSFMT-ref-M${mexp} dSFMT-ref.c 
    ./dSFMT-ref-M${mexp} > dSFMT.${mexp}.out.txt
    ./dSFMT-ref-M${mexp} -ar > dSFMT.${mexp}.ar-out.txt
done
exit 0
