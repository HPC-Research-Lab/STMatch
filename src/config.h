#pragma once

namespace STMatch {


  typedef int graph_node_t;
  typedef unsigned long graph_edge_t;
  typedef char pattern_node_t;
  typedef char set_op_t;
  typedef unsigned int bitarray32;

  inline constexpr size_t PAT_SIZE = 7;
  inline constexpr size_t GRAPH_DEGREE = 4096;
  inline constexpr size_t MAX_SLOT_NUM = 15;

  inline constexpr bool LABELED = false;
  inline constexpr bool EDGE_INDUCED = true;

  inline constexpr int NUM_GPU = 2;
  inline constexpr int GRID_DIM = 82;
  inline constexpr int BLOCK_DIM = 1024;
  inline constexpr int WARP_SIZE = 32;
  inline constexpr int NWARPS_PER_BLOCK = (BLOCK_DIM / WARP_SIZE);
  //inline constexpr int NWARPS_TOTAL_PER_GPU = ((GRID_DIM * BLOCK_DIM + WARP_SIZE - 1) / WARP_SIZE);
  inline constexpr int NWARPS_TOTAL = ((GRID_DIM * BLOCK_DIM + WARP_SIZE - 1) / WARP_SIZE);

  inline constexpr graph_node_t JOB_CHUNK_SIZE = 8;
  //static_assert(2 * JOB_CHUNK_SIZE <= GRAPH_DEGREE); 

  // this is the maximum unroll size
  inline constexpr int UNROLL = 8;

  inline constexpr bool STEAL_IN_BLOCK = true;
  inline constexpr bool STEAL_ACROSS_BLOCK = true;
  inline constexpr int DETECT_LEVEL = 1;
  inline constexpr int STOP_LEVEL = 2;
}
