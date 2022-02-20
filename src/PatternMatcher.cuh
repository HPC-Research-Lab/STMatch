#pragma once

#include "CallStack.h"
#include "GraphPreprocessor.h"
#include "PatternPreprocessor.h"
#include "Config.h"
#include "JobQueue.cuh"
#include <climits>
#include <omp.h>

namespace libra {

  typedef struct {

    // pointer to graph and pattern stored in shared memory for all warps in the threadblock
    Graph* graph;
    Pattern* pattern;
    // callstack stored in shared memory for this warp 
    CallStack stk;
    JobQueue* job_queue;
    // loop level
    int level;

    size_t count = 0;
  } PatternMatcher;


  __device__ void match(PatternMatcher* pm);

  // TODO: implement intersection and difference
  template<typename DATA_T, typename SIZE_T>
  __device__ void intersection(DATA_T* set1, DATA_T* set2, SIZE_T set1_size, SIZE_T set2_size, DATA_T ub) {

  }

  template<typename DATA_T, typename SIZE_T>
  __device__ void difference(DATA_T* set1, DATA_T* set2, SIZE_T set1_size, SIZE_T set2_size, DATA_T ub) {

  }

}