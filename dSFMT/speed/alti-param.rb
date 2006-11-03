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

IO.foreach(ARGV[0]) {
  |line|
  line.chomp!
  if line.include? '#define'
    if line.include? 'MEXP' and $MEXP == 0
      $MEXP = line.sub(/^[^ ]* [^ ]* */,'').to_i
    end
    if line.include? 'WORDSIZE' and $WORDSIZE == 0
      $WORDSIZE = line.sub(/^[^ ]* [^ ]* */,'').to_i
    end
    if line.include? 'POS1'
      $POS1 = line.sub(/^[^ ]* [^ ]* */,'').to_i
    end
    if line.include? '#define SL1' and $SL1 == 0
      $SL1 = line.sub(/^[^ ]* [^ ]* */,'').to_i
    end
    if line.include? 'SL2' and $SL2 == 0
      $SL2 = line.sub(/^[^ ]* [^ ]* */,'').to_i
    end
    if line.include? 'SR1' and $SR1 == 0
      $SR1 = line.sub(/^[^ ]* [^ ]* */,'').to_i
    end
    if line.include? 'SR2' and $SR2 == 0
      $SR2 = line.sub(/^[^ ]* [^ ]* */,'').to_i
    end
    if line.include? 'MSK1' and $MSK1 == 0
      line = line.sub(/^[^ ]* [^ ]* */,'')
      line = line.sub(/ULL/,'')
      $MSK1 = line.hex
    end
    if line.include? 'MSK2'and $MSK2 == 0
      line = line.sub(/^[^ ]* [^ ]* */,'')
      line = line.sub(/ULL/,'')
      $MSK2 = line.hex
    end
  end
}
=begin
printf("#define MEXP %d\n", $MEXP)
printf("#define WORDSIZE %d\n", $WORDSIZE)
printf("#define N %d\n", $MEXP / $WORDSIZE)
printf("#define MAXDEGREE %d\n", $WORDSIZE * ($MEXP / $WORDSIZE + 1))
printf("#define POS1 %d\n", $POS1)
printf("#define SL1 %d\n", $SL1)
printf("#define SL2 %d\n", $SL2)
printf("#define SR1 %d\n", $SR1)
printf("#define SR2 %d\n", $SR2)
printf("#define MSK1 0x%016xULL\n", $MSK1)
printf("#define MSK2 0x%016xULL\n", $MSK2)
=end

# SL1 (52 bit free shift)
printf("#define ALTI_SL1 %d\n", $SL1 % 8)
sl1_pm = $SL1 / 8
printf("#define SL1_PERM \\\n(vector unsigned char)%s\n", sl_perm(sl1_pm))
if $SL1 > 32
  sl1_m1 = (0xffffffff << ($SL1 - 32)) & 0xffffffff
else
  sl1_m1 = 0xffffffff
end
sl1_m2 = (0xffffffff << ($SL1 % 32)) & 0xffffffff
printf("#define SL1_MSK \\\n")
printf("(vector unsigned int)(0x%08xU,0x%08xU,0x%08xU,0x%08xU)\n",
       sl1_m1, sl1_m2, sl1_m1, sl1_m2)
# SL2 (128 bit 8 multiple shift)
printf("#define SL2_PERM \\\n(vector unsigned char)%s\n", sl_perm($SL2))
# SR1 (0-7 bit shift)
printf("#define ALTI_SR1 %d\n", $SR1)
sr1_m1 = 0xffffffff >> $SR1
sr1_m2 = 0xffffffff
printf("#define SR1_MSK \\\n")
printf("(vector unsigned int)(0x%08xU,0x%08xU,0x%08xU,0x%08xU)\n",
       sr1_m1 & ($MSK1 >> 32), sr1_m2 & $MSK1,
       sr1_m1 & ($MSK2 >> 32), sr1_m2 & $MSK2)
# SR2 (0-48 bit shift multiple of 8)
printf("#define SR2_PERM \\\n(vector unsigned char)%s\n", sr_perm64($SR2/8))
printf("#define ALTI_PERM (vector unsigned char)\\\n")
printf("  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)\n")
#printf("#define ALTI_MSK (vector unsigned int)\\\n")
#printf("    (0x7ffebfd7U, 0xff6affffU, 0xfeffbff7U, 0xfffffbbfU)\n")
printf("#define ALTI_LOW_MSK (vector unsigned int)\\\n")
printf("  (LOW_MASK32_1, LOW_MASK32_2, LOW_MASK32_1, LOW_MASK32_2)\n")
printf("#define ALTI_HIGH_CONST ")
printf("(vector unsigned int)(HIGH_CONST32, 0, HIGH_CONST32, 0)\n")
