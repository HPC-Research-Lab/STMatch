#!/bin/bash -e

dir="${1}"
fname="${dir}/snap.txt"

make

nedge=1000
perorig=80
nba=20
#original graph
cd ../../graph_converter
bash convert.sh ${1}
cd -
#labels
bin/labelize ${fname}

#exit 1
#make daf
bin/outtoDAF ${fname}
#split for continuous/batch
bin/split_tool $fname 80 $nedge $nba
echo bin/split_tool $fname 80 $nedge $nba
#convert start
cd ../../graph_converter
bash convert.sh ${fname}.start
cd -
#make turb 
bin/unlabelled_turb ${fname}.start
#make the updates
for i in $(seq 0 $((${nba}-1)))
do
    echo $nedge > ${fname}.add.$i.upd
    cat ${fname}.add.$i >> ${fname}.add.${i}.upd
    bin/updateconvert ${fname}.add.${i}
done
