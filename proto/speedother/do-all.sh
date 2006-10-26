#!/bin/bash
file="well_fastc_inline2 rand_fastc_inline2 rand48_fastc_inline2 \
	mrg_fastc_inline2 random256g2_fastc_inline2 xor3_fastc_inline2"
for i in $file; do
    ./$i > $i.ppc.txt
done