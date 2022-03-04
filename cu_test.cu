#include <string>
#include <iostream>
#include "src/gpu_match.cuh"

using namespace std;
using namespace libra;

int main(int argc, char* argv[]) {

  libra::GraphPreprocessor g(argv[1]);
  libra::PatternPreprocessor p(argv[2]);

  // copy graph and pattern to GPU global memory
  Graph* gpu_graph = g.to_gpu();
  Pattern* gpu_pattern = p.to_gpu();
  JobQueue* gpu_queue = JobQueuePreprocessor(g.g, p.pat).to_gpu();
  CallStack* gpu_callstack;

  // allocate the callstack for all warps in global memory
  graph_node_t* slot_storage;
  cudaMalloc(&slot_storage, sizeof(graph_node_t) * NWARPS_TOTAL * PAT_SIZE * PAT_SIZE * GRAPH_DEGREE);
  cout << "global memory usage: " << sizeof(graph_node_t) * NWARPS_TOTAL * PAT_SIZE * PAT_SIZE * GRAPH_DEGREE / 1024.0 / 1024 / 1024 << " GB" << endl;

  std::vector<CallStack> stk(NWARPS_TOTAL);

  for (int i = 0; i < NWARPS_TOTAL; i++) {
    auto& s = stk[i];
    memset(s.iter, 0, sizeof(s.iter));
    memset(s.slot_size, 0, sizeof(s.slot_size));
    s.slot_storage = (graph_node_t(*)[PAT_SIZE][GRAPH_DEGREE])((char*)slot_storage + i * sizeof(graph_node_t) * PAT_SIZE * PAT_SIZE * GRAPH_DEGREE);
  }
  cudaMalloc(&gpu_callstack, NWARPS_TOTAL * sizeof(CallStack));
  cudaMemcpy(gpu_callstack, stk.data(), sizeof(CallStack) * NWARPS_TOTAL, cudaMemcpyHostToDevice);

  size_t* gpu_res;
  cudaMalloc(&gpu_res, sizeof(size_t) * NWARPS_TOTAL);
  cudaMemset(gpu_res, 0, sizeof(size_t) * NWARPS_TOTAL);
  size_t* res = new size_t[NWARPS_TOTAL];


  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);

  cudaEventRecord(start);

  cout << "shared memory usage: " << sizeof(Graph) << " " << sizeof(Pattern) << " " << sizeof(JobQueue) << " " << sizeof(CallStack) * NWARPS_PER_BLOCK << " " << NWARPS_PER_BLOCK * 33 * sizeof(int) << " Bytes" << endl;

  _parallel_match << <GRID_DIM, BLOCK_DIM >> > (gpu_graph, gpu_pattern, gpu_callstack, gpu_queue, gpu_res);


  cudaEventRecord(stop);

  cudaEventSynchronize(stop);

  float milliseconds = 0;
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("matching time: %f ms\n", milliseconds);

  cudaMemcpy(res, gpu_res, sizeof(size_t) * NWARPS_TOTAL, cudaMemcpyDeviceToHost);

  size_t tot_count = 0;
  for (int i=0; i<NWARPS_TOTAL; i++) tot_count += res[i];

  cout << "count: " << tot_count << endl;
  return 0;
}
