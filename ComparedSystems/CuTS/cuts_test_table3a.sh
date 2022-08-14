#!/bin/bash
graph_path=./graph/

pattern_path=./pattern/

pattern=(1.g  2.g  3.g  4.g  5.g  6.g  7.g  8.g  9.g  10.g  11.g  12.g  13.g  14.g  15.g  16.g  17.g  18.g  19.g  20.g  21.g  22.g  23.g  24.g)



small_graph_files=(wiki-Vote.g enron.g amazon.g)

idx=1
plus_one=1
for p in ${pattern[@]};
do
    echo ./build/cuts  ./graph/mico.g ${pattern_path}${p}
done