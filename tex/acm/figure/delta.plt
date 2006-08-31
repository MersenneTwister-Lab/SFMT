#set terminal postscript eps color
set terminal tgif
#set output "delta.eps"
set output "delta.obj"
plot [1:32][0:600] "mt-delta.txt" title "mt" with line linewidth 8, "sfmt-delta.txt" title "sfmt" with line linewidth 8
