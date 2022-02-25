#ifndef GRAPH_PREPROCESSOR_H_
#define GRAPH_PREPROCESSOR_H_

#include <string>

#include <cstdlib>
#include <cstring>
#include <cuda.h>

#include "graph/StlGraph.h"
#include "config.h"

namespace libra{

typedef struct {
    graph_node_t* rowptr;
    graph_node_t* colidx;
    graph_node_t nnodes = 0;
    graph_edge_t nedges = 0;
    graph_node_t max_degree;
    label_t* vertex_label;

  } Graph;

struct GraphDataManager{
    GraphDataManager(){}
    void read_file(const char* _filename){
        stl_graph_.readfile(_filename);
        filename_=std::string(_filename);
    }
    size_t total_size(){
        return end_offset();
    }
    size_t graph_offset(){
        return 0;
    }
    size_t rowptr_offset(){
        return sizeof(Graph);
    }
    size_t colidx_offset(){
        return sizeof(Graph)+
                stl_graph_.rowptr_size()*sizeof(graph_node_t);
    }
    size_t end_offset(){
        return sizeof(Graph)+
                stl_graph_.rowptr_size()*sizeof(graph_node_t)+
                stl_graph_.colidx_size()*sizeof(graph_node_t);
    }

    void set_graph_info(Graph* _graph){
        _graph->nnodes = stl_graph_.num_node();
        _graph->nedges = stl_graph_.num_edge();
        _graph->max_degree = stl_graph_.max_degree();
        memcpy(_graph->rowptr, stl_graph_.rowptr(), stl_graph_.rowptr_size()*sizeof(graph_node_t));
        memcpy(_graph->colidx, stl_graph_.colidx(), stl_graph_.colidx_size()*sizeof(graph_node_t));
    }

    void set_graph_ptr(Graph* _graph, void* _start_ptr){
        _graph->rowptr = (graph_node_t*)((char*)_start_ptr+rowptr_offset());
        _graph->colidx = (graph_node_t*)((char*)_start_ptr+colidx_offset());
    }


    StlGraph stl_graph_;
    std::string filename_;
};

class GraphPreprocessor{

public:
    GraphPreprocessor(){

    }
    ~GraphPreprocessor() {
        _free();
    }
    void readfile(const char* _filename){
        _free();
        gm_.read_file(_filename);
    }

    Graph* to_gpu(){
        return (Graph*)gpu_data();
    }
    Graph* cpu_data(){
        if(!cpu_graph_data_){
            cpu_graph_data_ = malloc(gm_.total_size());
            gm_.set_graph_ptr((Graph*)cpu_graph_data_, (void*)cpu_graph_data_);
            gm_.set_graph_info((Graph*)cpu_graph_data_);
        }
        return (Graph*)cpu_graph_data_;
    }

    CUdeviceptr gpu_data(){
        if(!cpu_graph_data_){
            cpu_data();
        }
        if(!gpu_graph_data_){
            checkCudaErrors( cuMemAlloc(&gpu_graph_data_, gm_.total_size()) );
            gm_.set_graph_ptr((Graph*)cpu_graph_data_, (void*)gpu_graph_data_);
            checkCudaErrors (cuMemcpyHtoD(gpu_graph_data_, cpu_graph_data_, gm_.total_size()) );
            gm_.set_graph_ptr((Graph*)cpu_graph_data_, (void*)cpu_graph_data_);
        }
        return gpu_graph_data_;
    }

private:
    void _free(){
        if(cpu_graph_data_) free(cpu_graph_data_);
        if(gpu_graph_data_) cuMemFree(gpu_graph_data_);
    }

    Graph ret_g_;
    GraphDataManager gm_;
    void* cpu_graph_data_ = nullptr;
    CUdeviceptr gpu_graph_data_ = 0;
};
}
#endif