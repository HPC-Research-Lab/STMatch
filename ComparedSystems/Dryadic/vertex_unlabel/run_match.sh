#!/bin/bash

graph_path=../graphs

size5_pattern=(5_12_best 5_14_best 5_15_best 5_16_best 5_17_best 5_18_best 5_19_best 5_20_best)
size6_pattern=(6_47_best 6_28_best 6_50_best 6_53_best 6_62_best 6_107_best 6_110_best 6_111_best )
size7_pattern=(7_830_best 7_845_best 7_846_best 7_847_best 7_848_best 7_850_best 7_851_best 7_852_best)

small_graph_files=(Wiki-votels Enron)
mid_graph_files=(Enron)

tab=$'\t'
for f in ${mid_graph_files[@]} 
do
    for p in ${size5_pattern[@]}
    do
        echo -n ${p}${tab}${f}${tab}
	    ./bin/${p}  ${graph_path}/${f}/snap.txt
        sleep 0.2
    done
done
