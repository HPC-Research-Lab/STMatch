#pragma once

#include "graph.h"

namespace libra {

  typedef struct {
    graph_node_t nodes[PAT_SIZE];
  } Job;

  typedef struct {
    Job* q;
    int start_level;
    graph_node_t length;
    graph_node_t cur;
    int mutex = 0;
  } JobQueue;

  struct JobQueuePreprocessor {

    JobQueue q;


    JobQueuePreprocessor(Graph& g, Pattern& p, bool enum_edge = true) {
      if (enum_edge) {
        std::vector<graph_node_t> vr, vc;
        if (p.partial[0] == 1) {
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

        q.q = new Job[vr.size()];
        for (graph_node_t i = 0; i < vr.size(); i++) {
          (q.q[i].nodes)[0] = vr[i];
          (q.q[i].nodes)[1] = vc[i];
        }
        q.length = vr.size();
        q.cur = 0;
        q.start_level = 2;
      }
      else {
        q.q = new Job[g.nnodes];
        for (graph_node_t i = 0; i<g.nnodes; i++) {
          (q.q[i].nodes)[0] = i;
        }
        q.length = g.nnodes;
        q.cur = 0;
        q.start_level = 1;
      }
    }

    JobQueue* to_gpu() {
      JobQueue qcopy = q;
      cudaMalloc(&qcopy.q, sizeof(Job) * q.length);
      cudaMemcpy(qcopy.q, q.q, sizeof(Job) * q.length, cudaMemcpyHostToDevice);

      JobQueue* gpu_q;
      cudaMalloc(&gpu_q, sizeof(JobQueue));
      cudaMemcpy(gpu_q, &qcopy, sizeof(JobQueue), cudaMemcpyHostToDevice);
      return gpu_q;
    }
  };
}