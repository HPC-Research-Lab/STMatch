labeled_graph_path=/space/ywei51/graphs/dryadic_test_data/dryadic_graph/
labeled_graph_path=~/graphs/dryadic_test_data/dryadic_graph_labeled/
#labeled_graph_path=/data/not_backed_up/ywei51/dryadic_graph_labeled/

labeled_pattern_path=/space/ywei51/graphs/our_test_data/edge_induced_patterns/
labeled_pattern_path=~/graphs/our_test_data/edge_induced_patterns_labeled/

size5_pattern=(5_12.txt 5_14.txt 5_15.txt 5_16.txt 5_17.txt 5_18.txt 5_19.txt 5_20.txt)
size6_pattern=(6_47.txt 6_28.txt 6_50.txt 6_53.txt 6_62.txt 6_107.txt 6_110.txt 6_111.txt )
size7_pattern=(7_830.txt 7_845.txt 7_846.txt 7_847.txt 7_848.txt 7_850.txt 7_851.txt 7_852.txt)
size8_pattern=(8_0.txt 8_1.txt 8_2.txt 8_3.txt 8_4.txt 8_5.txt 8_6.txt 8_7.txt)
#size7_pattern=(7_852.txt 7_851.txt 7_850.txt )


small_graph_files=(Wiki-votels Amazon Enron DBLP Patents)
labeled_graph=(Orkut) #MiCo Youtube LiveJournal Orkut)

tab=$'\t'
for f in ${labeled_graph[@]} #${mid_graph_files[@]};
do
    for p in ${size5_pattern[@]} ${size6_pattern[@]} ${size7_pattern[@]}
    do
        #echo -n ${p}${tab}${f}${tab}
	    ./cu_test  ${labeled_graph_path}/${f}/snap.txt ${labeled_pattern_path}/${p}
        #nv-nsight-cu-cli  --set full -f -o ${f}${p} ./cu_test  ${labeled_graph_path}/${f}/snap.txt ${labeled_pattern_path}/${p}
        sleep 0.2
    done
done

#tab=$'\t'
#for f in ${small_graph_files[@]} #${mid_graph_files[@]};
#do
#    for p in ${size7_pattern[@]}
#    do
#        echo -n ${p}${tab}${f}${tab}
#	    ./cu_test  ${graph_path}/${f}/snap.txt ${pattern_path}/${p}
#        sleep 0.2
#    done
#done
