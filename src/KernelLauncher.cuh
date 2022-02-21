#pragma once

#include "PatternMatcher.cuh"
#include "GraphPreprocessor.h"
#include "PatternPreprocessor.h"
#include "JobQueue.cuh"
#include <cuda.h>


namespace libra {

  __global__ void _parallel_match(Graph* dev_graph, Pattern* dev_pattern, CallStack* dev_callstack, JobQueue* job_queue);

  struct KernelLauncher {

    Graph* gpu_graph_;
    Pattern* gpu_pattern_;
    CallStack* gpu_callstack_;
    JobQueue* job_queue_;

    KernelLauncher(GraphPreprocessor& g, PatternPreprocessor& pat) {

      // copy graph and pattern to GPU global memory
      gpu_graph_ = g.to_gpu();
      gpu_pattern_ = pat.to_gpu();

      // allocate the callstack for all warps in global memory
      graph_node_t* slot_storage;
      cudaMalloc(&slot_storage, sizeof(graph_node_t) * NWARPS_TOTAL * PAT_SIZE * PAT_SIZE * GRAPH_DEGREE);

      std::vector<CallStack> stk(NWARPS_TOTAL);

      for (int i = 0; i < NWARPS_TOTAL; i++) {
        auto &s = stk[i];
        memset(s.path, 0, sizeof(s.path));
        memset(s.iter, 0, sizeof(s.iter));
        memset(s.slot_size, 0, sizeof(s.slot_size));
        s.slot_storage = (graph_node_t(*)[PAT_SIZE][GRAPH_DEGREE])(slot_storage + i * sizeof(graph_node_t) * PAT_SIZE * PAT_SIZE * GRAPH_DEGREE);
      }
      cudaMalloc(&gpu_callstack_, NWARPS_TOTAL * sizeof(CallStack));
      cudaMemcpy(gpu_callstack_, stk.data(), sizeof(CallStack) * NWARPS_TOTAL, cudaMemcpyHostToDevice);

      job_queue_ = JobQueue(g.g).to_gpu();
    }

    void start() {

      cudaEvent_t start, stop;
      cudaEventCreate(&start);
      cudaEventCreate(&stop);

      cudaEventRecord(start);

      std::cout << "shared memory usage: " << sizeof(Graph) << " " << sizeof(Pattern) << " " << sizeof(PatternMatcher) * NWARPS_PER_BLOCK << std::endl;

      _parallel_match << <GRID_DIM, BLOCK_DIM >> > (gpu_graph_, gpu_pattern_, gpu_callstack_, job_queue_);

      cudaDeviceSynchronize();

      cudaEventRecord(stop);

      float milliseconds = 0;
      cudaEventElapsedTime(&milliseconds, start, stop);
      printf("matching time: %f ms\n", milliseconds);
    }
  };
}