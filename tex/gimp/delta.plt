set terminal postscript eps color
set output "delta.eps"
plot [0:32][0:600] "mt-delta.txt" title "mt" with line linewidth 8, "sfmt-delta.txt" title "sfmt" with line linewidth 8
