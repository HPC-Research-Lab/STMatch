## Overview
STMatch is a stack-based subgraph pattern matching on GPU. 

## Building STMatch

### Data Preparation

Go to graph_converter directory and compile graph converter
```Shell
cd graph_converter/
make
```

In graph_converter directory, run prepare_data.sh to download graph data and transfer the data foramat to our system's format. 
The shell script prepare_data.sh will also labelize the downloaded graphs
```Shell
#we commented 2 graph links(Orkut and Friendster) on the top of prepare_data.sh because this 2 graphs are extreme big. 
#It will take you about 1 ~ 2 hours to download and do conversion jobs. 
#You can uncomment the link of this 2 graph in prepare_data.sh if you want to test them and then re-run prepare_data.sh
bash prepare_data.sh  
```

Now you can see some directories in ~/project/dara/bin_graph/
```Shell
#it's like "com-amazon.ungraph  com-dblp.ungraph  com-youtube.ungraph  email-Enron  wiki-Vote"
```

### Compile STMatch
Configure the prograpm
```Shell
#You should configure src/config.h file before you compile. 
#There are 4 types of graph matching: labeled/vertex-induced, unlabeled/vertex-induced, labeled/edge-induced, unlabeled/edge-induced
#For example, if you need to test unlabeled/edge-induced graph matching, you should set LABELED to be false and EDGE_INDUCED to be true in src/config.h. There are 3 parameters you should care about: LABELED, EDGE_INDUCED, JOB_CHUNK_SIZE. 
#For testing graph amzon, set JOB_CHUNK_SIZE to be 64 could get the best performance. 
#For testing all other graphs, set JOB_CHUNK_SIZE to be 8 could get the best performance. 
vim src/config.h
```

Go to the home directory of STMatch
```Shell
make clean
make
```

## Test STMatch

We provide a shell script for batch testing. You can use our script to test directly,

### Run batch test

```Shell
#For example, if you neet to test graph wiki-Vote, you should execute bash batch_test.sh ./data/bin_graph/wiki-Vote/
bash batch_test.sh  /path/to/converted/graph/directory/
```

### Run single test
```Shell
#For example, if you need to test graph Enron and pattern 1.g, 
# you should execute ./cu_test ./data/bin_graph/email-Enron/snap.txt ./data/pattern/1.g 
./cu_test /path/to/converted/graph/directory/snap.txt /path/to/pattern/file/
```
