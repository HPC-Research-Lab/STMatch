
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "StlGraph.h"

namespace libra {

void StlGraph::_clear(){
    node_label_.clear();
    adj_list_.clear();
    rowptr_.clear();
    colidx_.clear();
}

void StlGraph::readfile(const char* _filename){
    _clear();
    std::ifstream ifs(_filename, std::ifstream::in);
    graph_node_t v1, v2;
    for (std::string line; std::getline(ifs, line);) {
        if(line[0]=='#') continue;
        else{
            std::istringstream iss(line);
            graph_node_t n1, n2;
            iss>>n1>>n2;
            //std::cout<<n1<<" "<<n2<<std::endl;
            if(adj_list_.size()<n1+1) adj_list_.resize(n1+1);
            if(adj_list_.size()<n2+1) adj_list_.resize(n2+1);
            adj_list_[n1].push_back(n2);
            adj_list_[n2].push_back(n1);
        }
    }
    rowptr_.push_back(0);
    std::for_each(adj_list_.begin(), adj_list_.end(), [&](auto& _adj){
        std::sort(_adj.begin(), _adj.end());
        int pos = 0;
        for (size_t j = 1; j < _adj.size(); j++) {
            if (_adj[j] != _adj[pos]) _adj[++pos] = _adj[j];
        }
        _adj.resize(pos+1);
        if(_adj.size()>max_degree_) max_degree_=_adj.size();

        if (_adj.size() > 0){
            colidx_.insert(colidx_.end(), _adj.begin(), _adj.end());
        }
        rowptr_.push_back(colidx_.size());
    });

    num_node_ = adj_list_.size();
    num_edge_ = colidx_.size()/2;

   

}

void StlGraph::print(){
    printf("StlGraph: num_node_:%u, num_edge_:%lu, max_degree_:%u\n", num_node_, num_edge_, max_degree_);
}

} /** namespace libra */