#!/bin/bash

graph_path=/space/ywei51/graphs/dryadic_test_data/dryadic_graph/
pattern_path=/space/ywei51/graphs/our_test_data/pattern_6/

graph_files=(Enron)

#compile
 
#for f in `ls ${pattern_path}`;
##do
    #./bin/compile --pattern ${pattern_path}/${f}
#done

tab="wwwww"
for f in ${graph_files[@]};
do
    for p in `ls ${pattern_path}`;
    do
        echo -n ${p}${tab}${f}${tab} 
        ./cu_test  ${graph_path}/${f}/snap.txt ${pattern_path}/${p}
	sleep 2
    done
done
