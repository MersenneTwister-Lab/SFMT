#!/sw/bin/ruby
ARGV.each {
  |f|
  out = false
  IO.foreach(f) {
    |line|
    if line =~ /^[01]+$/
      out = true
    end
    if out
      print line
    end
    if out and line =~ /deg/
      break
    end
  }
}
