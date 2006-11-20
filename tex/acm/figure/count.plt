#set terminal postscript eps color
set terminal tgif
#set output "sfmt-zero.eps"
set output "sfmt-zero.obj"
plot [0:5000][0:0.51] \
     "WELL20000.count.txt" title "WELLc" with line linewidth 2, \
     "PMT20000.count.txt" title "PMT" with line linewidth 2, \
     "SFMTP37-20000.count.txt" title "SFMT" with line linewidth 2, \
     "MT20000.count.txt" title "MT" with line linewidth 2
