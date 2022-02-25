#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <iostream>

#include "ZCompiler.h"
#include "util.h"

namespace libra{

void ZCompiler::compile(StlPattern& _stl_pattern){
  clear();
  stl_pattern_ = &_stl_pattern;

  L_adj_matrix_.resize(stl_pattern_->num_vertex(), 
      std::vector<stl_pattern_node_t>(stl_pattern_->num_vertex(), 0));



  this->_get_matching_order();
  //this->_topological_sorting();
  this->_automorphic_filtering();
  this->_apply_restriction();
  this->_fill_partial();
  //print2dim(L_adj_matrix_, "L_adj_matrix_");
}


void ZCompiler::_update_degree(std::vector<stl_pattern_node_t>& _degree, stl_pattern_node_t _removed_vertex){
    for(stl_pattern_node_t i=0; i<stl_pattern_->num_vertex(); i++){
        if(stl_pattern_->connected(i, _removed_vertex) && i!=_removed_vertex){
          _degree[i]--;
        }
    }
}

void ZCompiler::_get_matching_order(){

    stl_pattern_node_t root = stl_pattern_->max_degree_vertex();
    stl_pattern_node_t max_degree = stl_pattern_->max_degree();

  std::vector<stl_pattern_node_t> vertex_degrees_;
  vertex_degrees_.resize(stl_pattern_->num_vertex(), 0);
  for(stl_pattern_node_t i=0; i<stl_pattern_->num_vertex(); i++){
    vertex_degrees_[i] = stl_pattern_->num_neighbors(i);
  }


    std::queue<stl_pattern_node_t> q;
    q.push(root); 
    std::vector<stl_pattern_node_t> visited(stl_pattern_->num_vertex(), 0);
    while (!q.empty()) {
        int width = q.size();
        stl_pattern_node_t a = q.front();
        q.pop();
      
        _update_degree(vertex_degrees_, a);
        for(int i=0; i<width; i++){
            if (!visited[a]){
                vertex_order_.push_back(a);
            }
            visited[a] = 1;

            std::vector<int> nbs;
            for (stl_pattern_node_t b : stl_pattern_->neighbors(a)){
                if (!visited[b])
                  nbs.push_back(b);
            }
            std::sort(nbs.begin(), nbs.end(), [&](stl_pattern_node_t _a, stl_pattern_node_t _b){
                  return vertex_degrees_[(int)_a]>vertex_degrees_[(int)_b];
            });

            for(auto& k:nbs){
              q.push(k);
            }
        }
    }
}

void ZCompiler::_topological_sorting(){
    stl_pattern_node_t root = stl_pattern_->max_degree_vertex();
    stl_pattern_node_t max_degree = stl_pattern_->max_degree();

    std::queue<stl_pattern_node_t> q;
    q.push(root); 
    std::vector<stl_pattern_node_t> visited(stl_pattern_->num_vertex(), 0);
    while (!q.empty()) {
        stl_pattern_node_t a = q.front();
        q.pop();
        if (!visited[a]){
            vertex_order_.push_back(a);
        }
        visited[a] = 1;
        for (stl_pattern_node_t b : stl_pattern_->neighbors(a)){
            if (!visited[b])
            q.push(b);
          }
    }
}

void ZCompiler::_automorphic_filtering(){
    std::vector<stl_pattern_node_t> p1;
    for (stl_pattern_node_t i = 0; i < stl_pattern_->num_vertex(); i++)
      p1.push_back(i);
    std::vector<std::vector<stl_pattern_node_t>> permute;

    do {
      permute.push_back(p1);
    } while ( std::next_permutation(p1.begin(),p1.end()) );
    
    order_map_.resize(stl_pattern_->num_vertex());
    for (stl_pattern_node_t i = 0; i < stl_pattern_->num_vertex(); i++)
        order_map_[vertex_order_[i]] = i;
    
    for (auto pp : permute) {
      
        std::vector<std::vector<stl_pattern_node_t>> adj_tmp(stl_pattern_->num_vertex());
        for (stl_pattern_node_t i = 0; i < stl_pattern_->num_vertex(); i++) {
            std::set<stl_pattern_node_t> tp;
            for (stl_pattern_node_t j : stl_pattern_->neighbors(i)){
                tp.insert(pp[j]);
            }
            adj_tmp[pp[i]] = std::vector<stl_pattern_node_t>(tp.begin(), tp.end());
        }
        bool valid = true;
        for (stl_pattern_node_t i = 0; i < stl_pattern_->num_vertex(); i++) {
          if (adj_tmp[i] != stl_pattern_->neighbors(i)) {
            valid = false;
            break;
          }
        }
        if (valid)
          valid_permute_.push_back(pp);
    }
}

void ZCompiler::_apply_restriction(){
    for (stl_pattern_node_t v : vertex_order_) {
        std::vector<std::vector<stl_pattern_node_t>> stabilized_aut;
        for (auto& x : valid_permute_) {
          if (x[v] == v) {
            stabilized_aut.push_back(x);
          }
          else {
            L_adj_matrix_[order_map_[v]][order_map_[x[v]]] = 1;
            /*
            L_.insert(std::make_pair(
               order_map_[v], order_map_[x[v]]));
               */
          }
        }
        valid_permute_ = stabilized_aut;
    }
}

void ZCompiler::_fill_partial(){
    partial_.resize(stl_pattern_->num_vertex());
    int pivot = -1;
    for(stl_pattern_node_t level = 0; level<stl_pattern_->num_vertex(); level++){
      pivot=-1;
      for (int j = level-1; j>=0; j--) {
          if (L_adj_matrix_[j][level]==true){
              pivot = j;
              break;
          }
      }
      partial_[level] = (stl_pattern_node_t)pivot;
    }
}

}