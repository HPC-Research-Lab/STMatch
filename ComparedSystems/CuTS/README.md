# AE cuTS: Scaling Subgraph Isomorphism on Distributed Multi-GPU Systems Using Trie Based Data Structure

This repository contains the code for the "cuTS: Scaling Subgraph Isomorphism on Distributed Multi-GPU Systems Using Trie Based Data Structure" framework. The cuTS framework is an efficient subgraph isomorphism solver for GPUs. 


## Package requirements:
* cmake(>=3.10)
* CUDA(>=10.0)

We used `cmake 3.17.2`, `openmpi/3.0.1` and `CUDA 11.0`

    
## Reprooduce the results in table 3a
All the test graphs and patterns are uploaded to this repo. The only thing you need to do is to compile the program. 
    
    mkdir build
    cd build

Now an executable file cuts was generated. 

Run the following commands to reproduce the results in table 3a.
    
    ./build/cuts ./graph/wiki-Vote.g ./pattern/1.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/2.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/3.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/4.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/5.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/6.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/7.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/8.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/9.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/10.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/11.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/12.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/13.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/14.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/15.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/16.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/17.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/18.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/19.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/20.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/21.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/22.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/23.g
    ./build/cuts ./graph/wiki-Vote.g ./pattern/24.g

    ./build/cuts ./graph/enron.g ./pattern/1.g
    ./build/cuts ./graph/enron.g ./pattern/2.g
    ./build/cuts ./graph/enron.g ./pattern/3.g
    ./build/cuts ./graph/enron.g ./pattern/4.g
    ./build/cuts ./graph/enron.g ./pattern/5.g
    ./build/cuts ./graph/enron.g ./pattern/6.g
    ./build/cuts ./graph/enron.g ./pattern/7.g
    ./build/cuts ./graph/enron.g ./pattern/8.g
    ./build/cuts ./graph/enron.g ./pattern/9.g
    ./build/cuts ./graph/enron.g ./pattern/10.g
    ./build/cuts ./graph/enron.g ./pattern/11.g
    ./build/cuts ./graph/enron.g ./pattern/12.g
    ./build/cuts ./graph/enron.g ./pattern/13.g
    ./build/cuts ./graph/enron.g ./pattern/14.g
    ./build/cuts ./graph/enron.g ./pattern/15.g
    ./build/cuts ./graph/enron.g ./pattern/16.g
    ./build/cuts ./graph/enron.g ./pattern/17.g
    ./build/cuts ./graph/enron.g ./pattern/18.g
    ./build/cuts ./graph/enron.g ./pattern/19.g
    ./build/cuts ./graph/enron.g ./pattern/20.g
    ./build/cuts ./graph/enron.g ./pattern/21.g
    ./build/cuts ./graph/enron.g ./pattern/22.g
    ./build/cuts ./graph/enron.g ./pattern/23.g
    ./build/cuts ./graph/enron.g ./pattern/24.g

    ./build/cuts ./graph/mico.g ./pattern/1.g
    ./build/cuts ./graph/mico.g ./pattern/2.g
    ./build/cuts ./graph/mico.g ./pattern/3.g
    ./build/cuts ./graph/mico.g ./pattern/4.g
    ./build/cuts ./graph/mico.g ./pattern/5.g
    ./build/cuts ./graph/mico.g ./pattern/6.g
    ./build/cuts ./graph/mico.g ./pattern/7.g
    ./build/cuts ./graph/mico.g ./pattern/8.g
    ./build/cuts ./graph/mico.g ./pattern/9.g
    ./build/cuts ./graph/mico.g ./pattern/10.g
    ./build/cuts ./graph/mico.g ./pattern/11.g
    ./build/cuts ./graph/mico.g ./pattern/12.g
    ./build/cuts ./graph/mico.g ./pattern/13.g
    ./build/cuts ./graph/mico.g ./pattern/14.g
    ./build/cuts ./graph/mico.g ./pattern/15.g
    ./build/cuts ./graph/mico.g ./pattern/16.g
    ./build/cuts ./graph/mico.g ./pattern/17.g
    ./build/cuts ./graph/mico.g ./pattern/18.g
    ./build/cuts ./graph/mico.g ./pattern/19.g
    ./build/cuts ./graph/mico.g ./pattern/20.g
    ./build/cuts ./graph/mico.g ./pattern/21.g
    ./build/cuts ./graph/mico.g ./pattern/22.g
    ./build/cuts ./graph/mico.g ./pattern/23.g
    ./build/cuts ./graph/mico.g ./pattern/24.g
    

