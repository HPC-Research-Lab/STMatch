#ifndef STL_GRAPH_H_
#define STL_GRAPH_H_

#include <vector>

#include "config.h"

namespace libra {
/**
 *StlGraph, Using std and C++20 Syntax.
*/
class StlGraph {
public:
  StlGraph() = default;
  void readfile(const char* _filename);
  void print();

  inline graph_node_t num_node(){
    return this->num_node_;
  }
  inline graph_edge_t num_edge() {
    return this->num_edge_;
  }
  inline graph_node_t max_degree() {
    return this->max_degree_;
  }
  inline graph_node_t* rowptr() {
    return (this->rowptr_).data();
  }
  inline graph_node_t* colidx() {
    return (this->colidx_).data();
  }
  inline size_t rowptr_size() {
    return (this->rowptr_).size();
  }
  inline size_t colidx_size() {
    return (this->colidx_).size();
  }

private:
  void _clear();

  std::vector<graph_label_t> node_label_;
  std::vector<std::vector<graph_node_t> > adj_list_;

  std::vector<graph_node_t> rowptr_;
  std::vector<graph_node_t> colidx_;

  graph_node_t num_node_ = 0;
  graph_edge_t num_edge_ = 0;
  graph_node_t max_degree_ = 0;
};
}  /** namespace libra */

#endif