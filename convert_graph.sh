txtdir=./data/txt_graph/
bindir=./data/bin_graph/

for graph in ${txtdir}*
do
    filename=${graph##*/}
    filename=${filename%.txt}
    mkdir -p ${bindir}/${filename}
    cp ${graph} ${bindir}/${filename}/snap.txt
done

