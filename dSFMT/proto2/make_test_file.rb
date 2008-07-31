#!/usr/bin/env ruby

def get_value(fname, vn1, vn2)
  v1 = ''
  v2 = ''
  IO.foreach(fname) {
    |line|
    if line.include? vn1 then
      v1 = line.sub(/^.*= /, '')
    end
    if line.include? vn2 then
      v2 = line.sub(/^.*= /, '')
    end
  }
  return v1, v2
end

def repl_value(fname, hash)
  IO.foreach(fname) {
    |line|
    hash.each_pair {
      |key, value|
      if line.include? key then
        line = "#{key} = #{value}"
        break
      end
    }
    print line
  }
end

fixf = ARGV[0]
pcvf = ARGV[1]

fix0, fix1 = get_value(fixf, 'fix[0]', 'fix[1]')
pcv0, pcv1 = get_value(pcvf, 'p[0]', 'p[1]')
pcv0.sub!('0x','')
pcv1.sub!('0x','')
hash = {'fix1' => fix0, 'fix2' => fix1, 'pcv1' => pcv0, 'pcv2' => pcv1}

repl_value(fixf, hash)
