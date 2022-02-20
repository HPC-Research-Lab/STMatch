#pragma once

namespace libra {

  inline constexpr size_t PAT_SIZE = 8;
  inline constexpr size_t GRAPH_DEGREE = 4096;

  inline constexpr int GRID_DIM = 1;
  inline constexpr int BLOCK_DIM = 32;
  inline constexpr int WARP_SIZE = 32;
  inline constexpr int NWARPS_PER_BLOCK = (BLOCK_DIM / WARP_SIZE);
  inline constexpr int NWARPS_TOTAL = ((GRID_DIM * BLOCK_DIM + WARP_SIZE - 1) / WARP_SIZE);

  typedef unsigned int graph_node_t;
  typedef unsigned long graph_edge_t; 
  typedef unsigned char pattern_node_t;
  typedef unsigned char label_t;

  typedef int set_op_t;

}