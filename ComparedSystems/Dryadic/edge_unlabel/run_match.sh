#!/bin/bash

graph_path=../graphs

graph_files=(Wikivote Enron MiCo)

#compile
 
#for f in `ls ${pattern_path}`;
#do
    #./bin/compile --pattern ${pattern_path}/${f}
#done

tab="wwwww"
for f in ${graph_files[@]};
do
    for p in `ls bin`;
    do
    #echo -n ${p}${tab}${f}${tab} 
	echo ./bin/${p} ${graph_path}/${f}/snap.txt
    done
done
``