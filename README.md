## Overview
STMatch is a stack-based subgraph pattern matching system on GPU. 

## Test on Distributed GPUs. 

### Data Preparation

Go to graph_converter directory and compile graph converter
```Shell
cd graph_converter/
make
```

In graph_converter directory, run prepare_data.sh to download graph data and transfer the data format to our system's format. 
The shell script prepare_data.sh will also labelize the downloaded graphs
```Shell
#we commented 2 graph links(Orkut and Friendster) on the top of prepare_data.sh \
#because these 2 graphs are extreme big. 
#It will take you about 1 ~ 2 hours to download and convert them. 
#You can uncomment the link of these 2 graphs in prepare_data.sh \
#if you want to test them and then re-run prepare_data.sh
bash prepare_data.sh  
```

Now you can see some directories in ~/project/dara/bin_graph/
```Shell
#it's like "com-amazon.ungraph  com-dblp.ungraph  com-youtube.ungraph  email-Enron  wiki-Vote"
```

### Compile STMatch
Configure the program
```Shell
#You should configure file src/config.h before you compile the project. 
#There are 4 types of subgraph matching: 
#labeled/vertex-induced, unlabeled/vertex-induced, labeled/edge-induced, unlabeled/edge-induced

#For example:
#If you need to test unlabeled and edge-induced subgraph matching,  \
#you should set LABELED to be false and EDGE_INDUCED to be true in src/config.h, \
#and then recompile the project

#For more details about how to configure and reproduce the results in the paper, \
#you can follow the instructions at the bottom of README.md
vim src/config.h
```

Go to home directory of STMatch and compile the project
```Shell
make clean
make
```

## Test STMatch

We provide a shell script for batched test, or you can input the command to test a specific pattern and graph directly.
We also provide the instructions about how to reproduce the result of Table III, Table IV and Figure 10 in the paper. 

### Run batched test
Batched test could test all given patterns for one input graph by executing a shell script. 
It will print out 24 lines for 24 given patterns in ./data/pattern/ directory. 
```Shell
#For example, if you neet to test graph wiki-Vote, you should execute bash batch_test.sh ./data/bin_graph/wiki-Vote/
bash batch_test.sh  /path/to/converted/graph/directory/
```

### Run single test
Single test could test one specific pattern for one input graph.
It will only print out one line for the input pattern. 
```Shell
#For example, if you need to test graph Enron and pattern 1.g, 
# you should execute ./cu_test ./data/bin_graph/email-Enron/snap.txt ./data/pattern/1.g 
./cu_test /path/to/converted/graph/directory/snap.txt /path/to/pattern/file/
```

### Reproducing the results of Figure 11
```Shell
#1. Configure src/config.h 
#For 1 GPU:
#set NUM_GPU=1 LABELED = true,  EDGE_INDUCED = true, UNROLL = 8, \
#STEAL_IN_BLOCK = true, STEAL_ACROSS_BLOCK = false 

#For 2 GPU:
#set NUM_GPU=2 LABELED = true,  EDGE_INDUCED = true, UNROLL = 8, \
#STEAL_IN_BLOCK = true, STEAL_ACROSS_BLOCK = false 

#For 4 GPU:
#set NUM_GPU=4 LABELED = true,  EDGE_INDUCED = true, UNROLL = 8, \
#STEAL_IN_BLOCK = true, STEAL_ACROSS_BLOCK = false 
vim src/config.h 

#2. Recompile the program. First make clean please. 
make clean
make

#3. Run batched test script and specify a test graph in command line.
bash batch_test.sh ./data/bin_graph/com-orkut.ungraph/ 
bash batch_test.sh ./data/bin_graph/soc-LiveJournal1/ 
```