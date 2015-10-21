arraysize=( 1200 2400 4800 )
npr=( 1 2 4 6 8 10 12 )
repeat=16

icpc -O3 -fopenmp main.cc MatUtil.c -o APSP

for a in ${arraysize[@]}; do
    for n in ${npr[@]}; do
        ./APSP $n $a $repeat
    done
done