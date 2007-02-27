#!/sw/bin/ruby

$param_key = ['POS1','SL1','SL2','SR1','SR2','MSK1','MSK2','MSK3','MSK4',
              'p[0]','p[1]','p[2]','p[3]']
$param_name = ['POS1','SL1','SL2','SR1','SR2','MSK1','MSK2','MSK3','MSK4',
              'PARITY1','PARITY2','PARITY3','PARITY4']
$tbl32 = [12,13,14,15,8,9,10,11,4,5,6,7,0,1,2,3]
$tbl64 = [8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7]

def sl_perm(sl, tbl)
  sl = sl.to_i
  str = '('
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
  str << ')'
end

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

ARGV.each{
  |f|
  mexp = f.sub(/parity.resX78./,"").sub(/.magi.*txt/,"")
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
  printf("#define ALTI_SL2_PERM \\\n(vector unsigned char)%s\n",
         sl_perm(params['SL2'], $tbl32))
  printf("#define ALTI_SL2_PERM64 \\\n(vector unsigned char)%s\n",
         sl_perm(params['SL2'], $tbl64))
  printf("#define ALTI_SR2_PERM \\\n(vector unsigned char)%s\n",
         sl_perm("-"+params['SR2'], $tbl32))
  printf("#define ALTI_SR2_PERM64 \\\n(vector unsigned char)%s\n",
         sl_perm("-"+params['SR2'], $tbl64))
  printf("#define IDSTR\t\"SFMT-%s:%s-%s-%s-%s-%s:%s-%s-%s-%s\"\n",
         mexp, params['POS1'], params['SL1'],
         params['SL2'], params['SR1'], params['SR2'],
         params['MSK1'], params['MSK2'], params['MSK3'],
         params['MSK4'])
}
