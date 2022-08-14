#!/bin/bash

graph_path=../graphs/
#graph_path=/data/not_backed_up/ywei51/dryadic_graph_labeled/

size5_pattern=(5_12.txt_li_best 5_14.txt_li_best 5_15.txt_li_best 5_16.txt_li_best 5_17.txt_li_best 5_18.txt_li_best 5_19.txt_li_best 5_20.txt_li_best)
size6_pattern=(6_47.txt_li_best 6_28.txt_li_best 6_50.txt_li_best  6_53.txt_li_best 6_62.txt_li_best 6_107.txt_li_best 6_110.txt_li_best 6_111.txt_li_best)
size7_pattern=(7_830.txt_li_best 7_845.txt_li_best 7_846.txt_li_best 7_847.txt_li_best 7_848.txt_li_best 7_850.txt_li_best 7_851.txt_li_best 7_852.txt_li_best)
#size7_pattern=(7_852.txt_li_best 7_851.txt_li_best 7_850.txt_li_best)
size8_pattern=(8_0.txt_li_best 8_1.txt_li_best 8_2.txt_li_best 8_3.txt_li_best 8_4.txt_li_best 8_5.txt_li_best 8_6.txt_li_best 8_7.txt_li_best)

#small_graph_files=(Wiki-votels Enron DBLP Amazon Patents)
big_graph_files=(Wikivote Enron Youtube MiCo LiveJournal Orkut FriendSter)

tab=$'\t'
for f in ${big_graph_files[@]} 
do
    for p in ${size5_pattern[@]} ${size6_pattern[@]} ${size7_pattern[@]}
    do
        #echo -n ${p}${tab}${f}${tab}
	    echo ./bin/${p}  ${graph_path}/${f}/snap.txt
        #sleep 0.2
    done
done

#tab=$'\t'
#for f in ${big_graph_files[@]} 
#do
#    for p in ${size7_pattern[@]}
#    do
#        echo -n ${p}${tab}${f}${tab}
#	    ./bin/${p}  ${graph_path}/${f}/snap.txt
#        sleep 0.2
#    done
#done
