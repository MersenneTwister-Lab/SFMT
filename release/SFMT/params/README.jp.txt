このディレクトリにはパラメータファイルを生成するデータが置いてあります。
擬似乱数をいくつか並列に生成する場合、異なるパラメータを使用することによって、
二つのメリットがあります。
1. 生成する数列が重なってしまうことがない。
2. 均等分布次元は和になることが期待出来る。
ひとつのパラメータで初期値だけ変えた場合は、こうなりません。
つまり、
1. 生成する数列が重なってしまう可能性がある。（確率は低い）
2. メモリは余分に使うのに、均等分布次元は並列でない場合と同じ。
詳しくは Dynamic Creator[1] の論文を参照してください。

csv2param.awk を使うことによって、csvファイルからパラメータファイルを
作ることができます。

./csv2param.awk line-no csv-filename > parameter-file

ここで、line-no はcsvファイルの行を指定する1から32までの数です。

[1] Makoto Matsumoto and Takuji Nishimura, 
    "Dynamic Creation of Pseudorandom Number Generators",
     Monte Carlo and Quasi-Monte Carlo Methods 1998, Springer, 2000, pp 56--69.
     http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/ARTICLES/articles.html	
