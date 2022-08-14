#!/bin/bash
graph_path=./graph/

pattern_path=./pattern/

pattern=(1.g  2.g  3.g  4.g  5.g  6.g  7.g  8.g  9.g  10.g  11.g  12.g  13.g  14.g  15.g  16.g  17.g  18.g  19.g  20.g  21.g  22.g  23.g  24.g)

size5_pattern=(5_12.txt 5_14.txt 5_15.txt 5_16.txt 5_17.txt 5_18.txt 5_19.txt 5_20.txt)
size6_pattern=(6_47.txt 6_28.txt 6_50.txt 6_53.txt 6_62.txt 6_107.txt 6_110.txt 6_111.txt)
size7_pattern=(7_830.txt 7_845.txt 7_846.txt 7_847.txt 7_848.txt 7_850.txt 7_851.txt 7_852.txt)

idx=1
plus_one=1
for p in ${size5_pattern[@]}  ${size6_pattern[@]}  ${size7_pattern[@]};
do
    
    #echo ${idx} 
    mv ${pattern_path}${p} ${pattern_path}${idx}.g
    declare -i idx=$idx+$plus_one
done