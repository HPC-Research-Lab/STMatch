#include "gpu_match.cuh"
#include <cuda.h>

namespace libra {

  // TODO: implement intersection and difference

  template<typename DATA_T, typename SIZE_T>
  __device__
    inline bool lower_bound_exist(DATA_T* set2, SIZE_T set2_size, DATA_T target) {
    int mid;
    int low = 0;
    int high = set2_size - 1;
    while (low <= high) {
      mid = (low + high) / 2;
      if (target == set2[mid]) {
        return true;
      }
      else if (target > set2[mid]) {
        low = mid + 1;
      }
      else {
        high = mid - 1;
      }
    }
    return false;
  }


  __device__ inline void prefix_sum(int* _input) {

    int thid = threadIdx.x % WARP_SIZE;
    int offset = 1;
    int last_element = _input[WARP_SIZE - 1];
    // build sum in place up the tree
    for (int d = WARP_SIZE >> 1; d > 0; d >>= 1) {
      //__syncthreads();
      if (thid < d) {
        int ai = offset * (2 * thid + 1) - 1;
        int bi = offset * (2 * thid + 2) - 1;
        _input[bi] += _input[ai];
      }
      offset <<= 1;
    }
    if (thid == 0) { _input[WARP_SIZE - 1] = 0; } // clear the last element
     // traverse down tree & build scan
    for (int d = 1; d < WARP_SIZE; d <<= 1) {
      offset >>= 1;
      //__syncthreads();
      if (thid < d) {
        int ai = offset * (2 * thid + 1) - 1;
        int bi = offset * (2 * thid + 2) - 1;
        int t = _input[ai];
        _input[ai] = _input[bi];
        _input[bi] += t;
      }
    }

    if (thid == WARP_SIZE - 1)
      _input[WARP_SIZE] = _input[WARP_SIZE - 1] + last_element;
  }


  template<typename DATA_T, typename SIZE_T>
  __device__ void intersection(DATA_T* set1, DATA_T* set2, DATA_T* _res, SIZE_T set1_size, SIZE_T set2_size, SIZE_T* res_size, DATA_T ub) {

    __shared__ int pos[NWARPS_PER_BLOCK][33];
    __shared__ bool still_loop[NWARPS_PER_BLOCK];

    int wid = threadIdx.x / WARP_SIZE;
    int tid = threadIdx.x % WARP_SIZE;

    int end_pos = 0;

    still_loop[wid] = true;

    for (int idx = tid; (idx < (((set1_size - 1) / WARP_SIZE + 1) * WARP_SIZE) && still_loop[wid]); idx += WARP_SIZE) {
      pos[wid][tid] = 0;
      pos[wid][WARP_SIZE] = 0;
      if (idx < set1_size && set1[idx] < ub) {
        if (lower_bound_exist(set2, set2_size, set1[idx])) {
          pos[wid][tid] = 1;
        }
      }
      else {
        still_loop[wid] = false;
      }
      __syncwarp();

      prefix_sum(&pos[wid][0]);

      if (pos[wid][tid + 1] > pos[wid][tid]) {
        _res[end_pos + pos[wid][tid]] = set1[idx];
      }
      end_pos += pos[wid][WARP_SIZE];
    }

    *res_size = end_pos;
  }

  template<typename DATA_T, typename SIZE_T>
  __device__ void difference(DATA_T* set1, DATA_T* set2, DATA_T* _res, SIZE_T set1_size, SIZE_T set2_size, SIZE_T* res_size, DATA_T ub) {
    __shared__ int pos[NWARPS_PER_BLOCK][33];
    __shared__ bool still_loop[NWARPS_PER_BLOCK];

    int wid = threadIdx.x / WARP_SIZE;
    int tid = threadIdx.x % WARP_SIZE;

    int end_pos = 0;

    still_loop[wid] = true;

    for (int idx = tid; (idx < (((set1_size - 1) / WARP_SIZE + 1) * WARP_SIZE) && still_loop[wid]); idx += WARP_SIZE) {
      pos[wid][tid] = 0;
      pos[wid][WARP_SIZE] = 0;
      if (idx < set1_size && set1[idx] < ub) {
        if (set2 != NULL) {
          if (!lower_bound_exist(set2, set2_size, set1[idx])) {
            pos[wid][tid] = 1;
          }
        }
        else {
          pos[wid][tid] = 1;
        }
      }
      else {
        still_loop[wid] = false;
      }
      __syncwarp();

      prefix_sum(&pos[wid][0]);

      // TODO: if _res and set1 are the same
      if (pos[wid][tid + 1] > pos[wid][tid]) {
        _res[end_pos + pos[wid][tid]] = set1[idx];
      }
      end_pos += pos[wid][WARP_SIZE];
    }
    *res_size = end_pos;
  }

  __device__ void lock(int* mutex) {
    while (atomicCAS(mutex, 0, 1) != 0);
  }

  __device__ void unlock(int* mutex) {
    atomicExch(mutex, 0);
  }

  __device__ void get_job(JobQueue* q, graph_node_t& cur_pos, graph_node_t& njobs) {
    lock(&(q->mutex));
    cur_pos = q->cur;
    q->cur += JOB_CHUNK_SIZE;
    if (q->cur > q->length) q->cur = q->length;
    njobs = q->cur - cur_pos;
    unlock(&(q->mutex));
  }

  __device__ void extend(Graph* g, Pattern* pat, CallStack* stk, JobQueue* q, graph_node_t level) {
    if (level == 0) {
      graph_node_t cur_job, njobs;

      // TODO: change to warp
      if (threadIdx.x % WARP_SIZE == 0) {
        get_job(q, cur_job, njobs);
        // printf("%d %d: %d %d\n", blockIdx.x, threadIdx.x, cur_job, njobs);
        for (size_t i = 0; i < njobs; i++) {
          stk->slot_storage[0][0][i] = q->queue[cur_job + i];
        }
        stk->slot_size[0][0] = njobs;
      }
      __syncwarp();
    }
    else {

      for (pattern_node_t i = 0; i < PAT_SIZE; i++) {

        // compute ub based on pattern->partial
        graph_node_t ub = INT_MAX;
        if (pat->partial[level - 1][i] >= 0) ub = stk->path[pat->partial[level - 1][i]];
        graph_node_t* neighbor = &g->colidx[g->rowptr[stk->path[level - 1]]];
        graph_node_t neighbor_size = (graph_node_t)(g->rowptr[stk->path[level - 1] + 1] - g->rowptr[stk->path[level - 1]]);

        if (pat->set_ops[level - 1][i] & 0x10) {

          graph_node_t* nb = NULL;
          graph_node_t nsize = 0;

          if (level >= 2) {
            nb = &g->colidx[g->rowptr[stk->path[level - 2]]];
            nsize = (graph_node_t)(g->rowptr[stk->path[level - 2] + 1] - g->rowptr[stk->path[level - 2]]);
          }
          // when the second set is empty, the difference function simply checks ub and copy first set to res set.
          difference(neighbor, nb, &(stk->slot_storage[level][i][0]), neighbor_size, nsize, &(stk->slot_size[level][i]), ub);

          for (pattern_node_t j = level - 3; j >= 0; j--) {
            nb = &g->colidx[g->rowptr[stk->path[j]]];
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
    }
    stk->iter[level] = 0;
  }

  __device__ void match(Graph* g, Pattern* pat, CallStack* stk, JobQueue* q, size_t* count) {
    graph_node_t level = 0;

    while (true) {

      if (level < pat->nnodes - 1) {
        if (stk->slot_size[level][0] == 0) {

          extend(g, pat, stk, q, level);
          if (level == 0 && stk->slot_size[level][0] == 0) break;
        }
        if (stk->iter[level] < stk->slot_size[level][0]) {
          stk->path[level] = stk->slot_storage[level][0][stk->iter[level]];
          level++;
        }
        else {
          stk->slot_size[level][0] = 0;
          if (level > 0) {
            level--;
            if (threadIdx.x % WARP_SIZE == 0) stk->iter[level]++;
            __syncwarp();
          }
        }
      }
      else if (level == pat->nnodes - 1) {

        // TODO: we can save the storage of sets for the last level
        extend(g, pat, stk, q, level);
        if (threadIdx.x % WARP_SIZE == 0) {
          *count += stk->slot_size[level][0];
        }
        __syncwarp();
        stk->slot_size[level][0] = 0;
        level--;
        if (threadIdx.x % WARP_SIZE == 0) stk->iter[level]++;
        __syncwarp();
      }
    }
  }

  __global__ void _parallel_match(Graph* dev_graph, Pattern* dev_pattern, CallStack* dev_callstack, JobQueue* job_queue, size_t* res) {

    __shared__ Graph graph;
    __shared__ Pattern pat;
    __shared__ CallStack stk[NWARPS_PER_BLOCK];
    __shared__ size_t count[NWARPS_PER_BLOCK];

    int global_tid = blockIdx.x * blockDim.x + threadIdx.x;
    int global_wid = global_tid / WARP_SIZE;
    int local_wid = threadIdx.x / WARP_SIZE;

    if (threadIdx.x == 0) {
      graph = *dev_graph;
      pat = *dev_pattern;
    }
    __syncthreads();

    if (threadIdx.x % WARP_SIZE == 0) {
      stk[local_wid] = dev_callstack[global_wid];
    }
    __syncwarp();

    auto start = clock64();
    while (true) {
      match(&graph, &pat, &stk[local_wid], job_queue, &count[local_wid]);
      break;
      // TODO: load balance
    }
    auto stop = clock64();

    if (threadIdx.x % WARP_SIZE == 0) {
      res[global_wid] = count[local_wid];
      // printf("%ld\n", stop - start);
    }
  }
}