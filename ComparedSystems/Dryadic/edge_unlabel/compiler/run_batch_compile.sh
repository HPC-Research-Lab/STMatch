#!/bin/bash

graph_path=/space/ywei51/graphs/cuts_test_data/test_graphs/
pattern_path=/space/ywei51/graphs/dryadic_test_data/dryadic_pattern/all/

pattern_size=(size-4 size-5)

graph_files=(email-Enron wiki-Vote  com-amazon.ungraph com-dblp.ungraph cit-Patents mico)

#compile
 
for f in `ls ${pattern_path}`;
do
    ./bin/compile --pattern ${pattern_path}/${f}
done
