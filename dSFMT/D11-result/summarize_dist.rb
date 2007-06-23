#!/sw/bin/ruby

def get_last(line)
  line.sub(/^.*= */,"")
end

if ARGV.size != 2
  print "usage:summarize_dist mexp filename\n"
  exit
end
mexp = ARGV[0].to_i
fname = ARGV[1]
#p mexp, word_width, fname
op = false
IO.foreach(fname){
  |line|
  line.chomp!
  if line.include? 'k-distribution'
    op = true
    next
  end
  if op and line.include? 'D.D'
    dist = line.sub(/,.*$/,'').sub(/^.*: */,'')
    printf("#{dist}\n");
    op = false
    next
  end
  if op and line =~ /^k/
    dist = get_last(line).to_i
    k = line.sub(/^k\(/,'').sub(/\).*/,'').to_i
#    printf("#{k}\t#{dist}(#{mexp/k - dist})\n")
    printf("#{dist}(#{mexp/k - dist})\n")
  end
}
