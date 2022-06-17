#include "gpu_match.cuh"
#include <cuda.h>

#define UNROLL_SIZE(l) (l > 0 ? UNROLL: 1) 

namespace STMatch {
  struct StealingArgs {
    int* idle_warps;
    int* idle_warps_count;
    int* global_mutex;
    int* local_mutex;
    CallStack* global_callstack;
  };

  __forceinline__ __device__ void lock(int* mutex) {
    while (atomicCAS((int*)mutex, 0, 1) != 0) {
    }
  }
  __forceinline__ __device__ void unlock(int* mutex) {
    atomicExch((int*)mutex, 0);
  }

  __device__ bool trans_layer(CallStack& _target_stk, CallStack& _cur_stk, Pattern* _pat, int _k, int ratio = 2) {
    if (_target_stk.level <= _k)
      return false;

    int num_left_task = _target_stk.slot_size[_pat->rowptr[_k]][_target_stk.uiter[_k]] -
      (_target_stk.iter[_k] + _target_stk.uiter[_k + 1] + 1);
    if (num_left_task <= 0)
      return false;

    int stealed_start_idx_in_target = _target_stk.iter[_k] + _target_stk.uiter[_k + 1] + 1 + num_left_task / ratio;

    _cur_stk.slot_storage[_pat->rowptr[0]][_target_stk.uiter[0]][_target_stk.iter[0] + _target_stk.uiter[1]] = _target_stk.slot_storage[_pat->rowptr[0]][_target_stk.uiter[0]][_target_stk.iter[0] + _target_stk.uiter[1]];
    _cur_stk.slot_storage[_pat->rowptr[0]][_target_stk.uiter[0]][_target_stk.iter[0] + _target_stk.uiter[1] + JOB_CHUNK_SIZE] = _target_stk.slot_storage[_pat->rowptr[0]][_target_stk.uiter[0]][_target_stk.iter[0] + _target_stk.uiter[1] + JOB_CHUNK_SIZE];

    for (int i = 1; i < _k; i++) {
      _cur_stk.slot_storage[_pat->rowptr[i]][_target_stk.uiter[i]][_target_stk.iter[i] + _target_stk.uiter[i + 1]] = _target_stk.slot_storage[_pat->rowptr[i]][_target_stk.uiter[i]][_target_stk.iter[i] + _target_stk.uiter[i + 1]];
    }

    for (int r = _pat->rowptr[_k]; r < _pat->rowptr[_k + 1]; r++) {
      for (int u = 0; u < UNROLL_SIZE(_k); u++) {
        int loop_end = _k == 0 ? JOB_CHUNK_SIZE * 2 : _target_stk.slot_size[r][u];
        for (int t = 0; t < loop_end; t++) {
          _cur_stk.slot_storage[r][u][t] = _target_stk.slot_storage[r][u][t];
        }
      }
    }

    for (int l = 0; l < _k; l++) {
      _cur_stk.iter[l] = _target_stk.iter[l];
      _cur_stk.uiter[l] = _target_stk.uiter[l];
      for (int s = _pat->rowptr[l]; s < _pat->rowptr[l + 1]; s++) {
        if (s > _pat->rowptr[l]) {
          for (int u = 0; u < UNROLL; u++) {
            _cur_stk.slot_size[s][u] = _target_stk.slot_size[s][u];
          }
        }
        else {
          for (int u = 0; u < UNROLL_SIZE(l); u++) {
            if (u == _cur_stk.uiter[l])
              _cur_stk.slot_size[_pat->rowptr[l]][u] = _target_stk.iter[l] + 1;
            else
              _cur_stk.slot_size[_pat->rowptr[l]][u] = 0;
          }
        }
      }
    }

    // copy
    for (int i = stealed_start_idx_in_target - _target_stk.iter[_k]; i < UNROLL_SIZE(_k + 1); i++) {
      _target_stk.slot_size[_pat->rowptr[_k + 1]][i] = 0;
    }

    for (int s = _pat->rowptr[_k]; s < _pat->rowptr[_k + 1]; s++) {
      if (s == _pat->rowptr[_k]) {
        for (int u = 0; u < UNROLL_SIZE(_k); u++) {
          if (u == _target_stk.uiter[_k])
            _cur_stk.slot_size[s][u] = _target_stk.slot_size[s][u];
          else
            _cur_stk.slot_size[s][u] = 0;
        }
      }
      else {
        for (int u = 0; u < UNROLL_SIZE(_k); u++) {
          _cur_stk.slot_size[s][u] = _target_stk.slot_size[s][u];
        }
      }
    }

    _cur_stk.uiter[_k] = _target_stk.uiter[_k];
    _cur_stk.iter[_k] = stealed_start_idx_in_target;
    _target_stk.slot_size[_pat->rowptr[_k]][_target_stk.uiter[_k]] = stealed_start_idx_in_target;
    // copy
    for (int l = _k + 1; l < _pat->nnodes - 1; l++) {
      _cur_stk.iter[l] = 0;
      _cur_stk.uiter[l] = 0;
      for (int s = _pat->rowptr[l]; s < _pat->rowptr[l + 1]; s++) {
        for (int u = 0; u < UNROLL_SIZE(l); u++) {
          _cur_stk.slot_size[s][u] = 0;
        }
      }
    }
    _cur_stk.iter[_pat->nnodes - 1] = 0;
    _cur_stk.uiter[_pat->nnodes - 1] = 0;
    for (int u = 0; u < UNROLL_SIZE(_pat->nnodes - 1); u++) {
      _cur_stk.slot_size[_pat->rowptr[_pat->nnodes - 1]][u] = 0;
    }
    _cur_stk.level = _k + 1;
    return true;
  }

  __device__ bool trans_skt(CallStack* _all_stk, CallStack* _cur_stk, Pattern* pat, StealingArgs* _stealing_args) {

    int max_left_task = 0;
    int stk_idx = -1;
    int at_level = -1;

    for (int level = 0; level < STOP_LEVEL; level++) {
      for (int i = 0; i < NWARPS_PER_BLOCK; i++) {

        if (i == threadIdx.x / WARP_SIZE)
          continue;
        lock(&(_stealing_args->local_mutex[i]));

        int left_task = _all_stk[i].slot_size[pat->rowptr[level]][_all_stk[i].uiter[level]] -
          (_all_stk[i].iter[level] + _all_stk[i].uiter[level + 1] + 1);
        if (left_task > max_left_task) {
          max_left_task = left_task;
          stk_idx = i;
          at_level = level;
        }
        unlock(&(_stealing_args->local_mutex[i]));
      }
      if (stk_idx != -1)
        break;
    }

    if (stk_idx != -1) {
      bool res;
      lock(&(_stealing_args->local_mutex[threadIdx.x / WARP_SIZE]));
      lock(&(_stealing_args->local_mutex[stk_idx]));
      res = trans_layer(_all_stk[stk_idx], *_cur_stk, pat, at_level);

      unlock(&(_stealing_args->local_mutex[threadIdx.x / WARP_SIZE]));
      unlock(&(_stealing_args->local_mutex[stk_idx]));
      return res;
    }
    return false;
  }


  __forceinline__ __device__ graph_node_t path(CallStack* stk, Pattern* pat, int level, int k) {
    if (level > 0)
      return stk->slot_storage[pat->rowptr[level]][stk->uiter[level]][stk->iter[level] + k];
    else {
      return stk->slot_storage[0][stk->uiter[0]][stk->iter[0] + k + (level + 1) * JOB_CHUNK_SIZE];
    }
  }

  __forceinline__ __device__ graph_node_t* path_address(CallStack* stk, Pattern* pat, int level, int k) {
    if (level > 0)
      return &(stk->slot_storage[pat->rowptr[level]][stk->uiter[level]][stk->iter[level] + k]);
    else {
      return &(stk->slot_storage[0][stk->uiter[0]][stk->iter[0] + k + (level + 1) * JOB_CHUNK_SIZE]);
    }
  }

  typedef struct {
    graph_node_t* set1[UNROLL], * set2[UNROLL], * res[UNROLL];
    graph_node_t set1_size[UNROLL], set2_size[UNROLL], * res_size[UNROLL];
    graph_node_t ub[UNROLL];
    bitarray32 label;
    Graph* g;
    int num_sets;
    bool cached;
    int level;
    Pattern* pat;
  } Arg_t;

  template<typename DATA_T, typename SIZE_T>
  __forceinline__ __device__
    bool bsearch_exist(DATA_T* set2, SIZE_T set2_size, DATA_T target) {
    if (set2_size <= 0) return false;
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
  __forceinline__ __device__
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

  __forceinline__ __device__
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
    int predicate;

    for (int idx = tid; (idx < ((size_psum[wid][WARP_SIZE] > 0) ? (((size_psum[wid][WARP_SIZE] - 1) / WARP_SIZE + 1) * WARP_SIZE) : 0) && still_loop); idx += WARP_SIZE) {
      predicate = 0;

      if (idx < size_psum[wid][WARP_SIZE]) {

        while (idx >= size_psum[wid][slot_idx + 1]) {
          slot_idx++;
        }
        offset = idx - size_psum[wid][slot_idx];

        bitarray32 lb = arg->g->vertex_label[arg->set1[slot_idx][offset]];

        predicate = ((lb & arg->label) == lb) && (DIFF ^ bsearch_exist(arg->set2[slot_idx], arg->set2_size[slot_idx], arg->set1[slot_idx][offset]));

      }
      else {
        slot_idx = arg->num_sets;
        still_loop = false;
      }

      still_loop = __shfl_sync(0xFFFFFFFF, still_loop, 31);
      predicate = __ballot_sync(0xFFFFFFFF, predicate);

      bool cond = (arg->level < arg->pat->nnodes - 2 && predicate & (1 << tid));
      graph_node_t res_tmp;
      if (cond) {
        res_tmp = arg->set1[slot_idx][offset];
      }

      int prev_idx = ((idx / WARP_SIZE == size_psum[wid][slot_idx] / WARP_SIZE) ? size_psum[wid][slot_idx] % WARP_SIZE : 0);

      if (cond) {
        arg->res[slot_idx][end_pos[wid][slot_idx] + __popc(predicate & ((1 << tid) - (1 << prev_idx)))] = res_tmp;
      }

      if (slot_idx < __shfl_down_sync(0xFFFFFFFF, slot_idx, 1)) {
        end_pos[wid][slot_idx] += __popc(predicate & ((1 << (tid + 1)) - (1 << prev_idx)));
      }
      else if (tid == WARP_SIZE - 1 && slot_idx < arg->num_sets) {
        end_pos[wid][slot_idx] += __popc(predicate & (0xFFFFFFFF - (1 << prev_idx) + 1));
      }
    }
    __syncwarp();
    if (tid < arg->num_sets) {
      *(arg->res_size[tid]) = end_pos[wid][tid];
    }
    __syncwarp();
  }

  __forceinline__ __device__ void get_job(JobQueue* q, graph_node_t& cur_pos, graph_node_t& njobs) {
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
      for (int k = 0; k < UNROLL_SIZE(level); k++) {
        if (threadIdx.x % WARP_SIZE == 0) {
          get_job(q, cur_job, njobs);

          for (size_t i = 0; i < njobs; i++) {
            for (int j = 0; j < 2; j++) {
              stk->slot_storage[0][k][i + JOB_CHUNK_SIZE * j] = (q->q[cur_job + i].nodes)[j];
            }
          }
          stk->slot_size[0][k] = njobs;
        }
        __syncwarp();
      }
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
        if (!LABELED) {
          graph_node_t ub = ((i == pat->rowptr[level]) ? INT_MAX : -1);
          if (pat->partial[i] != 0) {

            // compute ub with nodes after start_level until previous level
            for (pattern_node_t k = 1; k < level - 1; k++) {
              if ((pat->partial[i] & (1 << (k + 1))) && ((i == pat->rowptr[level]) ^ (ub < path(stk, pat, k, stk->uiter[k + 1])))) ub = path(stk, pat, k, stk->uiter[k + 1]);
            }
            // compute ub with nodes in the previous level
            for (pattern_node_t k = 0; k < arg[wid].num_sets; k++) {
              arg[wid].ub[k] = ub;
              int prev_level = (level > 1 ? 2 : 1);
              int prev_iter = (level > 1 ? stk->uiter[1] : k);
              // compute ub with the first few nodes before start_level
              for (pattern_node_t j = 0; j < prev_level; j++) {
                if ((pat->partial[i] & (1 << j)) && ((i == pat->rowptr[level]) ^ (arg[wid].ub[k] < path(stk, pat, j - 1, prev_iter)))) arg[wid].ub[k] = path(stk, pat, j - 1, prev_iter);
              }

              if ((pat->partial[i] & (1 << level)) && ((i == pat->rowptr[level]) ^ (arg[wid].ub[k] < path(stk, pat, level - 1, k)))) arg[wid].ub[k] = path(stk, pat, level - 1, k);
              if (arg[wid].ub[k] == -1) arg[wid].ub[k] = INT_MAX;
            }
          }
          else {
            for (pattern_node_t k = 0; k < arg[wid].num_sets; k++) {
              arg[wid].ub[k] = INT_MAX;
            }
          }
        }
        else {
          for (pattern_node_t k = 0; k < arg[wid].num_sets; k++) {
            arg[wid].ub[k] = INT_MAX;
          }
        }

        arg[wid].label = pat->slot_labels[i];

        if (pat->set_ops[i] & 0x20) {
          for (graph_node_t k = 0; k < arg[wid].num_sets; k++) {

            arg[wid].set2[k] = NULL;
            arg[wid].set2_size[k] = 0;
            if (!EDGE_INDUCED) {
              graph_node_t t = path(stk, pat, level - 2, ((level > 1) ? stk->uiter[level - 1] : k));
              arg[wid].set2[k] = &g->colidx[g->rowptr[t]];
              arg[wid].set2_size[k] = (graph_node_t)(g->rowptr[t + 1] - g->rowptr[t]);
            }
            graph_node_t t = path(stk, pat, level - 1, k);
            arg[wid].set1[k] = &g->colidx[g->rowptr[t]];
            arg[wid].res[k] = &(stk->slot_storage[i][k][0]);
            arg[wid].set1_size[k] = (graph_node_t)(g->rowptr[t + 1] - g->rowptr[t]);
            arg[wid].res_size[k] = &(stk->slot_size[i][k]);
          }
          // arg[wid].cached = (level > 1);
          arg[wid].level = level;
          arg[wid].pat = pat;
          compute_set<true>(&arg[wid]);

          if(EDGE_INDUCED && !LABELED){
            for (int j = level-1; j >= -1; j--)
            {
              int unrollIdx = threadIdx.x % WARP_SIZE;
              if(unrollIdx < arg[wid].num_sets)
              {
                if(level==1){
                  arg[wid].set2[unrollIdx] = path_address(stk, pat, j, unrollIdx);
                }
                else{
                  if(j==level-1){
                    arg[wid].set2[unrollIdx] = path_address(stk, pat, j, unrollIdx);
                    arg[wid].set2_size[unrollIdx] = 1;
                  }
                  else{
                    if(j>0){
                      arg[wid].set2[unrollIdx] = path_address(stk, pat, j, stk->uiter[j + 1]);
                    }
                    else{
                      arg[wid].set2[unrollIdx] = path_address(stk, pat, j, stk->uiter[1]);
                    }
                  }
                }
                arg[wid].set1[unrollIdx] = &(stk->slot_storage[i][unrollIdx][0]);
                arg[wid].res[unrollIdx] = &(stk->slot_storage[i][unrollIdx][0]);
                arg[wid].set1_size[unrollIdx] = stk->slot_size[i][unrollIdx];
                arg[wid].res_size[unrollIdx] = &(stk->slot_size[i][unrollIdx]);
                arg[wid].set2_size[unrollIdx] = 1;
                arg[wid].level = level;
                arg[wid].pat = pat;
              }
              __syncwarp();
              compute_set<true>(&arg[wid]);
            }
          }

          if (!EDGE_INDUCED) {
            for (pattern_node_t j = level - 3; j >= -1; j--) {
              graph_node_t t = path(stk, pat, j, stk->uiter[(j > 0 ? j + 1 : 1)]);

              for (graph_node_t k = 0; k < arg[wid].num_sets; k++) {
                arg[wid].set1[k] = &(stk->slot_storage[i][k][0]);
                arg[wid].set2[k] = &g->colidx[g->rowptr[t]];
                arg[wid].res[k] = &(stk->slot_storage[i][k][0]);
                arg[wid].set1_size[k] = stk->slot_size[i][k];
                arg[wid].set2_size[k] = (graph_node_t)(g->rowptr[t + 1] - g->rowptr[t]);
                arg[wid].res_size[k] = &(stk->slot_size[i][k]);
              }
              //arg[wid].cached = true;
              arg[wid].level = level;
              arg[wid].pat = pat;
              compute_set<true>(&arg[wid]);
            }
          }
          for (graph_node_t k = arg[wid].num_sets; k < UNROLL_SIZE(level); k++) stk->slot_size[i][k] = 0;
        }
        else {

          pattern_node_t slot_idx = (pat->set_ops[i] & 0x1F);

          if (pat->set_ops[i] & 0x40) { //INTE
            
            for (graph_node_t k = 0; k < arg[wid].num_sets; k++) {
              graph_node_t t = path(stk, pat, level - 1, k);
              graph_node_t* neighbor = &g->colidx[g->rowptr[t]];
              graph_node_t neighbor_size = (graph_node_t)(g->rowptr[t + 1] - g->rowptr[t]);

              if (level > 1) {
                arg[wid].set2[k] = &(stk->slot_storage[slot_idx][stk->uiter[level - 1]][0]);
                arg[wid].set2_size[k] = stk->slot_size[slot_idx][stk->uiter[level - 1]];
              }
              else {
                graph_node_t t = path(stk, pat, -1, k);
                arg[wid].set2[k] = &g->colidx[g->rowptr[t]];
                arg[wid].set2_size[k] = (graph_node_t)(g->rowptr[t + 1] - g->rowptr[t]);
              }

              arg[wid].set1[k] = neighbor;
              arg[wid].set1_size[k] = neighbor_size;
              arg[wid].res[k] = &(stk->slot_storage[i][k][0]);
              arg[wid].res_size[k] = &(stk->slot_size[i][k]);
            }
            //arg[wid].cached = (level > 1);
            arg[wid].level = level;
            arg[wid].pat = pat;
            compute_set<false>(&arg[wid]);
            for (graph_node_t k = arg[wid].num_sets; k < UNROLL_SIZE(level); k++) stk->slot_size[i][k] = 0;
          }
          else { //DIFF

            for (graph_node_t k = 0; k < arg[wid].num_sets; k++) {
              graph_node_t* neighbor = NULL;
              graph_node_t neighbor_size = 0;
              if(EDGE_INDUCED && !LABELED){
                neighbor = path_address(stk, pat, level - 1, k);
                neighbor_size = 1;
              }
              if (!EDGE_INDUCED) {
                graph_node_t t = path(stk, pat, level - 1, k);
                neighbor = &g->colidx[g->rowptr[t]];
                neighbor_size = (graph_node_t)(g->rowptr[t + 1] - g->rowptr[t]);
              }

              if (level > 1) {
                arg[wid].set1[k] = &(stk->slot_storage[slot_idx][stk->uiter[level - 1]][0]);
                arg[wid].set1_size[k] = stk->slot_size[slot_idx][stk->uiter[level - 1]];
              }
              else {
                graph_node_t t = path(stk, pat, -1, k);
                arg[wid].set1[k] = &g->colidx[g->rowptr[t]];
                arg[wid].set1_size[k] = (graph_node_t)(g->rowptr[t + 1] - g->rowptr[t]);
              }

              arg[wid].set2[k] = neighbor;
              arg[wid].set2_size[k] = neighbor_size;
              arg[wid].res[k] = &(stk->slot_storage[i][k][0]);
              arg[wid].res_size[k] = &(stk->slot_size[i][k]);
            }
            //arg[wid].cached = false;
            arg[wid].level = level;
            arg[wid].pat = pat;
            compute_set<true>(&arg[wid]);
            for (graph_node_t k = arg[wid].num_sets; k < UNROLL_SIZE(level); k++) stk->slot_size[i][k] = 0;

          }
        }
      }
    }
    stk->iter[level] = 0;
    stk->uiter[level] = 0;
  }

  __forceinline__ __device__ void respond_across_block(int level, CallStack* stk, Pattern* pat, StealingArgs* _stealing_args) {
    if (level > 0 && level <= DETECT_LEVEL) {
      if (threadIdx.x % WARP_SIZE == 0) {
        int at_level = -1;
        int left_task = 0;
        for (int l = 0; l < level; l++) {
          left_task = stk->slot_size[pat->rowptr[l]][stk->uiter[l]] - stk->iter[l] - stk->uiter[l + 1] - 1;
          if (left_task > 0) {
            at_level = l;
            break;
          }
        }
        if (at_level != -1) {
          for (int b = 0; b < GRID_DIM; b++) {
            if (b == blockIdx.x) continue;
            if (atomicCAS(&(_stealing_args->global_mutex[b]), 0, 1) == 0) {
              if (atomicAdd(&_stealing_args->idle_warps[b], 0) == 0xFFFFFFFF) {
                __threadfence();

                trans_layer(*stk, _stealing_args->global_callstack[b * NWARPS_PER_BLOCK], pat, at_level, INT_MAX);
                __threadfence();

                atomicSub(_stealing_args->idle_warps_count, NWARPS_PER_BLOCK);
                atomicExch(&_stealing_args->idle_warps[b], 0);

                atomicExch(&(_stealing_args->global_mutex[b]), 0);
                break;
              }
              atomicExch(&(_stealing_args->global_mutex[b]), 0);
            }
          }
        }
      }
      __syncwarp();
    }
  }

  __device__ void match(Graph* g, Pattern* pat,
    CallStack* stk, JobQueue* q, size_t* count, StealingArgs* _stealing_args) {

    pattern_node_t& level = stk->level;

    while (true) {
      if (threadIdx.x % WARP_SIZE == 0) {
        lock(&(_stealing_args->local_mutex[threadIdx.x / WARP_SIZE]));
      }
      __syncwarp();

      if (level < pat->nnodes - 2) {

        if (STEAL_ACROSS_BLOCK) {
          respond_across_block(level, stk, pat, _stealing_args);
        }

        if (stk->uiter[level] == 0 && stk->slot_size[pat->rowptr[level]][0] == 0) {

          extend(g, pat, stk, q, level);
          if (level == 0 && stk->slot_size[0][0] == 0) {
            if (threadIdx.x % WARP_SIZE == 0)
              unlock(&(_stealing_args->local_mutex[threadIdx.x / WARP_SIZE]));
            __syncwarp();
            break;
          }
        }
        if (stk->uiter[level] < UNROLL_SIZE(level)) {
          if (stk->iter[level] < stk->slot_size[pat->rowptr[level]][stk->uiter[level]]) {
            if (threadIdx.x % WARP_SIZE == 0)
              level++;
            __syncwarp();
          }
          else {
            stk->slot_size[pat->rowptr[level]][stk->uiter[level]] = 0;
            stk->iter[level] = 0;
            if (threadIdx.x % WARP_SIZE == 0)
              stk->uiter[level]++;
            __syncwarp();
          }
        }
        else {
          stk->uiter[level] = 0;
          if (level > 0) {
            if (threadIdx.x % WARP_SIZE == 0)
              level--;
            if (threadIdx.x % WARP_SIZE == 0)
              stk->iter[level] += UNROLL_SIZE(level + 1);
            __syncwarp();
          }
        }
      }
      else if (level == pat->nnodes - 2) {

        extend(g, pat, stk, q, level);
        for (int j = 0; j < UNROLL_SIZE(level); j++) {
          if (threadIdx.x % WARP_SIZE == 0) {
            *count += stk->slot_size[pat->rowptr[level]][j];
          }
          __syncwarp();
          stk->slot_size[pat->rowptr[level]][j] = 0;
        }
        stk->uiter[level] = 0;
        if (threadIdx.x % WARP_SIZE == 0)
          level--;
        if (threadIdx.x % WARP_SIZE == 0)
          stk->iter[level] += UNROLL_SIZE(level + 1);
        __syncwarp();
      }
      //__syncwarp();
      if (threadIdx.x % WARP_SIZE == 0)
        unlock(&(_stealing_args->local_mutex[threadIdx.x / WARP_SIZE]));
      __syncwarp();
    }
  }



  __global__ void _parallel_match(Graph* dev_graph, Pattern* dev_pattern,
    CallStack* dev_callstack, JobQueue* job_queue, size_t* res,
    int* idle_warps, int* idle_warps_count, int* global_mutex) {
    __shared__ Graph graph;
    __shared__ Pattern pat;
    __shared__ CallStack stk[NWARPS_PER_BLOCK];
    __shared__ size_t count[NWARPS_PER_BLOCK];
    __shared__ bool stealed[NWARPS_PER_BLOCK];
    __shared__ int mutex_this_block[NWARPS_PER_BLOCK];

    __shared__ StealingArgs stealing_args;
    stealing_args.idle_warps = idle_warps;
    stealing_args.idle_warps_count = idle_warps_count;
    stealing_args.global_mutex = global_mutex;
    stealing_args.local_mutex = mutex_this_block;
    stealing_args.global_callstack = dev_callstack;

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
      match(&graph, &pat, &stk[local_wid], job_queue, &count[local_wid], &stealing_args);
      __syncwarp();

      stealed[local_wid] = false;

      if (STEAL_IN_BLOCK) {

        if (threadIdx.x % WARP_SIZE == 0) {
          stealed[local_wid] = trans_skt(stk, &stk[local_wid], &pat, &stealing_args);
        }
        __syncwarp();
      }

      if (STEAL_ACROSS_BLOCK) {
        if (!stealed[local_wid]) {

          __syncthreads();

          if (threadIdx.x % WARP_SIZE == 0) {

            atomicAdd(stealing_args.idle_warps_count, 1);

            lock(&(stealing_args.global_mutex[blockIdx.x]));

            atomicOr(&stealing_args.idle_warps[blockIdx.x], (1 << local_wid));

            unlock(&(stealing_args.global_mutex[blockIdx.x]));

            while ((atomicAdd(stealing_args.idle_warps_count, 0) < NWARPS_TOTAL) && (atomicAdd(&stealing_args.idle_warps[blockIdx.x], 0) & (1 << local_wid)));

            if (atomicAdd(stealing_args.idle_warps_count, 0) < NWARPS_TOTAL) {

              __threadfence();
              if (local_wid == 0) {
                stk[local_wid] = (stealing_args.global_callstack[blockIdx.x * NWARPS_PER_BLOCK]);
              }
              stealed[local_wid] = true;
            }
            else {
              stealed[local_wid] = false;
            }
          }
          __syncthreads();
        }
      }

      if (!stealed[local_wid]) {
        break;
      }
    }

    auto stop = clock64();

    if (threadIdx.x % WARP_SIZE == 0) {
      res[global_wid] = count[local_wid];
      // printf("%d\t%ld\t%d\t%d\n", blockIdx.x, stop - start, stealed[local_wid], local_wid);
      //printf("%ld\n", stop - start);
    }

    // if(threadIdx.x % WARP_SIZE == 0)
    //   printf("%d\t%d\t%d\n", blockIdx.x, local_wid, mutex_this_block[local_wid]);
  }
}
