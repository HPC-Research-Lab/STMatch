#ifndef Z_COMPILER_H_
#define Z_COMPILER_H_

#include "pattern/StlPattern.h"
#include "util.h"

namespace libra{

class ZCompiler{
public:
    void compile(StlPattern& _stl_pattern);
    void clear(){
        partial_.clear();
        L_adj_matrix_.clear();
        vertex_order_.clear();
        order_map_.clear();
        valid_permute_.clear();
    }

    stl_pattern_node_t vertex_at(size_t _idx){
        return vertex_order_[_idx];
    }
    void print(){
        print2dim(L_adj_matrix_, "L_adj_matrix_");
        print1dim(partial_, "partial_");
        print1dim(vertex_order_, "vertex_order_");
    }

public:
    StlPattern* stl_pattern_;

    void _get_matching_order();
    void _topological_sorting();
    void _automorphic_filtering();
    void _apply_restriction();
    void _fill_partial();
    void _update_degree(std::vector<stl_pattern_node_t>& _degree, stl_pattern_node_t _removed_vertex);

    std::vector<stl_pattern_node_t> partial_;
    std::vector<std::vector<stl_pattern_node_t> > L_adj_matrix_;

    //std::vector<stl_pattern_node_t> vertex_degrees_
    std::vector<stl_pattern_node_t> vertex_order_;
    std::vector<stl_pattern_node_t> order_map_;
    std::vector<std::vector<stl_pattern_node_t> > valid_permute_;
};

}

#endif