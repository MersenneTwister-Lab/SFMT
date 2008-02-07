#!/sw/bin/ruby
# make parameter file for altivec from standard c parameter file
$MEXP = 0
$WORDSIZE = 0
$N = 0
$POS1 = 0
$SL1 = 0
$SL2 = 0
$SR1 = 0
$SR2 = 0
$MSK1 = 0
$MSK2 = 0

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

def sr_perm(sr)
  tbl = [8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7]
  str = '('
  (0..15).each{
    |i|
    p = tbl[i] - sr
    if p > 15
      str << (16 - sr + 16).to_s << ','
    elsif p < 0
      str << (16 - sr).to_s << ','
    else
      str << tbl[p].to_s << ','
    end
  }
  str.chop!
  str << ')'
end

def sl_mask(sl)
  if sl > 32
    m1 = (0xffffffff << (sl - 32)) & 0xffffffff
  else
    m1 = 0xffffffff
  end
  m2 = (0xffffffff << (sl % 32)) & 0xffffffff
  return m1, m2
end

def sr_mask(sr)
  if sr > 32
    m1 = (0x0ffffffff >> (sr - 32)) & 0xffffffff
  else
    m1 = 0xffffffff
  end
  m2 = (0x0ffffffff >> (sr % 32)) & 0xffffffff
  return m2, m1
end

if ARGV.size == 0 then
  print "alti-slsr sl sr"
  exit
end
$SL1 = ARGV[0].to_i
$SR1 = ARGV[1].to_i
# SL1 (52 bit free shift)
printf("#define ALTI_SL1 (vector unsigned char)(%d)\n", $SL1 % 8)
sl1_pm = $SL1 / 8
printf("#define SL1_PERM \\\n(vector unsigned char)%s\n", sl_perm(sl1_pm))
sl1_m1, sl1_m2 = sl_mask($SL1)
printf("#define SL1_MSK \\\n")
printf("(vector unsigned int)(0x%08xU,0x%08xU,0x%08xU,0x%08xU)\n",
       sl1_m1, sl1_m2, sl1_m1, sl1_m2)
# SR2 (0-48 bit shift multiple of 8)
printf("#define ALTI_SR1 (vector unsigned char)(%d)\n", $SR1 % 8)
sr1_pm = $SR1 / 8
printf("#define SR1_PERM \\\n(vector unsigned char)%s\n", sr_perm(sr1_pm))
sr1_m1, sr1_m2 = sr_mask($SR1)
printf("#define SR1_MSK \\\n")
printf("(vector unsigned int)(0x%08xU,0x%08xU,0x%08xU,0x%08xU)\n",
       sr1_m1, sr1_m2, sr1_m1, sr1_m2)

