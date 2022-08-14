dir="${1}"
fname="${dir}/snap.txt"
bin/split_tool2 ${fname} 80 10 1800 200
bash convertNoRelabel.sh ${fname}.start
bin/labelize ${fname}.start
bin/unlabelled_turb ${fname}.start
for i in $(seq 0 19)
do
    bin/updateconvert ${fname}.chl.${i}
done
