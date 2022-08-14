#make unlabelled turboflux for 
dir="${1}"
fname="${dir}/snap.txt"
bin/split_tool ${fname} 80 1000 20
bash convertNoRelabel.sh ${fname}.start.v0
#bin/labelize ${fname}.start.v0
bin/unlabelled_turb ${fname}.start.v0
#for i in $(seq 0 19)
#do
    #echo 1000 > ${fname}.add.$i.upd
    #cat ${fname}.add.$i >> ${fname}.add.${i}.upd
    #bin/updateconvert ${fname}.add.${i}
#done
