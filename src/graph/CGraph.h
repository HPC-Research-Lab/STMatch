#ifndef GRAPH_H_
#define GRAPH_H_

#include "config.h"
#include <stdio.h>

#ifdef __CUDACC__
#define __DEVICE__  __device__
#else
#define __DEVICE__
#endif

namespace libra{

struct NeighborInfo{
    graph_node_t* ptr_;
    graph_node_t size_;
};

struct CGraph{
    graph_node_t rowptr_size_;
    graph_edge_t colidx_size_;
    graph_node_t* rowptr_;
    graph_node_t* colidx_;

    graph_node_t num_node_;
    graph_edge_t num_edge_;
    graph_node_t max_degree_;
};



__DEVICE__ inline void print_graph(CGraph* _graph);
__DEVICE__ inline void move_graph_pointer();
__DEVICE__ inline NeighborInfo get_neighbors_ptr(CGraph& _graph, graph_node_t _node_id);


__DEVICE__
void print_graph(CGraph* _graph){
    
    //printf("rowptr_size_:%d, colidx_size_:%lu, num_node_:%u, num_edge_:%lu, max_degree_:%d\n", _graph->rowptr_size_, _graph->colidx_size_, _graph->num_node_, _graph->num_edge_, _graph->max_degree_);

    //printf("start_:%lu, rowptr_:%lu, colidx_:%lu\n", _graph, _graph->rowptr_, _graph->colidx_);
    for(size_t i=0; i< _graph->rowptr_size_-1; i++){
        //printf("Node:%lu, idx: %u to  %u \n", i, _graph->rowptr_[i], _graph->rowptr_[i+1]);
        for(size_t j=_graph->rowptr_[i]; j< _graph->rowptr_[i+1]; j++){
            //printf("%u ", _graph->colidx_[j]);
        } 
        //printf("\n\n");   
    }
    //printf("\n");
}

__DEVICE__
NeighborInfo get_neighbors_ptr(CGraph& _graph, graph_node_t _node_id){
    graph_node_t start = ((graph_node_t*)_graph.rowptr_)[_node_id];
    graph_node_t end =   ((graph_node_t*)_graph.rowptr_)[_node_id + 1];
    return {(graph_node_t*)_graph.colidx_ + start, end-start};
}
} /** namespace libra */
#endif