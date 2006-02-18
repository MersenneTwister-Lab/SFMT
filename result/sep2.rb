#!/sw/bin/ruby

$NUM = 1
$dir = ""

def getfbase(filename)
  fbase = filename.gsub(/result/,"res")
  #fbase.gsub!(/\.magi??/, "")
  fbase.gsub!(/\.txt/, "")
  fbase
end

def writef(filename, lines)
#  p filename, lines
#  return
  open(filename, "w"){
    |io|
    lines.each{
      |line|
      printf io, "%s", line
    }
  }
end

def filter_ir(filename)
  fbase = getfbase(filename)
  lines = Array.new
  first = true;
  IO.foreach(filename){
    |line|
    lines.push line
    if /^--/ =~ line
      if first
        first = false
        lines = Array.new
      else
        f = $dir + fbase + sprintf(".%02d.txt", $NUM)
        writef(f, lines)
        lines = Array.new
        $NUM += 1
      end
    end
  }
  if not lines.empty?
    f = $dir + fbase + sprintf(".%02d.txt", $NUM)
    writef(f, lines)
  end
end

#p getfbase("result19937.0825.m09.txt")
#p read_filter("./test.txt")
$dir = ARGV.shift.dup
if $dir[-1] != ?/
  $dir << "/"
end
#p $dir
ARGV.each{
  |f|
  filter_ir(f)
#p f
}
