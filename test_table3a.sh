#!/bin/bash
graph_path=./data/bin_graph/
pattern_path=./data/pattern/
patterns=(1.g  2.g  3.g  4.g  5.g  6.g  7.g  8.g  9.g  10.g  11.g  12.g  13.g  14.g  15.g  16.g  17.g  18.g  19.g  20.g  21.g  22.g  23.g  24.g)

for pattern in ${patterns[@]}
do
	./bin/table_edge_ulb.exe ${1}/snap.txt ${pattern_path}/${pattern}
done