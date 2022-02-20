#include "KernelLauncher.cuh"
#include <stdio.h>

namespace libra {

#ifdef __CUDACC__

  __global__ void _parallel_match(Graph* dev_graph, Pattern* dev_pattern, CallStack* dev_callstack, JobQueue* job_queue) {

    extern __shared__ char graph_pat_pm_mem[];

    // cache the graph, pattern, and callstack on shared memory
    Graph* graph = (Graph*)graph_pat_pm_mem;
    Pattern* pat = (Pattern*)(graph_pat_pm_mem + sizeof(Graph));
    PatternMatcher* pm = (PatternMatcher*)(graph_pat_pm_mem + sizeof(Pattern) + sizeof(Graph));

    int global_tid = blockIdx.x * blockDim.x + threadIdx.x;
    int global_wid = global_tid / WARP_SIZE;
    int local_wid = threadIdx.x / WARP_SIZE;

    // TODO: change this to multiple threads
    // graph and pattern are shared among all threads in a threadblock
    if (threadIdx.x == 0) {
      *graph = *dev_graph;
      *pat = *dev_pattern;
    }


    // TODO: change this to multiple threads
    // callstack is shared among all threads in each warp
    if (threadIdx.x % WARP_SIZE == 0) {
      pm[local_wid].pattern_ = pat;
      pm[local_wid].graph_ = graph;
      pm[local_wid].stk_ = dev_callstack[global_wid];
      pm[local_wid].job_queue_ = job_queue;
    }




    while (true) {
      pm[local_wid].match();
      break;
      // TODO: load balance
    }
  }

  __device__ void lower_bound(size_t* nums, size_t& nums_size, size_t target) {
    ssize_t mid;
    ssize_t low = 0;
    ssize_t high = nums_size;

    while (low < high) {
      mid = low + (high - low) / 2;

      if (target <= nums[mid]) {
        high = mid;
      }
      else {
        low = mid + 1;
      }
    }
    if (low < nums_size && nums[low] < target) {
      low++;
    }
    nums_size = low;
  }


  __device__ bool _lower_bound_exist(size_t* _arr, size_t _size, size_t _target) {
    ssize_t mid;
    ssize_t low = 0;
    ssize_t high = _size;
    while (low < high) {
      mid = low + (high - low) / 2;
      if (_target <= _arr[mid]) {
        high = mid;
      }
      else {
        low = mid + 1;
      }
    }
    if (low < _size && _arr[low] < _target) {
      low++;
    }
    return (low < _size&& _arr[low] == _target);
  }


  __device__ void _prefix_sum(int* _input) {

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

  __device__ size_t intersection(size_t* _first, size_t* _second, size_t _first_size, size_t _second_size, size_t* _res, size_t _up) {
    __shared__ int pos[32];
    int tid = threadIdx.x;
    int end_pos = 0;
    int loop_end = _first_size - _first_size % warpSize + warpSize;
    bool last_find;
    for (int idx = tid; idx < loop_end && _first[idx] < _up; idx += warpSize) {
      last_find = false;
      pos[tid] = 0;
      if (idx < _first_size && _first[idx] < _up) {
        if (_lower_bound_exist(_second, _second_size, _first[idx])) {
          pos[tid] = 1;
        }
        if (threadIdx.x == warpSize - 1 && pos[warpSize - 1] == 1) {
          last_find = true;
        }
      }
      _prefix_sum(pos);

      if ((tid == warpSize - 1 && last_find) ||
        tid != warpSize - 1 && pos[tid] < pos[tid + 1]) {
        _res[end_pos + pos[tid]] = _first[idx];
      }
      end_pos += pos[warpSize - 1];
    }
    return end_pos;
  }

  __device__ size_t difference(size_t* _first, size_t* _second, size_t _first_size, size_t _second_size, size_t* _res, size_t _up) {
    __shared__ int pos[32];
    int tid = threadIdx.x;
    int end_pos = 0;
    int loop_end = _first_size - _first_size % warpSize + warpSize;
    bool last_find;
    for (int idx = tid; idx < loop_end; idx += warpSize) {
      last_find = false;
      pos[tid] = 0;

      if (idx < _first_size && _first[idx] < _up) {
        if (!_lower_bound_exist(_second, _second_size, _first[idx])) {
          pos[tid] = 1;
        }
        if (threadIdx.x == warpSize - 1 && pos[warpSize - 1] == 1) {
          last_find = true;
        }
      }
      _prefix_sum(pos);

      if ((tid == warpSize - 1 && last_find) ||
        tid != warpSize - 1 && pos[tid] < pos[tid + 1]) {
        _res[end_pos + pos[tid]] = _first[idx];
      }
      end_pos += pos[warpSize - 1];
    }
    return end_pos;
  }

#else

  size_t intersection(size_t* first, size_t* second, size_t first_size, size_t second_size, size_t* res, size_t up) {
    size_t pi = 0, pj = 0, pos = 0;
    while (pi != first_size && pj != second_size) {
      if (first[pi] >= up) break;
      if (second[pj] >= up) break;
      if (first[pi] < second[pj])
        pi++;
      else if (first[pi] > second[pj])
        pj++;
      else {
        res[pos++] = first[pi];
        pi++;
        pj++;
      }
    }
    return pos;
  }


  inline int difference(size_t* first, size_t* second, int first_size, int second_size, size_t* res, size_t up) {
    int pi = 0, pj = 0, pos = 0;
    while (pi != first_size && pj != second_size) {
      int left = first[pi]; int right = second[pj];
      if (left >= up) break;
      if (right >= up) break;
      if (left <= right) pi++;
      if (right <= left) pj++;
      if (left < right) {
        res[pos++] = left;
      }
    }
    while (pi < first_size) {
      int left = first[pi++];
      if (left >= up) break;
      res[pos++] = left;
    }
    return pos;
  }

  int difference_ns(size_t* first, size_t* second, int first_size, int second_size, size_t* res, size_t up) {
    int pi = 0, pj = 0, pos = 0;
    while (pi != first_size && pj != second_size) {
      if (first[pi] >= up) break;
      if (second[pj] >= up) break;
      if (first[pi] < second[pj]) {
        //res[pos] = first[pi];
        pi++;
        pos++;
      }
      else {
        if (first[pi] == second[pj]) {
          ++pi;
        }
        ++pj;
      }
    }
    for (int i = pi; i < first_size; i++) {
      if (first[i] >= up) break;
      pos++;
      //res[pos++]=first[i];
    }
    return pos;

    return pos;
  }

  size_t lower_bound(size_t* nums, size_t nums_size, size_t target) {
    size_t mid;
    size_t low = 0;
    size_t high = nums_size;

    while (low < high) {
      mid = low + (high - low) / 2;

      if (target <= nums[mid]) {
        high = mid;
      }
      else {
        low = mid + 1;
      }
    }
    if (low < nums_size && nums[low] < target) {
      low++;
    }
    return low;
  }

#endif

}