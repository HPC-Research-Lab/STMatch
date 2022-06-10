#pragma once

#include "graph.h"

namespace STMatch {

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
    double dividPercent[NUM_GPU];
    graph_node_t length[NUM_GPU];
    graph_node_t cur[NUM_GPU];

    void divid_jobs(){
        if(NUM_GPU==1){
          dividPercent[0]=1;
        }
        else if(NUM_GPU==2){
          dividPercent[0] = 0.35;
          dividPercent[1] = 0.65;
        }
        else if(NUM_GPU==3){
          dividPercent[0] = 0.1;
          dividPercent[1] = 0.2;
          dividPercent[2] = 0.7;
        }
        else if(NUM_GPU==4){
          /*
          dividPercent[0] = 0.32;
          dividPercent[1] = 0.04;
          dividPercent[2] = 0.24;
          dividPercent[3] = 0.4;
          */
          dividPercent[0] = 0.14;
          dividPercent[1] = 0.16;
          dividPercent[2] = 0.2;
          dividPercent[3] = 0.5;

        }
        else{
          for(int i=0; i<NUM_GPU; i++){
            dividPercent[i] = ((double)1)/NUM_GPU;
          }
        }

        cur[0] = 0;
        length[0] = q.length*dividPercent[0];
        for(int i=1; i<NUM_GPU; i++){
            graph_node_t widthThisPart = q.length*dividPercent[i];
            cur[i] = length[i-1];
            length[i] = (i==NUM_GPU-1)? q.length: cur[i]+widthThisPart;
        }

    }

    JobQueuePreprocessor(Graph& g, PatternPreprocessor& p) {
      std::vector<graph_node_t> vr, vc;
      for (graph_node_t r = 0; r < g.nnodes; r++) {
        for (graph_node_t j = g.rowptr[r]; j < g.rowptr[r + 1]; j++) {
          graph_node_t c = g.colidx[j];
          if ((!LABELED && p.partial[0][0] == 1 && r > c) || LABELED || p.partial[0][0] != 1) {
            if ((g.vertex_label[r] == (1 << p.vertex_labels[0])) && (g.vertex_label[c] == (1 << p.vertex_labels[1]))) {
              if (g.rowptr[r + 1] - g.rowptr[r] >= p.pat.degree[0] && g.rowptr[c + 1] - g.rowptr[c] >= p.pat.degree[1]) {
                bool valid = false;
                for (graph_node_t d = g.rowptr[c]; d < g.rowptr[c + 1]; d++) {
                  graph_node_t v = g.colidx[d];
                  if (g.rowptr[v + 1] - g.rowptr[v] >= p.pat.degree[2]) {
                    valid = true;
                    break;
                  }
                }

                if (valid) {
                  vr.push_back(r);
                  vc.push_back(c);
                }
              }
            }
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

      divid_jobs();

    }

    JobQueue* to_gpu(int gpuIdx) {
      JobQueue qcopy = q;
      cudaMalloc(&qcopy.q, sizeof(Job) * q.length);
      cudaMemcpy(qcopy.q, q.q, sizeof(Job) * q.length, cudaMemcpyHostToDevice);

      qcopy.cur = cur[gpuIdx];
      qcopy.length = length[gpuIdx];

      JobQueue* gpu_q;
      cudaMalloc(&gpu_q, sizeof(JobQueue));
      cudaMemcpy(gpu_q, &qcopy, sizeof(JobQueue), cudaMemcpyHostToDevice);
      return gpu_q;
    }
    
    /*
    JobQueue* distributedQueue(){

      printf("Total Length:%d\n", q.length);
      JobQueue qcopy[NUM_GPU];
      for(int i=0; i<NUM_GPU; i++){
        qcopy[i] = q;
        if(i==0){
          cudaMallocManaged(&qcopy[i].q, sizeof(Job) * q.length);
          cudaMemcpy(qcopy[i].q, q.q, sizeof(Job) * q.length, cudaMemcpyHostToDevice);
        }
        else{
          qcopy[i].q = qcopy[i-1].q;
        }
      }

      //double cut_percent[4] = {0.1, 0.2, 0.3, 0.4};
      //double cut_percent[NUM_GPU] = {1};

      double cut_percent[NUM_GPU];
      if(NUM_GPU==1){
        cut_percent[0]=1;
      }
      else{
        cut_percent[0] = 0.4;
        cut_percent[1] = 0.6;
      }

      int end_pos[2];
      for(int i=0; i<NUM_GPU; i++){
        if(i==NUM_GPU-1){
          if(i==0) qcopy[i].length  = q.length;
          else qcopy[i].length = q.length-end_pos[i-1];
        }
        else{
          qcopy[i].length = (graph_node_t)(q.length * cut_percent[i]);
        }
        
        if(i==0) {
          qcopy[i].cur = 0;
          end_pos[i] = qcopy[i].length;
        }
        else {
          qcopy[i].cur = end_pos[i-1];
          end_pos[i] = end_pos[i-1]+qcopy[i].length;
          qcopy[i].length = end_pos[i];
        }
      }

      JobQueue* gpu_q[NUM_GPU];
      for(int i=0; i<NUM_GPU; i++){
        cudaMalloc(&gpu_q[i], sizeof(JobQueue)*NUM_GPU);
        cudaMemcpy(gpu_q[i], &qcopy[i], sizeof(JobQueue)*NUM_GPU, cudaMemcpyHostToDevice);
      }
      return gpu_q;
    }
    */

  };
}