#pragma once

#include "graph.h"

namespace libra {

  typedef struct {
    graph_node_t* n0;
    graph_node_t* n1;
    graph_node_t length;
    graph_node_t cur;
    int mutex = 0;
  } JobQueue;

  struct JobQueuePreprocessor {

    JobQueue q;


    JobQueuePreprocessor(Graph& g, Pattern& p) {
      std::vector<graph_node_t> vr, vc;
      if (p.partial[0][0] == 1) {
        for (graph_node_t r = 0; r < g.nnodes; r++) {
          for (graph_node_t j = g.rowptr[r]; j < g.rowptr[r + 1]; j++) {
            graph_node_t c = g.colidx[j];
            if (r > c) {
              vr.push_back(r);
              vc.push_back(c);
            }
          }
        }
      }
      else {
        for (graph_node_t r = 0; r < g.nnodes; r++) {
          for (graph_node_t j = g.rowptr[r]; j < g.rowptr[r + 1]; j++) {
            graph_node_t c = g.colidx[j];
            vr.push_back(r);
            vc.push_back(c);
          }
        }
      }

      q.n0 = new graph_node_t[vr.size()];
      q.n1 = new graph_node_t[vr.size()];
      for (graph_node_t i = 0; i < vr.size(); i++) {
        q.n0[i] = vr[i];
        q.n1[i] = vc[i];
      }
      q.length = vr.size();
      q.cur = 0;
    }

    JobQueue* to_gpu() {
      JobQueue qcopy = q;
      cudaMalloc(&qcopy.n0, sizeof(graph_node_t) * q.length);
      cudaMemcpy(qcopy.n0, q.n0, sizeof(graph_node_t) * q.length, cudaMemcpyHostToDevice);
      cudaMalloc(&qcopy.n1, sizeof(graph_node_t) * q.length);
      cudaMemcpy(qcopy.n1, q.n1, sizeof(graph_node_t) * q.length, cudaMemcpyHostToDevice);

      JobQueue* gpu_q;
      cudaMalloc(&gpu_q, sizeof(JobQueue));
      cudaMemcpy(gpu_q, &qcopy, sizeof(JobQueue), cudaMemcpyHostToDevice);
      return gpu_q;
    }
  };
}