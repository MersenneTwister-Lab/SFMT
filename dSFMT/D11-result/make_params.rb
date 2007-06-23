#!/sw/bin/ruby

$param_key = ['POS1','SL1','SL2','SR1','SR2','MSK1','MSK2','p[0]','p[1]']
$param_name = ['POS1','SL1','SL2','SR1','SR2','MSK1','MSK2', 'PARITY1',
               'PARITY2']
$mexp = "";
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
  printf("#define ALTI_SL1_PERM \\\n(vector unsigned char)%s\n",
         sl_perm(sl1_pm))
  if sl1 > 32
    sl1_m1 = (0xffffffff << (sl1 - 32)) & 0xffffffff
    sl1_m2 = 0
  else
    sl1_m1 = 0xffffffff
    sl1_m2 = (0xffffffff << (sl1 % 32)) & 0xffffffff
  end
  printf("#define ALTI_SL1_MSK \\\n")
  printf("(vector unsigned int)(0x%08xU,0x%08xU,0x%08xU,0x%08xU)\n",
         sl1_m1, sl1_m2, sl1_m1, sl1_m2)
end

def print_alti_sr1(sr1, msk1, msk2)
  sr1 = sr1.to_i
  msk1 = msk1.hex
  msk2 = msk2.hex
  printf("#define ALTI_SR1 %d\n", sr1)
  sr1_m1 = 0xffffffff >> sr1
  sr1_m2 = 0xffffffff
  printf("#define ALTI_SR1_MSK \\\n")
  printf("(vector unsigned int)(0x%08xU,0x%08xU,0x%08xU,0x%08xU)\n",
         sr1_m1 & (msk1 >> 32), sr1_m2 & msk1,
         sr1_m1 & (msk2 >> 32), sr1_m2 & msk2)
end

ARGV.each{
  |f|
  #mexp = f.sub(/parity.resD11./,"").sub(/.magi.*txt/,"")
  prefix = "SFMT_"
  params = get_params(f)
  mexp = $mexp
  printf("#ifndef DSFMT_PARAMS%s_H\n", mexp)
  printf("#define DSFMT_PARAMS%s_H\n\n", mexp)
  $param_name.each {
    |key|
    if !(key.include? 'MSK') and !(key.include? 'PARITY')
      printf("#define %s%s\t%s\n", prefix, key, params[key])
    end
  }
  printf("#define %s%s\tUINT64_C(0x%s)\n", prefix, 'MSK1', params['MSK1'])
  printf("#define %s%s\tUINT64_C(0x%s)\n", prefix, 'MSK2', params['MSK2'])
  printf("#define %sMSK32_1\t0x%sU\n", prefix, params['MSK1'][0,8])
  printf("#define %sMSK32_2\t0x%sU\n", prefix, params['MSK1'][8,8])
  printf("#define %sMSK32_3\t0x%sU\n", prefix, params['MSK2'][0,8])
  printf("#define %sMSK32_4\t0x%sU\n", prefix, params['MSK2'][8,8])
  printf("#define %s%s\tUINT64_C(%s)\n", prefix, "PCV1", params['PARITY1'])
  printf("#define %s%s\tUINT64_C(%s)\n", prefix, "PCV2", params['PARITY2'])
  printf("#define %sIDSTR \\\n\"dSFMT-%s:%s-%s-%s-%s-%s:%s-%s\"\n",
         prefix, mexp, params['POS1'], params['SL1'],
         params['SL2'], params['SR1'], params['SR2'],
         params['MSK1'], params['MSK2'])

  printf("\n\n/* PARAMETERS FOR ALTIVEC */\n")
  print_alti_sl1(params['SL1'])
  printf("#define ALTI_SL2_PERM \\\n(vector unsigned char)%s\n",
         sl_perm(params['SL2']))
  print_alti_sr1(params['SR1'], params['MSK1'], params['MSK2'])
  printf("#define ALTI_SR2_PERM \\\n(vector unsigned char)%s\n",
         sr_perm64(params['SR2'].to_i/8))
  printf("#define ALTI_PERM (vector unsigned char) \\\n")
  printf("  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)\n")
  printf("#define ALTI_LOW_MSK (vector unsigned int) \\\n")
  printf("  (%sLOW_MASK32_1, %sLOW_MASK32_2, %sLOW_MASK32_1, %sLOW_MASK32_2)\n",
         prefix, prefix, prefix, prefix)
  printf("#define ALTI_HIGH_CONST ")
  printf("(vector unsigned int)(%sHIGH_CONST32, 0, %sHIGH_CONST32, 0)\n",
         prefix, prefix)
  printf("\n#endif /* DSFMT_PARAMS%s_H */\n", mexp)
}
