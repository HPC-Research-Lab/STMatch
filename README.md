## Overview
STMatch is a stack-based subgraph pattern matching system on GPU. 

## Build STMatch

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

Now you can see some directories in ~/project/data/bin_graph/
```Shell
#it's like "com-amazon.ungraph  com-dblp.ungraph  com-youtube.ungraph  email-Enron  wiki-Vote"
```

### Compile STMatch
Go to home directory of STMatch and compile the project
```Shell
make clean
#Be careful, don't use make's multi-threads execution like make -j 16. 
make
```
Now you can see 7 executable files ending with .exe in ./bin directory. 


## Test STMatch

We provide some shell scripts for batched test, or you can input the command to test a specific pattern and graph directly.
We also provide the instructions about how to reproduce the result of Table III, Table IV and Figure 10 in the paper. 


### Reproducing the results of Table III (a)
```Shell
#Run batched test script and specify a test graph in command line.
bash test_table3a.sh ./data/bin_graph/wiki-Vote/ 
bash test_table3a.sh ./data/bin_graph/email-Enron/ 
bash test_table3a.sh ./data/bin_graph/mico/ 
```

### Reproducing the results of Table III (b)
```Shell
#Run batched test script and specify a test graph in command line.
bash test_table3b.sh ./data/bin_graph/wiki-Vote/   
bash test_table3b.sh ./data/bin_graph/email-Enron/ 
bash test_table3b.sh ./data/bin_graph/mico/  
```

### Reproducing the results of Table IV
```Shell
#Run batched test script and specify a test graph in command line.
bash test_table4.sh ./data/bin_graph/wiki-Vote/ 
bash test_table4.sh ./data/bin_graph/email-Enron/ 
bash test_table4.sh ./data/bin_graph/com-youtube.ungraph/  
bash test_table4.sh ./data/bin_graph/mico/  
bash test_table4.sh ./data/bin_graph/soc-LiveJournal1/ 

#Before running the following test, \
# uncomment the links of Orkut and FriendSter in graph_converter/prepare_data.sh, \
# and download and convert them by executing bash prepare_data.sh under graph_converter directory. 
#It will take you about 1~2 hours and 50GB disk space to get the graph data. 
#IMPORTANT: When you test graph friendster, you should make a change in line 6 in file src/config.h
#Changing line 6 in file src/config.h from "typedef long graph_edge_t;" to "typedef unsigned long graph_edge_t;"
#Then re-make the program.
bash test_table4.sh ./data/bin_graph/com-orkut.ungraph/        
bash test_table4.sh ./data/bin_graph/com-friendster.ungraph/    
```

### Reproducing the results of Figure 10
```Shell
#Naive(Without Any Optimization) executions
bash test_fig10_naive.sh ./data/bin_graph/email-Enron/
bash test_fig10_naive.sh ./data/bin_graph/com-youtube.ungraph/
bash test_fig10_naive.sh ./data/bin_graph/mico/ 
bash test_fig10_naive.sh ./data/bin_graph/soc-LiveJournal1/ 

#Local Work Stealing(With Only One Optimization) executions
bash test_fig10_local.sh ./data/bin_graph/email-Enron/
bash test_fig10_local.sh  ./data/bin_graph/com-youtube.ungraph/
bash test_fig10_local.sh  ./data/bin_graph/mico/ 
bash test_fig10_local.sh  ./data/bin_graph/soc-LiveJournal1/ 

#[Local Work Stealing]+[Global Work Stealing] (With Two Optimizations) executions
bash test_fig10_local_global.sh ./data/bin_graph/email-Enron/
bash test_fig10_local_global.sh ./data/bin_graph/com-youtube.ungraph/
bash test_fig10_local_global.sh ./data/bin_graph/mico/ 
bash test_fig10_local_global.sh ./data/bin_graph/soc-LiveJournal1/ 

#[Local Work Stealing]+[Global Work Stealing]+[Loop Unrolling] (With Three Optimizations) executions
bash test_fig10_local_global_unroll8.sh ./data/bin_graph/email-Enron/
bash test_fig10_local_global_unroll8.sh ./data/bin_graph/com-youtube.ungraph/
bash test_fig10_local_global_unroll8.sh ./data/bin_graph/mico/ 
bash test_fig10_local_global_unroll8.sh ./data/bin_graph/soc-LiveJournal1/ 
```

## Test cuTS, GSI and Dryadic
Code and scripts for reproducing results of cuTS, GSI and Dryadic in Table 3a,  Table 3b and Table 4 are included in the directory ComparedSystems:
```Shell
    #Please follow the instructions in this directory to test cuTS, GSI and Dryadic. 
    cd ComparedSystems
```
