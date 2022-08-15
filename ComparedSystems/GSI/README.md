## Reproduce the results of GSI in table 4
    
    mkdir build
    cd build
    cmake ..
    make

Now an executable file cuts is generated. 

Decompress the graph `youtube' in directory ./test_data/graph

    tar -xzvf youtube.g.tar.gz 

Run the following commands to reproduce the results of GSI in table 3a.
    
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/1.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/2.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/3.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/4.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/5.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/6.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/7.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/8.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/9.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/10.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/11.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/12.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/13.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/14.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/15.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/16.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/17.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/18.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/19.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/20.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/21.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/22.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/23.g
    ./GSI.exe ./test_data/graph/WikiVote ./test_data/pattern/24.g

    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/1.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/2.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/3.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/4.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/5.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/6.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/7.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/8.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/9.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/10.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/11.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/12.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/13.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/14.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/15.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/16.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/17.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/18.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/19.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/20.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/21.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/22.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/23.g
    ./GSI.exe ./test_data/graph/Enron.g ./test_data/pattern/24.g

    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/1.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/2.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/3.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/4.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/5.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/6.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/7.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/8.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/9.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/10.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/11.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/12.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/13.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/14.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/15.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/16.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/17.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/18.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/19.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/20.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/21.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/22.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/23.g
    ./GSI.exe ./test_data/graph/youtube.g ./test_data/pattern/24.g


