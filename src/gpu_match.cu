#include "gpu_match.cuh"
#include <cuda.h>

namespace libra {

  // TODO: implement intersection and difference

  template<typename DATA_T, typename SIZE_T>
  __device__
    inline bool lower_bound_exist(DATA_T* set2, SIZE_T set2_size, DATA_T target) {
    int mid;
    int low = 0;
    int high = set2_size;
    while (low < high) {
      mid = low + (high - low) / 2;
      if (target <= set2[mid]) {
        high = mid;
      }
      else {
        low = mid + 1;
      }
    }
    if (low < set2_size && set2[low] < target) {
      low++;
    }
    return (low < set2_size&& set2[low] == target);
  }


  __device__ void prefix_sum(int* _input) {

    int thid = threadIdx.x;
    int offset = 1;
    for (int d = warpSize >> 1; d > 0; d >>= 1) // build sum in place up the tree
    {
      //__syncthreads();
      if (thid < d)
      {
        int ai = offset * (2 * thid + 1) - 1;
        int bi = offset * (2 * thid + 2) - 1;
        _input[bi] += _input[ai];
      }
      offset *= 2;
    }
    if (thid == 0) { _input[warpSize - 1] = 0; } // clear the last element
    for (int d = 1; d < warpSize; d *= 2) // traverse down tree & build scan
    {
      offset >>= 1;
      //__syncthreads();
      if (thid < d)
      {
        int ai = offset * (2 * thid + 1) - 1;
        int bi = offset * (2 * thid + 2) - 1;
        int t = _input[ai];
        _input[ai] = _input[bi];
        _input[bi] += t;
      }
    }
    //__syncthreads();
  }


  template<typename DATA_T, typename SIZE_T>
  __device__ void intersection(DATA_T* set1, DATA_T* set2, DATA_T* _res, SIZE_T set1_size, SIZE_T set2_size, SIZE_T* res_size, DATA_T ub) {

    __shared__ int pos[BLOCK_DIM];

    int tid = threadIdx.x;
    int end_pos = 0;
    int loop_end = set1_size - set1_size % warpSize + warpSize;
    bool last_find;
    for (int idx = tid; idx < loop_end && set1[idx] < ub; idx += warpSize) {
      last_find = false;
      pos[tid] = 0;
      if (idx < set1_size && set1[idx] < ub) {
        if (lower_bound_exist(set2, set2_size, set1[idx])) {
          pos[tid] = 1;
        }
        if (threadIdx.x == warpSize - 1 && pos[warpSize - 1] == 1) {
          last_find = true;
        }
      }
      prefix_sum(pos);

      if ((tid == warpSize - 1 && last_find) ||
        tid != warpSize - 1 && pos[tid] < pos[tid + 1]) {
        _res[end_pos + pos[tid]] = set1[idx];
      }
      end_pos += pos[warpSize - 1];
    }
    // __syncthreads();
     /*
     if(tid==0){
       for(int i =0; i<end_pos; i++){
         printf("%d ", _res[i]);
       }
       printf("\n");
     }
 */
    *res_size = end_pos;
  }

  template<typename DATA_T, typename SIZE_T>
  __device__ void difference(DATA_T* set1, DATA_T* set2, DATA_T* res, SIZE_T set1_size, SIZE_T set2_size, SIZE_T* res_size, DATA_T ub) {

    __shared__ int pos[BLOCK_DIM];

    int tid = threadIdx.x;
    int end_pos = 0;
    int loop_end = set1_size - set1_size % warpSize + warpSize;
    bool last_find;
    for (int idx = tid; idx < loop_end; idx += warpSize) {
      last_find = false;
      pos[tid] = 0;

      if (idx < set1_size && set1[idx] < ub) {
        if (!lower_bound_exist(set2, set2_size, set1[idx])) {
          pos[tid] = 1;
        }
        if (threadIdx.x == warpSize - 1 && pos[warpSize - 1] == 1) {
          last_find = true;
        }
      }
      prefix_sum(pos);

      if ((tid == warpSize - 1 && last_find) ||
        tid != warpSize - 1 && pos[tid] < pos[tid + 1]) {
        res[end_pos + pos[tid]] = set1[idx];
      }
      end_pos += pos[warpSize - 1];
    }
    // __syncthreads();
     /*
     if(tid==0){
       for(int i =0; i<end_pos; i++){
         printf("%d ", _res[i]);
       }
       printf("\n");
     }
 */
 //clock_t stop_time = clock();
 //if(threadIdx.x==0){
     //printf("time pointer2:%p\n", time_);
     //time_[blockIdx.x] = stop_time - start_time;
 //}
    * res_size = end_pos;

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

              // TODO: compute ub based on pattern->partial
              graph_node_t ub = INT_MAX;
              if (pat->partial[level - 1][i] >= 0) ub = stk->path[pat->partial[level - 1][i]];

              graph_node_t* neighbor = g->rowptr[stk->path[level - 1]] + g->colidx;
              graph_node_t neighbor_size = (graph_node_t)(g->rowptr[stk->path[level - 1] + 1] - g->rowptr[stk->path[level - 1]]);

              if (pat->set_ops[level - 1][i] & 0x10) {

                graph_node_t* nb = NULL;
                graph_node_t nsize = 0;

                if (level >= 2) {
                  nb = g->rowptr[stk->path[level - 2]] + g->colidx;
                  nsize = (graph_node_t)(g->rowptr[stk->path[level - 2] + 1] - g->rowptr[stk->path[level - 2]]);
                }
                // when the second set is empty, the difference function simply checks ub and copy first set to res set.
                difference(neighbor, nb, &(stk->slot_storage[level][i][0]), neighbor_size, nsize, &(stk->slot_size[level][i]), ub);

                for (pattern_node_t j = level - 3; j >= 0; j--) {
                  nb = g->rowptr[stk->path[j]] + g->colidx;
                  nsize = (graph_node_t)(g->rowptr[stk->path[j] + 1] - g->rowptr[stk->path[j]]);

                  difference(&(stk->slot_storage[level][i][0]), nb, &(stk->slot_storage[level][i][0]), stk->slot_size[level][i], nsize, &(stk->slot_size[level][i]), ub);
                }
              }
              else {

                pattern_node_t slot_idx = (pat->set_ops[level - 1][i] & 0xF);

                if (pat->set_ops[level - 1][i] & 0x20) {
                  intersection(&(stk->slot_storage[level - 1][slot_idx][0]), neighbor, &(stk->slot_storage[level][i][i]), stk->slot_size[level - 1][slot_idx], neighbor_size, &(stk->slot_size[level][i]), ub);
                }
                else {
                  difference(&(stk->slot_storage[level - 1][slot_idx][0]), neighbor, &(stk->slot_storage[level][i][i]), stk->slot_size[level - 1][slot_idx], neighbor_size, &(stk->slot_size[level][i]), ub);
                }
              }

              if (pat->set_ops[level - 1][i] < 0) break;
            }
          } //end extend

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