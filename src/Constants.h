#pragma once

namespace libra {
  inline constexpr size_t MAX_PAT_SIZE = 8;
  inline constexpr size_t MAX_GRAPH_DEGREE = 5000;

#ifdef __CUDACC__

  inline constexpr int GRID_DIM = 1;
  inline constexpr int BLOCK_DIM = 32;
  inline constexpr int WARP_SIZE = 32;
  inline constexpr int NWARPS_PER_BLOCK = (BLOCK_DIM / WARP_SIZE);
  inline constexpr int NWARPS_TOTAL = ((GRID_DIM * BLOCK_DIM + WARP_SIZE - 1) / WARP_SIZE);

#endif
}