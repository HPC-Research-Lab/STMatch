#pragma once

#include "GraphPreprocessor.h"

namespace libra {
  struct JobQueue {
    size_t* queue_;
    size_t length_;
    size_t cur_;
    int mutex = 0;

    JobQueue(Graph& g) {
      queue_ = new size_t[g.nnodes];
      for (size_t i = 0; i < g.nnodes; i++) queue_[i] = i;
      length_ = g.nnodes;
      cur_ = 0;
    }

    JobQueue() {}


    JobQueue* to_gpu() {
      JobQueue q;
      q.length_ = length_;
      q.cur_ = cur_;
      cudaMalloc(&q.queue_, sizeof(size_t) * length_);
      cudaMemcpy(q.queue_, queue_, sizeof(size_t) * length_, cudaMemcpyHostToDevice);

      JobQueue* gpu_q;
      cudaMalloc(&gpu_q, sizeof(JobQueue));
      cudaMemcpy(gpu_q, &q, sizeof(JobQueue), cudaMemcpyHostToDevice);
      return gpu_q;
    }

#ifdef __CUDACC__
    __device__ void get_job(size_t& cur_job, size_t& njobs) {
      lock(&mutex);
      size_t cur_pos = cur_;
      cur_ += 3264;
      if (cur_ > length_) cur_ = length_;
      njobs = cur_ - cur_pos;
      unlock(&mutex);
    }
    __device__ void lock(int* mutex) {
      while (atomicCAS(mutex, 0, 1) != 0);
    }
    __device__ void unlock(int* mutex) {
      atomicExch(mutex, 0);
    }
#else
#endif

  };



}