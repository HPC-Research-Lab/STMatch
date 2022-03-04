#include "gpu_match.cuh"
#include <cuda.h>

namespace libra {


  inline __device__ graph_node_t path(CallStack* stk, int level) {
    if (level >= stk->start_level) {
      return stk->slot_storage[level][0][stk->iter[level]];
    }
    else {
      return stk->slot_storage[0][0][stk->iter[0] + JOB_CHUNK_SIZE * level];
    }
  }

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
    for (int d = (WARP_SIZE >> 1); d > 0; d >>= 1) {
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
    __syncwarp();

    if (thid == WARP_SIZE - 1)
      _input[WARP_SIZE] = _input[WARP_SIZE - 1] + last_element;
  }

  typedef struct {
    graph_node_t* set1, * set2, * res;
    graph_node_t set1_size, set2_size, * res_size;
    graph_node_t ub;
    bitarray32 label;
    Graph* g;
  } Arg_t;


  __device__ void intersection(Arg_t* arg) {

    __shared__ int pos[NWARPS_PER_BLOCK][33];
    __shared__ bool still_loop[NWARPS_PER_BLOCK];

    int wid = threadIdx.x / WARP_SIZE;
    int tid = threadIdx.x % WARP_SIZE;

    int end_pos = 0;

    if (arg->set1_size > 0) {

      still_loop[wid] = true;

      for (int idx = tid; (idx < (((arg->set1_size - 1) / WARP_SIZE + 1) * WARP_SIZE) && still_loop[wid]); idx += WARP_SIZE) {
        pos[wid][tid] = 0;
        pos[wid][WARP_SIZE] = 0;
        if (idx < arg->set1_size && arg->set1[idx] < arg->ub) {
          bitarray32 lb = arg->g->vertex_label[arg->set1[idx]];
          if ((lb && arg->label == lb) && lower_bound_exist(arg->set2, arg->set2_size, arg->set1[idx])) {
            pos[wid][tid] = 1;
          }
        }
        else {
          still_loop[wid] = false;
        }
        __syncwarp();

        prefix_sum(&pos[wid][0]);

        graph_node_t res_tmp;
        if (pos[wid][tid + 1] > pos[wid][tid]) {
          res_tmp = arg->set1[idx];
        }
        __syncwarp();
        if (pos[wid][tid + 1] > pos[wid][tid]) {
          arg->res[end_pos + pos[wid][tid]] = res_tmp;
        }
        end_pos += pos[wid][WARP_SIZE];
      }
    }
    *(arg->res_size) = end_pos;
  }

  __device__ void difference(Arg_t* arg) {

    __shared__ int pos[NWARPS_PER_BLOCK][33];
    __shared__ bool still_loop[NWARPS_PER_BLOCK];

    int wid = threadIdx.x / WARP_SIZE;
    int tid = threadIdx.x % WARP_SIZE;

    int end_pos = 0;

    still_loop[wid] = true;

    for (int idx = tid; (idx < (((arg->set1_size - 1) / WARP_SIZE + 1) * WARP_SIZE) && still_loop[wid]); idx += WARP_SIZE) {
      pos[wid][tid] = 0;
      pos[wid][WARP_SIZE] = 0;
      if (idx < arg->set1_size && arg->set1[idx] < arg->ub) {
        bitarray32 lb = arg->g->vertex_label[arg->set1[idx]];
        if (arg->set2 != NULL) {
          if ((lb && arg->label == lb) && !lower_bound_exist(arg->set2, arg->set2_size, arg->set1[idx])) {
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

      graph_node_t res_tmp;
      if (pos[wid][tid + 1] > pos[wid][tid]) {
        res_tmp = arg->set1[idx];
      }
      __syncwarp();
      if (pos[wid][tid + 1] > pos[wid][tid]) {
        arg->res[end_pos + pos[wid][tid]] = res_tmp;
      }
      end_pos += pos[wid][WARP_SIZE];
    }
    *(arg->res_size) = end_pos;
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

  __device__ void extend(Graph* g, Pattern* pat, CallStack* stk, JobQueue* q, pattern_node_t level) {

    __shared__ Arg_t arg[NWARPS_PER_BLOCK];
    int wid = threadIdx.x / WARP_SIZE;

    if (level == 0) {
      graph_node_t cur_job, njobs;

      // TODO: change to warp
      if (threadIdx.x % WARP_SIZE == 0) {
        get_job(q, cur_job, njobs);

        for (size_t i = 0; i < njobs; i++) {
          for (int j = 0; j < q->start_level; j++) {
            stk->slot_storage[0][0][i + JOB_CHUNK_SIZE * j] = (q->q[cur_job + i].nodes)[j];
          }
        }
        stk->slot_size[0][0] = njobs;
        stk->start_level = q->start_level;
      }
      __syncwarp();
    }
    else {

      for (pattern_node_t i = 0; i < PAT_SIZE; i++) {

        if (pat->set_ops[level - 1][i] < 0) break;

        // compute ub based on pattern->partial
        graph_node_t ub = INT_MAX;
        // assert(level >= 1);
        if (i == 0) {
          ub = INT_MAX;
          if (pat->partial[level - 1][i] != 0) {
            for (pattern_node_t k = 0; k <= level - 1; k++) {
              if ((pat->partial[level - 1][0] & (1 << k)) && (ub > path(stk, k))) ub = path(stk, k);
            }
          }
        }
        else {
          ub = -1;
          if (pat->partial[level - 1][i] != 0) {
            for (pattern_node_t k = 0; k <= level - 1; k++) {
              if ((pat->partial[level - 1][i] & (1 << k)) && (ub < path(stk, k))) ub = path(stk, k);
            }
          }
          if (ub == -1) ub = INT_MAX;
        }

        bitarray32 lb = pat->vertex_label[level - 1][i];
        //printf("lb:%d\n", lb);

        graph_node_t* neighbor = &g->colidx[g->rowptr[path(stk, level - 1)]];
        graph_node_t neighbor_size = (graph_node_t)(g->rowptr[path(stk, level - 1) + 1] - g->rowptr[path(stk, level - 1)]);

        if (pat->set_ops[level - 1][i] & 0x10) {

          graph_node_t* nb = NULL;
          graph_node_t nsize = 0;

          if (level >= 2) {
            nb = &g->colidx[g->rowptr[path(stk, level - 2)]];
            nsize = (graph_node_t)(g->rowptr[path(stk, level - 2) + 1] - g->rowptr[path(stk, level - 2)]);
          }
          // when the second set is empty, the difference function simply checks ub and copy first set to res set.
          arg[wid].set1 = neighbor;
          arg[wid].set2 = nb;
          arg[wid].res = &(stk->slot_storage[level][i][0]);
          arg[wid].set1_size = neighbor_size;
          arg[wid].set2_size = nsize;
          arg[wid].res_size = &(stk->slot_size[level][i]);
          arg[wid].ub = ub;
          arg[wid].label = lb;
          arg[wid].g = g;
          difference(&arg[wid]);


          for (pattern_node_t j = level - 3; j >= 0; j--) {
            nb = &g->colidx[g->rowptr[path(stk, j)]];
            nsize = (graph_node_t)(g->rowptr[path(stk, j) + 1] - g->rowptr[path(stk, j)]);

            arg[wid].set1 = &(stk->slot_storage[level][i][0]);
            arg[wid].set2 = nb;
            arg[wid].res = &(stk->slot_storage[level][i][0]);
            arg[wid].set1_size = stk->slot_size[level][i];
            arg[wid].set2_size = nsize;
            arg[wid].res_size = &(stk->slot_size[level][i]);
            arg[wid].ub = ub;
            arg[wid].label = lb;
            arg[wid].g = g;
            difference(&arg[wid]);
          }
        }
        else {

          pattern_node_t slot_idx = (pat->set_ops[level - 1][i] & 0xF);

          if (pat->set_ops[level - 1][i] & 0x20) {

            arg[wid].set1 = &(stk->slot_storage[level - 1][slot_idx][0]);
            arg[wid].set2 = neighbor;
            arg[wid].res = &(stk->slot_storage[level][i][0]);
            arg[wid].set1_size = stk->slot_size[level - 1][slot_idx];
            arg[wid].set2_size = neighbor_size;
            arg[wid].res_size = &(stk->slot_size[level][i]);
            arg[wid].ub = ub;
            arg[wid].label = lb;
            arg[wid].g = g;
            intersection(&arg[wid]);
          }
          else {
            arg[wid].set1 = &(stk->slot_storage[level - 1][slot_idx][0]);
            arg[wid].set2 = neighbor;
            arg[wid].res = &(stk->slot_storage[level][i][0]);
            arg[wid].set1_size = stk->slot_size[level - 1][slot_idx];
            arg[wid].set2_size = neighbor_size;
            arg[wid].res_size = &(stk->slot_size[level][i]);
            arg[wid].ub = ub;
            arg[wid].label = lb;
            arg[wid].g = g;
            difference(&arg[wid]);
          }
        }
      }
    }
    stk->iter[level] = 0;
  }

  __device__ void match(Graph* g, Pattern* pat, CallStack* stk, JobQueue* q, size_t* count) {
    pattern_node_t level = 0;

    while (true) {

      if (level < pat->nnodes - 1) {

        if (stk->slot_size[level][0] == 0) {
          extend(g, pat, stk, q, level);
          if (level == 0 && stk->slot_size[level][0] == 0) break;
        }

        if (stk->iter[level] < stk->slot_size[level][0]) {

          level++;
        }
        else {
          stk->slot_size[level][0] = 0;
          if (level == stk->start_level) {
            level--;
            if (threadIdx.x % WARP_SIZE == 0) {
              stk->iter[0]++;
              for (int j = 1; j < stk->start_level; j++) {
                stk->iter[j] = stk->slot_size[j][0];
              }
            }
            __syncwarp();
          }
          else if (level > stk->start_level) {
            level--;
            if (threadIdx.x % WARP_SIZE == 0) stk->iter[level]++;
            __syncwarp();
          }
          else if (level > 0) level--;
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

    for (int i = 0;i < dev_graph->nnodes; i++) {
      //printf("%d ", dev_graph->vertex_label[i]);
    }
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