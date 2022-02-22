#pragma once

#include "GraphPreprocessor.h"

namespace libra {

  typedef struct {
    graph_node_t* queue;
    graph_node_t length;
    graph_node_t cur;
    int mutex = 0;
  } JobQueue;

  struct JobQueuePreprocessor {

    JobQueue q;


    JobQueuePreprocessor(Graph& g) {
      q.queue = new graph_node_t[g.nnodes];
      for (graph_node_t i = 0; i < g.nnodes; i++) q.queue[i] = i;
      q.length = g.nnodes;
      q.cur = 0;
    }

    JobQueue* to_gpu() {
      JobQueue qcopy = q;
      cudaMalloc(&qcopy.queue, sizeof(graph_node_t) * q.length);
      cudaMemcpy(qcopy.queue, q.queue, sizeof(graph_node_t) * q.length, cudaMemcpyHostToDevice);

      JobQueue* gpu_q;
      cudaMalloc(&gpu_q, sizeof(JobQueue));
      cudaMemcpy(gpu_q, &qcopy, sizeof(JobQueue), cudaMemcpyHostToDevice);
      return gpu_q;
    }
  };
}