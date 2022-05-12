#include <string>
#include <iostream>
#include "src/gpu_match.cuh"

using namespace std;
using namespace STMatch;

void printfProfile(ProfInfo& profile){
  for(int i=0; i<1; i++){
    long long int max = 0;
    long long int total = 0; 
    for(int j=0; j<BLOCK_DIM; j+=WARP_SIZE){
      total+= profile.clk[i][j];
      if(profile.clk[i][j]>max) max = profile.clk[i][j];
      //printf("%lld\n", profile.clk[i][j]);
    }
    printf("clk:\t%f\t%lld\t", (double)total/(double)NWARPS_PER_BLOCK, max);
  }

  for(int i=0; i<1; i++){
    size_t total = 0;
    for(int j=0; j<NWARPS_PER_BLOCK; j++){
      total+=profile.localMemStorage[i][j];
      //printf("%lld\n", profile.clk[i][j]);
    }
    printf("%lu\t", total);
  }



  uint64_t totalThreadUsed = 0;
  uint64_t busyThreadUsed = 0;
  for(int i=0; i<GRID_DIM; i++){
    for(int j=0; j<NWARPS_PER_BLOCK; j++){
      totalThreadUsed+=profile.totalThreadUsed[i][j];
      busyThreadUsed+=profile.busyThreadUsed[i][j];
    }
  }
   printf("%f\n", (double)busyThreadUsed/(double)totalThreadUsed);

  //------------------Global-----------

  double gridTotal = 0;
  double gridMax = 0;
  for(int i=0; i<GRID_DIM; i++){
    long long int blockTotal = 0; 
    for(int j=0; j<BLOCK_DIM; j+=WARP_SIZE){
        blockTotal+= profile.clk[i][j];
    }
    double blockAvg = (double)blockTotal/NWARPS_PER_BLOCK;

    gridTotal+=blockAvg;
    if(blockAvg>gridMax) gridMax = blockAvg;
  }
  double gridAvg = gridTotal/GRID_DIM;
  //printf("%f\t%f\n", gridAvg, gridMax);


}


int main(int argc, char* argv[]) {

  cudaSetDevice(0);

  STMatch::GraphPreprocessor g(argv[1]);
  STMatch::PatternPreprocessor p(argv[2]);

  // copy graph and pattern to GPU global memory
  Graph* gpu_graph = g.to_gpu();
  Pattern* gpu_pattern = p.to_gpu();
  JobQueue* gpu_queue = JobQueuePreprocessor(g.g, p).to_gpu();
  CallStack* gpu_callstack;

  // allocate the callstack for all warps in global memory
  graph_node_t* slot_storage;
  cudaMalloc(&slot_storage, sizeof(graph_node_t) * NWARPS_TOTAL * MAX_SLOT_NUM * UNROLL * GRAPH_DEGREE);
  //cout << "global memory usage: " << sizeof(graph_node_t) * NWARPS_TOTAL * MAX_SLOT_NUM * UNROLL * GRAPH_DEGREE / 1024.0 / 1024 / 1024 << " GB" << endl;

  std::vector<CallStack> stk(NWARPS_TOTAL);

  for (int i = 0; i < NWARPS_TOTAL; i++) {
    auto& s = stk[i];
    memset(s.iter, 0, sizeof(s.iter));
    memset(s.slot_size, 0, sizeof(s.slot_size));
    s.slot_storage = (graph_node_t(*)[UNROLL][GRAPH_DEGREE])((char*)slot_storage + i * sizeof(graph_node_t) * MAX_SLOT_NUM * UNROLL * GRAPH_DEGREE);
  }
  cudaMalloc(&gpu_callstack, NWARPS_TOTAL * sizeof(CallStack));
  cudaMemcpy(gpu_callstack, stk.data(), sizeof(CallStack) * NWARPS_TOTAL, cudaMemcpyHostToDevice);

  size_t* gpu_res;
  cudaMalloc(&gpu_res, sizeof(size_t) * NWARPS_TOTAL);
  cudaMemset(gpu_res, 0, sizeof(size_t) * NWARPS_TOTAL);
  size_t* res = new size_t[NWARPS_TOTAL];

  int* idle_warps;
  cudaMalloc(&idle_warps, sizeof(int) * GRID_DIM);
  cudaMemset(idle_warps, 0, sizeof(int) * GRID_DIM);

  int* idle_warps_count;
  cudaMalloc(&idle_warps_count, sizeof(int));
  cudaMemset(idle_warps_count, 0, sizeof(int));

  int* global_mutex;
  cudaMalloc(&global_mutex, sizeof(int) * GRID_DIM);
  cudaMemset(global_mutex, 0, sizeof(int) * GRID_DIM);

  bool* stk_valid;
  cudaMalloc(&stk_valid, sizeof(bool) * GRID_DIM);
  cudaMemset(stk_valid, 0, sizeof(bool) * GRID_DIM);

  ProfInfo* prof_info;
  cudaMalloc(&prof_info, sizeof(ProfInfo));
  cudaMemset(prof_info, 0, sizeof(ProfInfo));


  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);

  cudaEventRecord(start);

  //cout << "shared memory usage: " << sizeof(Graph) << " " << sizeof(Pattern) << " " << sizeof(JobQueue) << " " << sizeof(CallStack) * NWARPS_PER_BLOCK << " " << NWARPS_PER_BLOCK * 33 * sizeof(int) << " Bytes" << endl;

  _parallel_match << <GRID_DIM, BLOCK_DIM >> > (gpu_graph, gpu_pattern, gpu_callstack, gpu_queue, gpu_res, idle_warps, idle_warps_count, global_mutex, prof_info);


  cudaEventRecord(stop);

  cudaEventSynchronize(stop);

  float milliseconds = 0;
  cudaEventElapsedTime(&milliseconds, start, stop);
  //printf("matching time: %f ms\n", milliseconds);

  cudaMemcpy(res, gpu_res, sizeof(size_t) * NWARPS_TOTAL, cudaMemcpyDeviceToHost);


  ProfInfo cpuProfInfo;
  cudaMemcpy(&cpuProfInfo, prof_info, sizeof(ProfInfo), cudaMemcpyDeviceToHost);

  printfProfile(cpuProfInfo);

  size_t tot_count = 0;
  for (int i=0; i<NWARPS_TOTAL; i++) tot_count += res[i];
  //printf("%s\t%f\t%lu\n", argv[2], milliseconds, tot_count);
  //cout << "count: " << tot_count << endl;
  return 0;
}
