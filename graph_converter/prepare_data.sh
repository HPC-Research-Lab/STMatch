graphs=(
    https://snap.stanford.edu/data/wiki-Vote.txt.gz 
    https://snap.stanford.edu/data/email-Enron.txt.gz
    https://snap.stanford.edu/data/bigdata/communities/com-amazon.ungraph.txt.gz
    https://snap.stanford.edu/data/bigdata/communities/com-youtube.ungraph.txt.gz
    https://snap.stanford.edu/data/bigdata/communities/com-lj.ungraph.txt.gz
)

txtdir=../data/txt_graph/
bindir=../data/bin_graph/
mkdir -p ${txtdir}
mkdir -p ${bindir}

for graph in ${graphs[@]}
do
    wget -nc -P ${txtdir} ${graph}
done
gzip -f -d ${txtdir}*

for graph in ${txtdir}*
do
    filename=${graph##*/}
    filename=${filename%.txt}
    mkdir -p ${bindir}/${filename}
    cp ${graph} ${bindir}/${filename}/snap.txt
done

for graph in ${bindir}*
do
    bash convert.sh ${graph}
done
