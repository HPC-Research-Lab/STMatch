#include "Match.cuh"
#include <cuda.h>

namespace libra {

  // TODO: implement intersection and difference
  template<typename DATA_T, typename SIZE_T>
  __device__ void intersection(DATA_T* set1, DATA_T* set2, SIZE_T set1_size, SIZE_T set2_size, DATA_T ub) {

  }

  template<typename DATA_T, typename SIZE_T>
  __device__ void difference(DATA_T* set1, DATA_T* set2, SIZE_T set1_size, SIZE_T set2_size, DATA_T ub) {

  }

  __device__ void lock(int* mutex) {
    while (atomicCAS(mutex, 0, 1) != 0);
  }

  __device__ void unlock(int* mutex) {
    atomicExch(mutex, 0);
  }

  __device__ void get_job(JobQueue* q, graph_node_t& cur_job, graph_node_t& njobs) {
    lock(&(q->mutex));
    graph_node_t cur_pos = q->cur;
    q->cur += JOB_CHUNK_SIZE;
    if (q->cur > q->length) q->cur = q->length;
    njobs = q->cur - cur_pos;
    unlock(&(q->mutex));
  }

  __device__ void match(Graph* g, Pattern* pat, CallStack* stk, JobQueue* q, size_t* count) {
    graph_node_t level = 0;

    while (true) {

      if (level < pat->nnodes - 1) {
        if (stk->slot_size[level][0] == 0) {

          // extend 
          if (level == 0) {
            graph_node_t cur_job, njobs;

            // TODO: change to warp
            if (threadIdx.x % WARP_SIZE == 0) {
              get_job(q, cur_job, njobs);
              stk->iter[0] = 0;
              for (size_t i = 0; i < njobs; i++) {
                stk->slot_storage[0][0][i] = q->queue[cur_job + i];
              }
              stk->slot_size[0][0] = njobs;
            }
            __syncwarp();
          }
          else {

            for (pattern_node_t i = 0; i < PAT_SIZE; i++) {

              if (pat->set_ops[level][i] < 0) break;

              pattern_node_t slot_idx = (pat->set_ops[level][i] & 0xF);

              // TODO: compute ub based on pattern->partial
              graph_node_t ub = INT_MAX;

              graph_node_t* neighbor = g->rowptr[stk->path[level]] + g->colidx;
              graph_node_t neighbor_size = (graph_node_t)(g->rowptr[stk->path[level] + 1] - g->rowptr[stk->path[level]]);

              if (pat->set_ops[level][i] & 0x10) {
                // TODO: change to warp 
                for (graph_node_t j = 0; j < neighbor_size; j++) {
                  stk->slot_storage[level][i][j] = neighbor[j];
                }
                stk->slot_size[level][i] = neighbor_size;

                for (pattern_node_t j = level - 1; j >= 0; j--) {
                  graph_node_t* nb = g->rowptr[stk->path[j]] + g->colidx;
                  graph_node_t nsize = (graph_node_t)(g->rowptr[stk->path[j] + 1] - g->rowptr[stk->path[j]]);

                  difference(&(stk->slot_storage[level][i][0]), nb, stk->slot_size[level][i], nsize, ub);
                }

              }
              else {
                // TODO: change to warp
                for (graph_node_t j = 0; j < stk->slot_size[level - 1][slot_idx]; j++) {
                  stk->slot_storage[level][i][j] = stk->slot_storage[level - 1][slot_idx][j];
                }
                stk->slot_size[level][i] = stk->slot_size[level - 1][slot_idx];

                if (pat->set_ops[level][i] & 0x20) {
                  difference(&(stk->slot_storage[level][i][0]), neighbor, stk->slot_size[level][i], neighbor_size, ub);
                }
                else {
                  intersection(&(stk->slot_storage[level][i][0]), neighbor, stk->slot_size[level][i], neighbor_size, ub);
                }
              }
            }
          }

          stk->iter[level] = 0;
        }
        if (stk->iter[level] < stk->slot_size[level][0]) {
          stk->path[level] = stk->slot_storage[level][0][stk->iter[level]];
          level++;
        }
        else {
          stk->slot_size[level][0] = 0;
          level--;
          if (level < 0) {
            break;
          }
          if (threadIdx.x % WARP_SIZE == 0) stk->iter[level]++;
          __syncwarp();
        }
      }
      else if (level == pat->nnodes - 1) {
        // TODO: we can save the storage of sets for the last level
        if (threadIdx.x % WARP_SIZE == 0) {
          count += stk->slot_size[level][0];
        }
        __syncwarp();
        stk->slot_size[level][0] = 0;
        level--;
      }
    }
  }


  __global__ void _parallel_match(Graph* dev_graph, Pattern* dev_pattern, CallStack* dev_callstack, JobQueue* job_queue) {

    __shared__ Graph graph;
    __shared__ Pattern pat;
    __shared__ JobQueue queue;
    __shared__ CallStack stk[NWARPS_PER_BLOCK];
    __shared__ size_t count[NWARPS_PER_BLOCK];

    int global_tid = blockIdx.x * blockDim.x + threadIdx.x;
    int global_wid = global_tid / WARP_SIZE;
    int local_wid = threadIdx.x / WARP_SIZE;

    // TODO: change to warp 
    // graph and pattern are shared among all threads in a threadblock
    if (threadIdx.x == 0) {
      graph = *dev_graph;
      pat = *dev_pattern;
      queue = *job_queue;
    }
    __syncwarp();

    // TODO: change to warp
    // callstack is shared among all threads in each warp
    if (threadIdx.x % WARP_SIZE == 0) {
      stk[local_wid] = dev_callstack[global_wid];
    }
    __syncwarp();

    while (true) {
      match(&graph, &pat, &stk[local_wid], &queue, &count[local_wid]);
      break;
      // TODO: load balance
    }
  }
}