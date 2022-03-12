#include "gpu_match.cuh"
#include <cuda.h>

// TODO: change this to gpu array so we can use different unroll for diffrent levels
#define UNROLL_SIZE(l) UNROLL

namespace libra {

  inline __device__ graph_node_t path(CallStack* stk, Pattern* pat, int level, int k) {
    return stk->slot_storage[pat->rowptr[level]][stk->uiter[level]][stk->iter[level] + k];
  }

  typedef struct {
    graph_node_t* set1[UNROLL], * set2[UNROLL], * res[UNROLL];
    graph_node_t set1_size[UNROLL], set2_size[UNROLL], * res_size[UNROLL];
    graph_node_t ub[UNROLL];
    bitarray32 label;
    Graph* g;
    int num_sets;
  } Arg_t;

  template<typename DATA_T, typename SIZE_T>
  __device__
    inline bool bsearch_exist(DATA_T* set2, SIZE_T set2_size, DATA_T target) {
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

  template<typename DATA_T, typename SIZE_T>
  inline __device__
    SIZE_T upper_bound(DATA_T* set2, SIZE_T set2_size, DATA_T target) {
    int i, step;
    int low = 0;
    while (set2_size > 0) {
      i = low;
      step = set2_size / 2;
      i += step;
      if (target > set2[i]) {
        low = ++i; set2_size -= step + 1;
      }
      else {
        set2_size = step;
      }
    }
    return low;
  }

  inline __device__
    void prefix_sum(int* _input, int input_size) {

    int thid = threadIdx.x % WARP_SIZE;
    int offset = 1;
    int last_element = _input[input_size - 1];
    // build sum in place up the tree
    for (int d = (WARP_SIZE >> 1); d > 0; d >>= 1) {
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
      if (thid < d) {
        int ai = offset * (2 * thid + 1) - 1;
        int bi = offset * (2 * thid + 2) - 1;
        int t = _input[ai];
        _input[ai] = _input[bi];
        _input[bi] += t;
      }
    }
    __syncwarp();

    if (thid >= input_size - 1)
      _input[thid + 1] = _input[input_size - 1] + last_element;
  }


  template<bool DIFF>
  __device__ void compute_set(Arg_t* arg) {

    __shared__ int pos[NWARPS_PER_BLOCK][WARP_SIZE + 1];
    __shared__ graph_node_t size_psum[NWARPS_PER_BLOCK][WARP_SIZE + 1];
    __shared__ int end_pos[NWARPS_PER_BLOCK][UNROLL];

    int wid = threadIdx.x / WARP_SIZE;
    int tid = threadIdx.x % WARP_SIZE;


    if (tid < arg->num_sets) {
      arg->set1_size[tid] = upper_bound(arg->set1[tid], arg->set1_size[tid], arg->ub[tid]);
      size_psum[wid][tid] = arg->set1_size[tid];
      end_pos[wid][tid] = 0;
    }
    else {
      size_psum[wid][tid] = 0;
    }
    __syncwarp();

    prefix_sum(&size_psum[wid][0], arg->num_sets);
    __syncwarp();


    bool still_loop = true;
    int slot_idx = 0;
    int offset = 0;

    int size1 = (size_psum[wid][WARP_SIZE] > 0) ? (((size_psum[wid][WARP_SIZE] - 1) / WARP_SIZE + 1) * WARP_SIZE) : 0;

    for (int idx = tid; (idx < size1 && still_loop); idx += WARP_SIZE) {
      pos[wid][tid] = 0;

      if (idx < size_psum[wid][WARP_SIZE]) {

        while (idx >= size_psum[wid][slot_idx + 1]) {
          slot_idx++;
        }
        offset = idx - size_psum[wid][slot_idx];

        bitarray32 lb = arg->g->vertex_label[arg->set1[slot_idx][offset]];
        if ((lb && arg->label == lb) && (DIFF ^ bsearch_exist(arg->set2[slot_idx], arg->set2_size[slot_idx], arg->set1[slot_idx][offset]))) {
          pos[wid][tid] = 1;
        }
      }
      else {
        slot_idx = arg->num_sets;
        still_loop = false;
      }

      still_loop = __shfl_sync(0xFFFFFFFF, still_loop, 31);

      prefix_sum(&pos[wid][0], WARP_SIZE);
      __syncwarp();

      graph_node_t res_tmp;
      if (pos[wid][tid + 1] > pos[wid][tid]) {
        res_tmp = arg->set1[slot_idx][offset];
      }
      __syncwarp();
      int prev_idx = ((idx / WARP_SIZE == size_psum[wid][slot_idx] / WARP_SIZE) ? size_psum[wid][slot_idx] % WARP_SIZE : 0);

      if (pos[wid][tid + 1] > pos[wid][tid]) {
        arg->res[slot_idx][end_pos[wid][slot_idx] + pos[wid][tid] - pos[wid][prev_idx]] = res_tmp;
      }

      if (slot_idx < __shfl_down_sync(0xFFFFFFFF, slot_idx, 1)) {
        end_pos[wid][slot_idx] += pos[wid][tid + 1] - pos[wid][prev_idx];
      }
      else if (tid == WARP_SIZE - 1 && slot_idx < arg->num_sets) {
        end_pos[wid][slot_idx] += pos[wid][WARP_SIZE] - pos[wid][prev_idx];
      }
    }
    __syncwarp();
    if (tid < arg->num_sets) {
      *(arg->res_size[tid]) = end_pos[wid][tid];
    }
    __syncwarp();
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
    __shared__ graph_node_t cur_job_njobs[NWARPS_PER_BLOCK][2];
    int wid = threadIdx.x / WARP_SIZE;

    if (level == 0) {

      int tid = threadIdx.x % WARP_SIZE;

      for (int k = 0; k < UNROLL_SIZE(level); k++) {
        if (threadIdx.x % WARP_SIZE == 0) {
          get_job(q, cur_job_njobs[wid][0], cur_job_njobs[wid][1]);
        }
        __syncwarp();
        if (tid < cur_job_njobs[wid][1]) {
          for (int j = 0; j < q->start_level; j++) {
            stk->slot_storage[0][k][tid + JOB_CHUNK_SIZE * j] = (q->q[cur_job_njobs[wid][0] + tid].nodes)[j];
          }
        }
        stk->slot_size[0][k] = cur_job_njobs[wid][1];
        stk->start_level = q->start_level;
      }
      __syncwarp();
    }
    else {

    arg[wid].g = g;
    arg[wid].num_sets = UNROLL_SIZE(level);

    int remaining = stk->slot_size[pat->rowptr[level - 1]][stk->uiter[level - 1]] - stk->iter[level - 1];
    if (remaining >= 0 && UNROLL_SIZE(level) > remaining) {
      arg[wid].num_sets = remaining;
    }

    for (int i = pat->rowptr[level]; i < pat->rowptr[level + 1]; i++) {

      // compute ub based on pattern->partial
      graph_node_t ub = INT_MAX;
      // assert(level >= 1);
      if (i == pat->rowptr[level]) {
        ub = INT_MAX;
        if (pat->partial[i] != 0) {
          for (pattern_node_t k = 0; k < level - 1; k++) {
            if ((pat->partial[i] & (1 << k)) && (ub > path(stk, pat, k, stk->uiter[k + 1]))) ub = path(stk, pat, k, stk->uiter[k + 1]);
          }

          for (pattern_node_t k = 0; k < arg[wid].num_sets; k++) {
            arg[wid].ub[k] = ub;
            if ((pat->partial[i] & (1 << (level - 1))) && (arg[wid].ub[k] > path(stk, pat, level - 1, k))) arg[wid].ub[k] = path(stk, pat, level - 1, k);
          }
        }
        else {
          for (pattern_node_t k = 0; k < arg[wid].num_sets; k++) {
            arg[wid].ub[k] = INT_MAX;
          }
        }
      }
      else {
        ub = -1;
        if (pat->partial[i] != 0) {
          for (pattern_node_t k = 0; k < level - 1; k++) {
            if ((pat->partial[i] & (1 << k)) && (ub < path(stk, pat, k, stk->uiter[k + 1]))) ub = path(stk, pat, k, stk->uiter[k + 1]);
          }
          for (pattern_node_t k = 0; k < arg[wid].num_sets; k++) {
            arg[wid].ub[k] = ub;
            if ((pat->partial[i] & (1 << (level - 1))) && (arg[wid].ub[k] < path(stk, pat, level - 1, k))) arg[wid].ub[k] = path(stk, pat, level - 1, k);
            if (arg[wid].ub[k] == -1) arg[wid].ub[k] = INT_MAX;
          }
        }
        else {
          for (pattern_node_t k = 0; k < arg[wid].num_sets; k++) {
            arg[wid].ub[k] = INT_MAX;
          }
        }
      }

      arg[wid].label = pat->slot_labels[i];

      if (pat->set_ops[i] & 0x20) {

        graph_node_t* nb = NULL;
        graph_node_t nsize = 0;

        if (!EDGE_INDUCED) {
          if (level >= 2) {
            nb = &g->colidx[g->rowptr[path(stk, pat, level - 2, stk->uiter[level - 1])]];
            nsize = (graph_node_t)(g->rowptr[path(stk, pat, level - 2, stk->uiter[level - 1]) + 1] - g->rowptr[path(stk, pat, level - 2, stk->uiter[level - 1])]);
          }
        }
        for (graph_node_t k = 0; k < arg[wid].num_sets; k++) {
          graph_node_t* neighbor = &g->colidx[g->rowptr[path(stk, pat, level - 1, k)]];
          graph_node_t neighbor_size = (graph_node_t)(g->rowptr[path(stk, pat, level - 1, k) + 1] - g->rowptr[path(stk, pat, level - 1, k)]);
          arg[wid].set1[k] = neighbor;
          arg[wid].set2[k] = nb;
          arg[wid].res[k] = &(stk->slot_storage[i][k][0]);
          arg[wid].set1_size[k] = neighbor_size;
          arg[wid].set2_size[k] = nsize;
          arg[wid].res_size[k] = &(stk->slot_size[i][k]);
        }
        compute_set<true>(&arg[wid]);
        for (graph_node_t k = arg[wid].num_sets; k < UNROLL_SIZE(level); k++) stk->slot_size[i][k] = 0;


        if (!EDGE_INDUCED) {
          for (pattern_node_t j = level - 3; j >= 0; j--) {

            nb = &g->colidx[g->rowptr[path(stk, pat, j, stk->uiter[j + 1])]];
            nsize = (graph_node_t)(g->rowptr[path(stk, pat, j, stk->uiter[j + 1]) + 1] - g->rowptr[path(stk, pat, j, stk->uiter[j + 1])]);

            for (graph_node_t k = 0; k < arg[wid].num_sets; k++) {
              arg[wid].set1[k] = &(stk->slot_storage[i][k][0]);
              arg[wid].set2[k] = nb;
              arg[wid].res[k] = &(stk->slot_storage[i][k][0]);
              arg[wid].set1_size[k] = stk->slot_size[i][k];
              arg[wid].set2_size[k] = nsize;
              arg[wid].res_size[k] = &(stk->slot_size[i][k]);
            }
            compute_set<true>(&arg[wid]);
            for (graph_node_t k = arg[wid].num_sets; k < UNROLL_SIZE(level); k++) stk->slot_size[i][k] = 0;
          }
        }
      }
      else {

        pattern_node_t slot_idx = (pat->set_ops[i] & 0x1F);

        if (pat->set_ops[i] & 0x40) {
          for (graph_node_t k = 0; k < arg[wid].num_sets; k++) {
            graph_node_t* neighbor = &g->colidx[g->rowptr[path(stk, pat, level - 1, k)]];
            graph_node_t neighbor_size = (graph_node_t)(g->rowptr[path(stk, pat, level - 1, k) + 1] - g->rowptr[path(stk, pat, level - 1, k)]);

            arg[wid].set1[k] = &(stk->slot_storage[slot_idx][stk->uiter[level - 1]][0]);
            arg[wid].set2[k] = neighbor;
            arg[wid].res[k] = &(stk->slot_storage[i][k][0]);
            arg[wid].set1_size[k] = stk->slot_size[slot_idx][stk->uiter[level - 1]];
            arg[wid].set2_size[k] = neighbor_size;
            arg[wid].res_size[k] = &(stk->slot_size[i][k]);
          }
          compute_set<false>(&arg[wid]);
          for (graph_node_t k = arg[wid].num_sets; k < UNROLL_SIZE(level); k++) stk->slot_size[i][k] = 0;

        }
        else {

          for (graph_node_t k = 0; k < arg[wid].num_sets; k++) {
            graph_node_t* neighbor = NULL;
            graph_node_t neighbor_size = 0;
            if (!EDGE_INDUCED) {
              neighbor = &g->colidx[g->rowptr[path(stk, pat, level - 1, k)]];
              neighbor_size = (graph_node_t)(g->rowptr[path(stk, pat, level - 1, k) + 1] - g->rowptr[path(stk, pat, level - 1, k)]);
            }
            arg[wid].set1[k] = &(stk->slot_storage[slot_idx][stk->uiter[level - 1]][0]);
            arg[wid].set2[k] = neighbor;
            arg[wid].res[k] = &(stk->slot_storage[i][k][0]);
            arg[wid].set1_size[k] = stk->slot_size[slot_idx][stk->uiter[level - 1]];
            arg[wid].set2_size[k] = neighbor_size;
            arg[wid].res_size[k] = &(stk->slot_size[i][k]);
          }
          compute_set<true>(&arg[wid]);
          for (graph_node_t k = arg[wid].num_sets; k < UNROLL_SIZE(level); k++) stk->slot_size[i][k] = 0;

        }
      }
    }
    }
    stk->iter[level] = 0;
    stk->uiter[level] = 0;
}

__device__ void match(Graph* g, Pattern* pat, CallStack* stk, JobQueue* q, size_t* count) {
  pattern_node_t level = 0;

  while (true) {

    if (level < pat->nnodes - 1) {

      if (stk->uiter[level] == 0 && stk->slot_size[pat->rowptr[level]][0] == 0) {
        extend(g, pat, stk, q, level);
        if (level == 0 && stk->slot_size[0][0] == 0) break;
      }

      if (stk->uiter[level] < UNROLL_SIZE(level)) {
        if (stk->iter[level] < stk->slot_size[pat->rowptr[level]][stk->uiter[level]]) {
          if (level > 0 && level < stk->start_level && path(stk, pat, level, 0) != stk->slot_storage[0][stk->uiter[0]][stk->iter[0] + stk->uiter[1] + level * JOB_CHUNK_SIZE]) {
            stk->iter[level]++;
          }
          else if (level > 1 && level < stk->start_level + 1 && path(stk, pat, level - 1, stk->uiter[level]) != stk->slot_storage[0][stk->uiter[0]][stk->iter[0] + stk->uiter[1] + (level - 1) * JOB_CHUNK_SIZE]) {
            stk->uiter[level]++;
          }
          else {
            level++;
          }
        }
        else {
          stk->slot_size[pat->rowptr[level]][stk->uiter[level]] = 0;
          stk->iter[level] = 0;
          stk->uiter[level]++;
        }
      }
      else {
        stk->uiter[level] = 0;
        if (level > 0) {
          level--;
          if (threadIdx.x % WARP_SIZE == 0) stk->iter[level] += UNROLL_SIZE(level + 1);
          __syncwarp();
        }
      }
    }
    else if (level == pat->nnodes - 1) {

      // TODO: we can save the storage of sets for the last level
      extend(g, pat, stk, q, level);
      for (int j = 0; j < UNROLL_SIZE(level); j++) {
        if (threadIdx.x % WARP_SIZE == 0) {
          *count += stk->slot_size[pat->rowptr[level]][j];
        }
        __syncwarp();
        stk->slot_size[pat->rowptr[level]][j] = 0;
      }
      level--;
      if (threadIdx.x % WARP_SIZE == 0) stk->iter[level] += UNROLL_SIZE(level + 1);
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
    //printf("%ld\n", stop - start);
  }
}
}