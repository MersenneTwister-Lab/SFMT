#!/sw/bin/ruby

$param_key = ['mexp','D.D','POS1','SL1','SL2','SR1','SR2','MSK1','MSK2',
              'p[0]','p[1]']
$param_name = ['MEXP','D.D','POS1','SL1','SL2','SR1','SR2','MSK1','MSK2',
               'PCV1', 'PCV2']
def get_last(line)
  line.sub(/^.*= */,"")
end

def get_dd(line)
  line.sub(/^[^:]*: */,"").sub(/,.*$/,"")
end

def get_params(filename)
  params = Hash.new
  IO.foreach(filename) {
    |line|
    line.chomp!
    if line.include? 'mexp'
      $mexp = line.sub(/,.*/,'').sub(/^.*= /,'');
    end
    $param_key.each_index {
      |i|
      if line.include? $param_key[i]
        params.store($param_name[i], get_last(line))
      end
    }
  }
  return params
end

def sl_perm(sl)
  sl = sl.to_i
  tbl = [8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7]
  str = ''
  (0..15).each{
    |i|
    p = tbl[i] + sl
    if p > 15
      str << (16 - sl + 16).to_s << ','
    elsif p < 0
      str << (16 - sl).to_s << ','
    else
      str << tbl[p].to_s << ','
    end
  }
  str.chop!
end

def sr_perm64(sr)
  str = ''
  (0..7).each{
    |i|
    p = i - sr
    if p < 0
      str << (16 + sr).to_s << ','
    else
      str << p.to_s << ','
    end
  }
  (8..15).each{
    |i|
    p = i - sr
    if p < 8
      str << (16 + sr).to_s << ','
    else
      str << p.to_s << ','
    end
  }
  str.chop!
end

def alti_sl1(sl1)
  sl1 = sl1.to_i
  t = sl1 % 8
  $alti_sl1 = "#{t}, #{t}, #{t}, #{t}"
  sl1_pm = sl1 / 8
  $alti_sl1_perm = sl_perm(sl1_pm)
  if sl1 > 32
    sl1_m1 = (0xffffffff << (sl1 - 32)) & 0xffffffff
    sl1_m2 = 0
  else
    sl1_m1 = 0xffffffff
    sl1_m2 = (0xffffffff << (sl1 % 32)) & 0xffffffff
  end
  $alti_sl1_msk = sprintf("0x%08xU,0x%08xU,0x%08xU,0x%08xU",
                          sl1_m1, sl1_m2, sl1_m1, sl1_m2)
end

def alti_sr1(sr1, msk1, msk2)
  sr1 = sr1.to_i
  msk1 = msk1.hex
  msk2 = msk2.hex
  $alti_sr1 = "#{sr1}, #{sr1}, #{sr1}, #{sr1}"
  sr1_m1 = 0xffffffff >> sr1
  sr1_m2 = 0xffffffff
  $alti_sr1_msk = sprintf("0x%08xU,0x%08xU,0x%08xU,0x%08xU",
                          sr1_m1 & (msk1 >> 32), sr1_m2 & msk1,
                          sr1_m1 & (msk2 >> 32), sr1_m2 & msk2)
end

def get_dd(filename)
  IO.foreach(filename) {
    |line|
    line.chomp!
    if line.include? "D.D" then
      return line.sub(/^[^:]*: */,"").sub(/,.*$/,"")
    end
  }
end

params = get_params(ARGV[0])
dd = get_dd(ARGV[0].sub(/parity/,"sh"))
params.store('D.D', dd)

$param_name.each {
  |key|
  printf("#{params[key]},")
}
alti_sl1(params['SL1'])
alti_sr1(params['SR1'], params['MSK1'], params['MSK2'])
printf("#{$alti_sl1_perm},")
printf("#{$alti_sl1_msk},")
printf("#{sl_perm(params['SL2'])},")
printf("#{$alti_sr1_msk},")
printf("#{sr_perm64(params['SR2'].to_i/8)}")
printf("\n")

