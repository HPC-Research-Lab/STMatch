

#include <algorithm>
#include <fstream>
#include <iostream>

#include "StlPattern.h"
#include "util.h"

namespace libra {

void StlPattern::readfile(const char* _filename){
    _clear();
    
    std::ifstream ifs(_filename, std::ifstream::in);
    char t; stl_pattern_node_t v1, v2;
    while(!ifs.eof()){
        ifs>>t;
        
        if(t=='#') continue;
        else if(t=='v') num_vertex_++;
        else if(t=='e'){
            vertex_adj_matrix_.resize(num_vertex_, 
                std::vector<stl_pattern_node_t>(num_vertex_, 0));
            vertex_adj_list_.resize(num_vertex_);
            ifs>>v1>>v2;
            vertex_adj_list_[v1].push_back(v2);
            vertex_adj_list_[v2].push_back(v1);
            vertex_adj_matrix_[v1][v2]=1;
            vertex_adj_matrix_[v2][v1]=1;
        }
    }

    std::for_each(vertex_adj_list_.begin(), vertex_adj_list_.end(), [](auto& _item){
        std::sort(_item.begin(), _item.end());
    });
    stl_pattern_node_t iter_idx = 0;
    std::for_each(vertex_adj_matrix_.begin(), 
        vertex_adj_matrix_.end(), [&](auto& _k){
        stl_pattern_node_t d = (stl_pattern_node_t)std::count(_k.begin(), _k.end(), 1);
        if(d>max_degree_){
            max_degree_ = d;
            max_degree_vertex_ = iter_idx;
        }
        iter_idx++;
    });
    ifs.close();
}
} /** namespace libra */