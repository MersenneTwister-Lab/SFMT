#set terminal postscript eps color
set terminal tgif
#set output "sfmt-zero.eps"
set output "sfmt-zero.obj"
plot [0:20000][0:0.51] \
     "WELL20000.count.txt" title "WELLc" smooth bezier linewidth 2, \
     "PMT20000.count.txt" title "PMT" smooth bezier linewidth 2, \
     "SFMT20000.count.txt" title "SFMT" smooth bezier linewidth 2, \
     "SFMTP37-20000.count.txt" title "PSFMT" smooth bezier linewidth 2, \
     "MT20000.count.txt" title "MT" smooth bezier linewidth 2
