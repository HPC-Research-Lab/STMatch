#!/bin/bash

pattern_path=/space/ywei51/graphs/cuts_test_data/selected_pattern/
pattern5_path=/space/ywei51/graphs/cuts_test_data/selected_pattern/5_*
              #/space/ywei51/graphs/cuts_test_data/selected_pattern
pattern6_path=/space/ywei51/graphs/cuts_test_data/selected_pattern/6_*
pattern7_path=/space/ywei51/graphs/cuts_test_data/selected_pattern/7_*
pattern8_path=/space/ywei51/graphs/cuts_test_data/selected_pattern/8_*
labeled_pattern5_path=/space/ywei51/graphs/dryadic_test_data/selected_labeled_pattern/5_*
                      
labeled_pattern6_path=/space/ywei51/graphs/dryadic_test_data/selected_labeled_pattern/6_*
labeled_pattern7_path=/space/ywei51/graphs/dryadic_test_data/selected_labeled_pattern/7_*
labeled_pattern8_path=/space/ywei51/graphs/dryadic_test_data/selected_labeled_pattern/8_*

for f in ${labeled_pattern5_path} ${labeled_pattern6_path} ${labeled_pattern7_path};
do
    #./bin/compile --pattern-list-lic ${f}
    #./bin/compile --pattern-list-l ${f}
    ./bin/compile --pattern-list-li ${f}
done

