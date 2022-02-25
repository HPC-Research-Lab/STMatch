  
#ifndef STL_PATTERN_H_
#define STL_PATTERN_H_
#include <vector>

#include "config.h"

namespace libra {
/*
 * STLPattern, Using std and C++20 Syntax. 
 */
class StlPattern{
public:
     StlPattern(){}
     void print(){
          for(size_t i=0; i<vertex_adj_list_.size(); i++){
               printf("Neighbor of %lu: ", i);
               for(size_t j=0; j<vertex_adj_list_[i].size(); j++){
                    printf("%d ", vertex_adj_list_[i][j]);
               }
               printf("\n");
          }
     }
     void readfile(const char* _filename);
     bool connected(stl_pattern_node_t _i, stl_pattern_node_t _j){
          return vertex_adj_matrix_[(int)_i][(int)_j];
     }
     stl_pattern_node_t num_vertex(){
          return num_vertex_;
     }
     std::vector<stl_pattern_node_t> neighbors(size_t _idx){
          return vertex_adj_list_[_idx];
     }
     stl_pattern_node_t num_neighbors(stl_pattern_node_t _idx){
          return (stl_pattern_node_t)vertex_adj_list_[(int)_idx].size();
     }
     stl_pattern_node_t max_degree(){
          return max_degree_;
     }
     stl_pattern_node_t max_degree_vertex(){
          return max_degree_vertex_;
     }
private: 
     void _clear(){
          vertex_adj_list_.clear();
          vertex_adj_matrix_.clear();
          num_vertex_ = 0;
          max_degree_ = 0;
          max_degree_vertex_ = 0;
     }
    std::vector<std::vector<stl_pattern_node_t> > vertex_adj_list_;
    std::vector<std::vector<stl_pattern_node_t> > vertex_adj_matrix_;
    stl_pattern_node_t num_vertex_ = 0;
    stl_pattern_node_t max_degree_ = 0;
    stl_pattern_node_t max_degree_vertex_ = 0;

};
}/** namespace libra */

#endif
