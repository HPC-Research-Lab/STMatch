#!/bin/bash

pattern5_path=/space/ywei51/graphs/cuts_test_data/selected_pattern/5_*
pattern6_path=/space/ywei51/graphs/cuts_test_data/selected_pattern/6_*
pattern7_path=/space/ywei51/graphs/cuts_test_data/selected_pattern/7_*

for f in ${pattern5_path};
do
    ./bin/compile --pattern ${f}
done
