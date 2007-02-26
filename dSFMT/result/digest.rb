#!/sw/bin/ruby

def get_last(line)
  line.sub(/^.*= */,"")
end

def get_dd(line)
  line.sub(/^[^:]*: */,"").sub(/,.*$/,"")
end

def digest(filename)
  param_key = ['POS1','SL1','SL2','SR1','SR2','MSK1','MSK2','MSK3','MSK4']
  params = Array.new
  weights = ""
  dd = Array.new
  IO.foreach(filename) {
    |line|
    line.chomp!
    param_key.each {
      |key|
      if line.include? key
        params.push(get_last(line))
      end
    }
    if line.include? 'weight'
      weights = get_last(line)
    end
    if line.include? 'D.D'
      dd.push(get_dd(line))
    end
  }
  return params, weights, dd
end

#p getfbase("result19937.0825.m09.txt")
#p read_filter("./test.txt")
#p $dir
ARGV.each{
  |f|
  params, weight, dd = digest(f)
  printf "%s\t", f 
  params.each {
    |p|
    printf "%s\t", p
  }
  printf "%s\t", weight 
  dd.each {
    |p|
    printf "%s\t", p
  }
  printf "\n"
}
