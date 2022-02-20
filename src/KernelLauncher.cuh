#pragma once

#include "PatternMatcher.h"
#include "GraphPreprocessor.h"
#include "PatternPreprocessor.h"
#include "JobQueue.cuh"

#include <cuda.h>


namespace libra {


#ifdef __CUDACC__

  __global__ void _parallel_match(Graph* dev_graph, Pattern* dev_pattern, CallStack* dev_callstack, JobQueue* job_queue);
#else

#endif

  struct KernelLauncher {

    Graph* gpu_graph_;
    Pattern* gpu_pattern_;
    CallStack* gpu_callstack_;
    JobQueue* job_queue_;

    KernelLauncher(Graph& g, Pattern& pat) {

      // copy graph and pattern to GPU global memory
      gpu_graph_ = g.to_gpu();
      gpu_pattern_ = pat.to_gpu();

      // allocate the callstack for all warps in global memory
      size_t* slot_storage;
      cudaMalloc(&slot_storage, sizeof(size_t) * NWARPS_TOTAL * MAX_PAT_SIZE * MAX_PAT_SIZE * MAX_GRAPH_DEGREE);
      std::vector<CallStack> stk(NWARPS_TOTAL);
      for (int i = 0; i < NWARPS_TOTAL; i++) {
        stk[i].slot_storage = (size_t(*)[MAX_PAT_SIZE][MAX_GRAPH_DEGREE])(slot_storage + i * sizeof(size_t) * MAX_PAT_SIZE * MAX_PAT_SIZE * MAX_GRAPH_DEGREE);
      }
      cudaMalloc(&gpu_callstack_, NWARPS_TOTAL * sizeof(CallStack));
      cudaMemcpy(gpu_callstack_, stk.data(), sizeof(CallStack) * NWARPS_TOTAL, cudaMemcpyHostToDevice);

      job_queue_ = JobQueue(g).to_gpu();
    }

    void start() {

      //    cudaEvent_t start, stop;
     //     cudaEventCreate(&start);
    //      cudaEventCreate(&stop);

       //   cudaEventRecord(start);

      std::cout << sizeof(Graph) << " " << sizeof(Pattern) << " " << sizeof(PatternMatcher) * NWARPS_PER_BLOCK << std::endl;

      _parallel_match << <GRID_DIM, BLOCK_DIM, sizeof(Graph) + sizeof(Pattern) + sizeof(PatternMatcher) * NWARPS_PER_BLOCK >> > (gpu_graph_, gpu_pattern_, gpu_callstack_, job_queue_);

      cudaDeviceSynchronize();

      // cudaDeviceSynchronize();
      // cudaEventRecord(stop);

 //      float milliseconds = 0;
   //    cudaEventElapsedTime(&milliseconds, start, stop);
    //   printf("matching time: %f ms\n", milliseconds);
    }




    // __global__ void _ctx_init() {
  //   int tid = blockIdx.x * blockDim.x + threadIdx.x;
  //   // JIANG: we shouldn't assume a threadblock has only one warp
  //   if (threadIdx.x % WARP_SIZE == 0) {
  //     int warp_num = (gridDim.x * blockDim.x + WARP_SIZE - 1) / WARP_SIZE;
  //     int wid = tid / WARP_SIZE;

  //     int width = (dev_graph_->nnodes + warp_num - 1) / warp_num;
  //     int start = wid * width;
  //     int end = (wid + 1) * width;
  //     // JIANG: is this necessary? start should always be smaller than N
  //     // start = start < dev_graph_->num_vertex() ? start : dev_graph_->num_vertex(); 
  //     end = end < dev_graph_->nnodes ? end : dev_graph_->nnodes;
  //   }
  // }


  // __global__ void  _print_res() {

  //   int warp_num = (GRID_DIM * BLOCK_DIM + WARP_SIZE - 1) / WARP_SIZE;
  //   //printf("warp_num:%d\n", warp_num);
  //   long long  res = 0;
  //   for (int i = 0; i < warp_num; i++) {
  //     TriePatternMatcher& pm = dev_pm_list_[i];
  //     res += pm.count_;
  //   }
  //   printf("count:%lu\n", res);
  //   //printf("finish:%d, count_:%d, start:%d, end:%d, \n", wid, count_[wid], start, end);

  // }

  // __global__ void test(int* _ptr, int size) {
  //   while (true) {
  //     for (int i = 0; i < size; i++) {
  //       printf("%d\n", i);
  //       _ptr[i] = i;
  //     }
  //   }
  // }

   // __device__ bool _do_load_balance(TriePatternMatcher& pm) {

  //   //printf("start:%d, end:%d\n", pm.stk_->layer0_start_, pm.stk_->layer0_width_);

  //   int tid = blockIdx.x * blockDim.x + threadIdx.x;
  //   int wid = tid / WARP_SIZE;
  //   int max_left_task = 0;

  //   int idx = wid;

  //   int warp_num = (gridDim.x * blockDim.x + WARP_SIZE - 1) / WARP_SIZE;
  //   for (int i = 0; i < warp_num; i++) {

  //     TriePatternMatcher& target_pm = dev_pm_list_[i];

  //     //printf("num_left_task:%d\n", target_pm.num_left_task());
  //     if (i == wid) continue;
  //     if (*(target_pm.level_) == -1
  //       || *(target_pm.level_) == 0) continue;

  //     int left_task = target_pm.num_left_task();
  //     //int left_task = 0;
  //     if (left_task > max_left_task) {
  //       max_left_task = left_task;
  //       idx = i;
  //     }
  //   }
  //   //printf("max_left_task:%d\n", max_left_task);
  //   //return false;
  //   if (idx == wid) return false;
  //   TriePatternMatcher& target_pm = dev_pm_list_[idx];
  //   //kstd::omp_lock_guard<omp_lock_t> lock(pm.omp_lock_);
  //   bool res = false;
  //   if (target_pm.trans_ctx_layer0(pm)) {
  //     res = true;
  //   }
  //   return res;
  // }



  };
}