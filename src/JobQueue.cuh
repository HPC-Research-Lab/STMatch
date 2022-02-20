#pragma once

#include "GraphPreprocessor.h"

namespace libra {

  struct JobQueue {
    graph_node_t* queue;
    graph_node_t length;
    graph_node_t cur;
    int mutex = 0;

    static const graph_node_t JOB_CHUNK_SIZE = 256;

    JobQueue(Graph& g) {
      queue = new graph_node_t[g.nnodes];
      for (graph_node_t i = 0; i < g.nnodes; i++) queue[i] = i;
      length = g.nnodes;
      cur = 0;
    }

    JobQueue() {}


    JobQueue* to_gpu() {
      JobQueue q;
      q.length = length;
      q.cur = cur;
      cudaMalloc(&q.queue, sizeof(graph_node_t) * length);
      cudaMemcpy(q.queue, queue, sizeof(graph_node_t) * length, cudaMemcpyHostToDevice);

      JobQueue* gpu_q;
      cudaMalloc(&gpu_q, sizeof(JobQueue));
      cudaMemcpy(gpu_q, &q, sizeof(JobQueue), cudaMemcpyHostToDevice);
      return gpu_q;
    }

    __device__ void get_job(graph_node_t& cur_job, graph_node_t& njobs) {
      lock(&mutex);
      graph_node_t cur_pos = cur;
      cur += JOB_CHUNK_SIZE;
      if (cur > length) cur = length;
      njobs = cur - cur_pos;
      unlock(&mutex);
    }

    __device__ void lock(int* mutex) {
      while (atomicCAS(mutex, 0, 1) != 0);
    }

    __device__ void unlock(int* mutex) {
      atomicExch(mutex, 0);
    }

  };



}