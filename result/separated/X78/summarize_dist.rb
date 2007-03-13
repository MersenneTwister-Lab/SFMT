#!/sw/bin/ruby

def get_last(line)
  line.sub(/^.*= */,"")
end

if ARGV.size != 3
  print "usage:summarize_dist [-b32|-b64|-b128] mexp filename\n"
  exit
end
word_width = ARGV[0]
word_width = word_width.sub(/-b/,'')
mexp = ARGV[1].to_i
fname = ARGV[2]
#p mexp, word_width, fname
op = false
IO.foreach(fname){
  |line|
  line.chomp!
  if line.include? 'k-distribution' and line.include? word_width
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
