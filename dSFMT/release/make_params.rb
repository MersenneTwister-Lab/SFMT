#!/sw/bin/ruby

$param_key = ['POS1','SL1','SL2','SR1','SR2','MSK1','MSK2','p[0]','p[1]']
$param_name = ['POS1','SL1','SL2','SR1','SR2','MSK1','MSK2', 'PARITY1',
               'PARITY2']

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
  tbl = [8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7]
  str = '('
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
  str << ')'
end

def sr_perm64(sr)
  str = '('
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
  str << ')'
end

def print_alti_sl1(sl1)
  sl1 = sl1.to_i
  printf("#define ALTI_SL1 %d\n", sl1 % 8)
  sl1_pm = sl1 / 8
  printf("#define SL1_PERM \\\n(vector unsigned char)%s\n", sl_perm(sl1_pm))
  if sl1 > 32
    sl1_m1 = (0xffffffff << (sl1 - 32)) & 0xffffffff
  else
    sl1_m1 = 0xffffffff
  end
  sl1_m2 = (0xffffffff << (sl1 % 32)) & 0xffffffff
  printf("#define SL1_MSK \\\n")
  printf("(vector unsigned int)(0x%08xU,0x%08xU,0x%08xU,0x%08xU)\n",
         sl1_m1, sl1_m2, sl1_m1, sl1_m2)
end

def print_alti_sr1(sr1)
  sr1 = sr1.to_i
  printf("#define ALTI_SR1 %d\n", sr1)
  sr1_m1 = 0xffffffff >> sr1
  sr1_m2 = 0xffffffff
  printf("#define SR1_MSK \\\n")
  printf("(vector unsigned int)(0x%08xU,0x%08xU,0x%08xU,0x%08xU)\n",
         sr1_m1 & ($MSK1 >> 32), sr1_m2 & $MSK1,
         sr1_m1 & ($MSK2 >> 32), sr1_m2 & $MSK2)
end

ARGV.each{
  |f|
  params = get_params(f)
  $param_name.each {
    |key|
    if key.include? 'MSK'
      printf("#define %s\t0x%sU\n", key, params[key])
    elsif key.include? 'PARITY'
      printf("#define %s\t%sU\n", key, params[key])
    else
      printf("#define %s\t%s\n", key, params[key])
    end
  }

  printf("\n\n/* PARAMETERS FOR ALTIVEC */\n")
  print_alti_sl1(params['SL1'])
  printf("#define SL2_PERM \\\n(vector unsigned char)%s\n",
         sl_perm(params['SL2']))
  print_alti_sr1(params['SR1']);
}
