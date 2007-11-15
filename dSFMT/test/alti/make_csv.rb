#!/sw/bin/ruby

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

def sr_perm(sr)
  sr = sr.to_i
  tbl = [8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7]
  str = ''
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
end

def alti_sl64(sl)
  sl = sl.to_i
  t = sl % 8
#  sl_str = "#{t}, #{t}, #{t}, #{t}"
  sl_str = t.to_s
  sl_pm = sl / 8
  perm_str = sl_perm(sl_pm)
  if sl >= 32
    sl_m1 = (0xffffffff << (sl - 32)) & 0xffffffff
    sl_m2 = 0
  else
    sl_m1 = 0xffffffff
    sl_m2 = (0xffffffff << (sl % 32)) & 0xffffffff
  end
  msk_str = sprintf("0x%08xU,0x%08xU,0x%08xU,0x%08xU",
                    sl_m1, sl_m2, sl_m1, sl_m2)
  return sl_str, perm_str, msk_str
end

def alti_sr64(sr)
  sr = sr.to_i
  t = sr % 8
#  sr_str = "#{t}, #{t}, #{t}, #{t}"
  sr_str=t.to_s
  sr_pm = sr / 8
  perm_str = sr_perm(sr_pm)
  if sr >= 32
    sr_m1 = 0
    sr_m2 = (0x0ffffffff >> (sr - 32)) & 0xffffffff
  else
    sr_m1 = (0x0ffffffff >> (sr % 32)) & 0xffffffff
    sr_m2 = 0xffffffff
  end
  msk_str = sprintf("0x%08xU,0x%08xU,0x%08xU,0x%08xU",
                    sr_m1, sr_m2, sr_m1, sr_m2)
  return sr_str, perm_str, msk_str
end

n = ARGV[0]
sl_str, perm_sl, msk_sl = alti_sl64(n)
sr_str, perm_sr, msk_sr = alti_sr64(n)
printf("#define SL #{n}\n")
printf("#define SR #{n}\n")
printf("#define PERM_SL #{perm_sl}\n") 
printf("#define SL128 #{sl_str}\n")
printf("#define MASK_SL #{msk_sl}\n") 
printf("#define PERM_SR #{perm_sr}\n")
printf("#define SR128 #{sr_str}\n")
printf("#define MASK_SR #{msk_sr}\n")


