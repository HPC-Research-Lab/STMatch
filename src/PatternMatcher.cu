#include "PatternMatcher.cuh"


namespace libra {

  __device__ void _extend(PatternMatcher* pm) {

    if (pm->level == 0) {
      graph_node_t cur_job, njobs;

      // TODO: change to warp
      if (threadIdx.x % WARP_SIZE == 0) {
        pm->job_queue->get_job(cur_job, njobs);
        pm->stk.iter[0] = 0;
        for (size_t i = 0; i < njobs; i++) {
          pm->stk.slot_storage[0][0][i] = pm->job_queue->queue[cur_job + i];
        }
        pm->stk.slot_size[0][0] = njobs;
      }
      __syncwarp();
    }
    else {

      for (pattern_node_t i = 0; i < PAT_SIZE; i++) {

        pattern_node_t first_level = (pm->pattern->set_ops[pm->level][i] & 0x7F);
        pattern_node_t first_idx = (pm->pattern->set_ops[pm->level][i] & (0x7F << 7));
        pattern_node_t second_level = (pm->pattern->set_ops[pm->level][i] & (0x7F << 14));
        pattern_node_t second_idx = (pm->pattern->set_ops[pm->level][i] & (0x7F << 21));

        if (first_idx >= 64) {
          graph_node_t* neighbor = pm->graph->rowptr[pm->stk.path[first_level]] + pm->graph->colidx;
          graph_node_t neighbor_size = (graph_node_t)(pm->graph->rowptr[pm->stk.path[first_level - 1] + 1] - pm->graph->rowptr[pm->stk.path[first_level - 1]]);
          // TODO: change to warp 
          for (graph_node_t j = 0; j < neighbor_size; j++) {
            pm->stk.slot_storage[pm->level][i][j] = neighbor[j];
          }
          pm->stk.slot_size[pm->level][i] = neighbor_size;
        }
        else {
          // TODO: change to warp
          for (graph_node_t j = 0; j < pm->stk.slot_size[first_level][first_idx]; j++) {
            pm->stk.slot_storage[pm->level][i][j] = pm->stk.slot_storage[first_level][first_idx][j];
          }
          pm->stk.slot_size[pm->level][i] = pm->stk.slot_size[first_level][first_idx];
        }

        if (pm->pattern->set_ops[pm->level][i] < 0) break;

        // TODO: compute ub based on pattern->partial
        graph_node_t ub = 0;

        if (pm->pattern->set_ops[pm->level][i] & (1 << 28) == 0) {
          intersection(&(pm->stk.slot_storage[pm->level][i][0]), &(pm->stk.slot_storage[second_level][second_idx][0]), pm->stk.slot_size[pm->level][i], pm->stk.slot_size[second_level][second_idx], ub);
        }
        else {
          difference(&(pm->stk.slot_storage[pm->level][i][0]), &(pm->stk.slot_storage[second_level][second_idx][0]), pm->stk.slot_size[pm->level][i], pm->stk.slot_size[second_level][second_idx], ub);
        }
      }
    }
  }

  __device__ void match(PatternMatcher* pm) {
    pm->level = 0;

    while (true) {

      if (pm->level < pm->pattern->nnodes - 1) {
        if (pm->stk.slot_size[pm->level][0] == 0) {
          _extend(pm);
          pm->stk.iter[pm->level] = 0;
        }
        if (pm->stk.iter[pm->level] < pm->stk.slot_size[pm->level][0]) {
          pm->stk.path[pm->level] = pm->stk.slot_storage[pm->level][0][pm->stk.iter[pm->level]];
          if (threadIdx.x % WARP_SIZE == 0)
            pm->level++;
          __syncwarp();
        }
        else {
          pm->stk.slot_size[pm->level][0] = 0;
          if (threadIdx.x % WARP_SIZE == 0) {
            pm->level--;
          }
          __syncwarp();
          if (pm->level < 0) {
            break;
          }
          if (threadIdx.x % WARP_SIZE == 0) pm->stk.iter[pm->level]++;
          __syncwarp();
        }
      }
      else if (pm->level == pm->pattern->nnodes - 1) {
        // TODO: we can save the storage of sets for the last level
        if (threadIdx.x % WARP_SIZE == 0) {
          pm->count += pm->stk.slot_size[pm->level][0];
          pm->stk.slot_size[pm->level][0] = 0;
          pm->level--;
        }
        __syncwarp();
      }
    }

    if (threadIdx.x == 0) printf("count: %lu\n", pm->count);
  }


}