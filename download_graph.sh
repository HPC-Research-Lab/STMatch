graphs=(
    https://snap.stanford.edu/data/wiki-Vote.txt.gz 
    https://snap.stanford.edu/data/email-Enron.txt.gz
    https://snap.stanford.edu/data/bigdata/communities/com-amazon.ungraph.txt.gz
    https://snap.stanford.edu/data/bigdata/communities/com-youtube.ungraph.txt.gz
    https://snap.stanford.edu/data/bigdata/communities/com-lj.ungraph.txt.gz
)

for graph in ${graphs[@]}
do
    wget -nc -P ./data/txt_graph ${graph}
done

gzip -f -d ./data/txt_graph/*