#!/sw/bin/ruby

$param_key = ['mexp','D.D','POS1','SL1','SL2','SR1','SR2','MSK1','MSK2','MSK3',
              'MSK4', 'p[0]','p[1]','p[2]','p[3]']
$param_name = ['MEXP','D.D','POS1','SL1','SL2','SR1','SR2','MSK1','MSK2','MSK3',
               'MSK4', 'PARITY1','PARITY2','PARITY3','PARITY4']
$tbl32 = [12,13,14,15,8,9,10,11,4,5,6,7,0,1,2,3]
$tbl64 = [8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7]

def sl_perm(sl, tbl, l, r)
  sl = sl.to_i
  str = l
  pos = 12
  (0..15).each{
    |i|
    p = tbl[i] + sl
    if tbl[p] == 0
      pos = i + 16
    end
    if p > 15
      str << 'x' << ','
    elsif p < 0
      str << 'x' << ','
    else
      str << tbl[p].to_s << ','
    end
  }
  str = str.gsub(/x/, pos.to_s)
  str.chop!
  str << r
end

def get_last(line)
  line.sub(/^.*= */,"").sub(/0x/,"")
end

def get_dd(filename)
  IO.foreach(filename) {
    |line|
    line.chomp!
    if line.include? "32bit D.D" then
      return line.sub(/^[^:]*: */,"").sub(/,.*$/,"")
    end
  }
end

def get_params(filename)
  params = Hash.new
  IO.foreach(filename) {
    |line|
    line.chomp!
    $param_key.each_index {
      |i|
      if line.include? $param_key[i]
        params.store($param_name[i], get_last(line))
      end
    }
  }
  return params
end

params = get_params(ARGV[0])
dd = get_dd(ARGV[0].sub(/parity/,'sh'))
params.store('D.D',dd)

$param_name.each {
  |key|
  printf("%s,", params[key])
}
printf("%s,", sl_perm(params['SL2'], $tbl32, '',''))
printf("%s,", sl_perm(params['SL2'], $tbl64, '',''))
printf("%s,", sl_perm("-"+params['SR2'], $tbl32, '',''))
printf("%s", sl_perm("-"+params['SR2'], $tbl64, '',''))
printf("\n")
