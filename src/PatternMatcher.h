#pragma once

#include "CallStack.h"
#include "GraphPreprocessor.h"
#include "PatternPreprocessor.h"
#include "Util.h"
#include "Constants.h"
#include "JobQueue.cuh"
#include <climits>
#include <omp.h>

#ifdef __CUDACC__
#define __DEVICE__ __device__
#else
#define __DEVICE__
#endif

namespace libra {

#ifdef __CUDACC__

  __device__ void lower_bound(size_t* nums, size_t& nums_size, size_t target);
  __device__ size_t intersection(size_t* _first, size_t* _second, size_t _first_size, size_t _second_size, size_t* _res, size_t _up);
  __device__ size_t difference(size_t* _first, size_t* _second, size_t _first_size, size_t _second_size, size_t* _res, size_t _up);

#else
  size_t intersection(size_t* first, size_t* second, size_t first_size, size_t second_size, size_t* res, size_t up);
  inline int difference(size_t* first, size_t* second, int first_size, int second_size, size_t* res, size_t up);
  size_t lower_bound(size_t* nums, size_t nums_size, size_t target);

#endif

  struct PatternMatcher {

    // pointer to graph and pattern stored in shared memory for all warps in the threadblock
    Graph* graph_;
    Pattern* pattern_;
    // callstack stored in shared memory for this warp 
    CallStack stk_;
    JobQueue* job_queue_;

    // loop level
    int level_;
    
    size_t count_ = 0;

#ifdef __CUDACC__
    __device__ inline void match() {
      level_ = 0;

      while (true) {

        if (level_ < pattern_->nnodes - 1) {
          if (stk_.slot_size[level_][pattern_->candidate_idx_[level_]] == 0) {
            _extend();
            stk_.iter_[level_] = 0;
          }
          if (stk_.iter_[level_] < stk_.slot_size[level_][pattern_->candidate_idx_[level_]]) {
            stk_.path_[level_] = stk_.slot_storage[level_][pattern_->candidate_idx_[level_]][stk_.iter_[level_]];
            if (threadIdx.x % WARP_SIZE == 0)
              level_++;
            __syncwarp();
          }
          else {
            stk_.slot_size[level_][pattern_->candidate_idx_[level_]] = 0;
            if (threadIdx.x % WARP_SIZE == 0) {
              level_--;
            }
            __syncwarp();
            if (level_ < 0) {
              break;
            }
            if (threadIdx.x % WARP_SIZE == 0) stk_.iter_[level_]++;
            __syncwarp();
          }
        }
        else if (level_ == pattern_->nnodes - 1) {
          // TODO: we can save the storage of sets for the last level
          if (threadIdx.x % WARP_SIZE == 0) {
            printf("here\n");
            count_ += stk_.slot_size[level_][pattern_->candidate_idx_[level_]];
            stk_.slot_size[level_][pattern_->candidate_idx_[level_]] = 0;
            level_--;
          }
          __syncwarp();
        }
      }
      if (threadIdx.x == 0) printf("%lu\n", count_);
    }

    __device__ void _extend() {

      if (level_ == 0) {
        size_t cur_job; size_t njobs;

        // TODO: change this to multiple threads
        if (threadIdx.x % WARP_SIZE == 0) {
          job_queue_->get_job(cur_job, njobs);
          stk_.iter_[0] = 0;
          for (size_t i = 0; i < njobs; i++) {
            stk_.slot_storage[0][0][i] = job_queue_->queue_[cur_job + i];
          }
          stk_.slot_size[0][0] = njobs;
        }
        __syncwarp();
      }
      else {
        size_t* neighbor = graph_->rowptr_[stk_.path_[level_ - 1]] + graph_->colidx_;
        size_t neighbor_size = graph_->rowptr_[stk_.path_[level_ - 1] + 1] - graph_->rowptr_[stk_.path_[level_ - 1]];

        for (int i = 0; i < pattern_->dependency_[level_][0]; i++) {

          if (i == 0 && pattern_->first_positive_[level_] == 0) {
            continue;
          }
          if (i == 0 && pattern_->first_positive_[level_] == 1) {
            _extend_first_positive(0);
            continue;
          }
          int preced_idx = pattern_->dependency_[level_][2 * i + 1];
          int op = pattern_->dependency_[level_][2 * i + 2];

          // TODO: I guess this is computing the partial order upper bound for intermediate set, can this be simplified? 
          size_t ub = ULONG_MAX;
          for (int j = 0; j < MAX_PAT_SIZE; j++) {
            int t = pattern_->dependency_partial_[level_][i][j];
            if (t == -1) break;
            if (ub == ULONG_MAX) ub = stk_.path_[t];
            else
              ub = ((stk_.path_[t] > ub) ?
                stk_.path_[t] : ub);
          }

          if (op >= 0) {
            if (preced_idx == 0 && pattern_->first_positive_[level_ - 1] == 0) {
              _extend_first_positive(i);
            }
            else {
              stk_.slot_size[level_][i] = intersection(stk_.slot_storage[level_ - 1][preced_idx], neighbor, stk_.slot_size[level_ - 1][preced_idx], neighbor_size, stk_.slot_storage[level_][i], ub);
              printf("inters, %lu\n", stk_.slot_size[level_][i]);
            }
          }
          else if (op < 0) {
            stk_.slot_size[level_][i] = difference(stk_.slot_storage[level_ - 1][preced_idx], neighbor, stk_.slot_size[level_ - 1][preced_idx], neighbor_size, stk_.slot_storage[level_][i], ub);
            printf("diffs, %lu\n", stk_.slot_size[level_][i]);

          }
        }
      }
    }


    __device__ inline void _extend_first_positive(int idx) {

      for (int i = level_ - 1; i >= 0; i--) {
        size_t* neighbor = graph_->rowptr_[stk_.path_[i]] + graph_->colidx_;
        size_t neighbor_size = graph_->rowptr_[stk_.path_[i] + 1] - graph_->rowptr_[stk_.path_[i]];

        if (i == level_ - 1) {
          for (size_t j = 0; j < neighbor_size; j++) {
            stk_.slot_storage[level_][idx][j] = neighbor[j];
          }
          stk_.slot_size[level_][idx] = neighbor_size;
        }
        else {
          stk_.slot_size[level_][idx] = difference(stk_.slot_storage[level_][idx], neighbor, stk_.slot_size[level_][idx], neighbor_size, stk_.slot_storage[level_][idx], INT_MAX);
        }
      }
      if (pattern_->partial_[level_] != -1) {
        lower_bound(stk_.slot_storage[level_][idx], stk_.slot_size[level_][idx], stk_.path_[pattern_->partial_[level_]]);
      }
    }

#else

#endif




    // __DEVICE__ bool trans_ctx(TriePatternMatcher& other);
    // __DEVICE__ bool trans_ctx_layer0(TriePatternMatcher& other);
    // __DEVICE__ bool trans_ctx_layer1(TriePatternMatcher& other);
    // __DEVICE__ bool trans_ctx_layerk(TriePatternMatcher& other, int level);

    // __DEVICE__ inline int num_left_task() {
    //   return stk_.candidate_size_[0] - stk_.iter_[0];
    //   static int mul_helper_ = 1000;
    //   int mul = mul_helper_;

    //   for (int i = 0; i < level_; i++) {
    //     int k = stk_.candidate_size_[i] - stk_.iter_[i];
    //     if (k > 0) return k * mul;
    //     mul /= 10;
    //   }
    //   return 0;
    // }

    // __DEVICE__ inline void clear_ctx() {
    //   level_ = 0;

    //   for (int level = 0; level < pattern_->nnodes; level++) {
    //     stk_.iter_[level] = 0;
    //     stk_.path_[level] = 0;
    //     for (int i = 0; i < pattern_->dependency_size_[level]; i++) {
    //       stk_.slot_size(level, i / 2) = 0;
    //     }
    //     stk_.candidate_size_[level] = 0;
    //   }
    // }


  };
}

#ifdef __CUDACC__
#undef __DEVICE__
#endif