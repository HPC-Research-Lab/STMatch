#include <string>
#include <iostream>
#include <unistd.h>
#include <omp.h>
#include "src/gpu_match.cuh"

using namespace std;
using namespace STMatch;

void printfLocalProfile(ProfInfo& profile){

  double gridTotal = 0;
  for(int i=0; i<GRID_DIM; i++){
    double blockTotal = 0; 
    for(int j=0; j<BLOCK_DIM; j+=WARP_SIZE){
        blockTotal+=profile.clk[i][j];
    }
    gridTotal+=(blockTotal/NWARPS_PER_BLOCK);
  }
  //printf("%f\n", gridTotal/GRID_DIM);

  double memTotal = 0; 
  for(int i=0; i<GRID_DIM; i++){
    for(int j=0; j<NWARPS_PER_BLOCK; j++){
        memTotal+=profile.localMemStorage[i][j];
    }
  }
  printf("%f\n", memTotal/GRID_DIM);

}

void printfUtilization(ProfInfo& profile){
  uint64_t totalThreadUsed = 0;
  uint64_t busyThreadUsed = 0;
  for(int i=0; i<GRID_DIM; i++){
    for(int j=0; j<NWARPS_PER_BLOCK; j++){
      totalThreadUsed+=profile.totalThreadUsed[i][j];
      busyThreadUsed+=profile.busyThreadUsed[i][j];
    }
  }
   printf("%f\n", (double)busyThreadUsed/(double)totalThreadUsed);
}


void printfGlobalProfile(ProfInfo& profile){
  //------------------Global-----------
  double gridTotal = 0;
  for(int i=0; i<GRID_DIM; i++){
    long long int blockMax = 0; 
    for(int j=0; j<BLOCK_DIM; j+=WARP_SIZE){
       if(profile.clk[i][j]>blockMax) blockMax = profile.clk[i][j];
    }
    gridTotal+=blockMax;
  }
  //printf("%f\n", gridTotal/GRID_DIM);


  size_t storgeTotal = 0;
  size_t stkTotal = 0;
  for(int i=0; i<GRID_DIM; i++){
    for(int j=0; j<NWARPS_PER_BLOCK; j++){
      storgeTotal+=profile.globalMemStorage[i][j];
      stkTotal+=profile.globalMemStk[i][j];
    }
  }
  printf("%lu\t%lu\n", storgeTotal, stkTotal);

}


int main(int argc, char* argv[]) {
  STMatch::GraphPreprocessor g(argv[1]);
  STMatch::PatternPreprocessor p(argv[2]);


  //JobQueue* gpu_queue[NUM_GPU] = JobQueuePreprocessor(g.g, p).distributedQueue();
  Graph* gpu_graph[NUM_GPU];
  Pattern* gpu_pattern[NUM_GPU];
  JobQueue* gpu_queue[NUM_GPU];
  CallStack* gpu_callstack[NUM_GPU];
  graph_node_t* slot_storage[NUM_GPU];
  size_t* gpu_res[NUM_GPU];

  int* idle_warps[NUM_GPU];
  int* idle_warps_count[NUM_GPU];
  int* global_mutex[NUM_GPU];
  bool* stk_valid[NUM_GPU];

  for(int gpuIdx=0; gpuIdx<NUM_GPU; gpuIdx++){
    cudaSetDevice(gpuIdx);
    
    gpu_graph[gpuIdx] = g.to_gpu();
    gpu_pattern[gpuIdx] = p.to_gpu();
    gpu_queue[gpuIdx] = JobQueuePreprocessor(g.g, p).to_gpu(gpuIdx);
    cudaMalloc(&slot_storage[gpuIdx], sizeof(graph_node_t) *  NWARPS_TOTAL * MAX_SLOT_NUM * UNROLL * GRAPH_DEGREE);
    std::vector<CallStack> stk(NWARPS_TOTAL);
    for (int i = 0; i < NWARPS_TOTAL; i++) {
      auto& s = stk[i];
      memset(s.iter, 0, sizeof(s.iter));
      memset(s.slot_size, 0, sizeof(s.slot_size));
      s.slot_storage = (graph_node_t(*)[UNROLL][GRAPH_DEGREE])((char*)slot_storage[gpuIdx] + i * sizeof(graph_node_t) * MAX_SLOT_NUM * UNROLL * GRAPH_DEGREE);
    }
    cudaMalloc(&gpu_callstack[gpuIdx], NWARPS_TOTAL * sizeof(CallStack));
    cudaMemcpy(gpu_callstack[gpuIdx], stk.data(), sizeof(CallStack) * NWARPS_TOTAL, cudaMemcpyHostToDevice);

    cudaMalloc(&gpu_res[gpuIdx], sizeof(size_t) * NWARPS_TOTAL);
    cudaMemset(gpu_res[gpuIdx], 0, sizeof(size_t) * NWARPS_TOTAL);

    cudaMalloc(&idle_warps[gpuIdx], sizeof(int) * GRID_DIM);
    cudaMemset(idle_warps[gpuIdx], 0, sizeof(int) * GRID_DIM);

    cudaMalloc(&idle_warps_count[gpuIdx], sizeof(int));
    cudaMemset(idle_warps_count[gpuIdx], 0, sizeof(int));

    cudaMalloc(&global_mutex[gpuIdx], sizeof(int) * GRID_DIM);
    cudaMemset(global_mutex[gpuIdx], 0, sizeof(int) * GRID_DIM);

    cudaMalloc(&stk_valid[gpuIdx], sizeof(bool) * GRID_DIM);
    cudaMemset(stk_valid[gpuIdx], 0, sizeof(bool) * GRID_DIM);

  }
  
  size_t* res = new size_t[NWARPS_TOTAL];
  cudaEvent_t start[NUM_GPU], stop[NUM_GPU];
  float milliseconds[NUM_GPU];
  
//--------------------  
  #pragma omp parallel for num_threads(NUM_GPU)
  for(int i=0; i<NUM_GPU; i++) {
      cudaSetDevice(i);
      cudaEventCreate(&start[i]);
      cudaEventCreate(&stop[i]);
      cudaEventRecord(start[i]);
      _parallel_match << <GRID_DIM, BLOCK_DIM>> > (gpu_graph[i], gpu_pattern[i], gpu_callstack[i], gpu_queue[i], gpu_res[i], idle_warps[i], idle_warps_count[i], global_mutex[i], i);
      cudaEventRecord(stop[i]);
      cudaEventSynchronize(stop[i]);
      cudaEventElapsedTime(&milliseconds[i], start[i], stop[i]);
   }
   //printf("All Finished\n");



  float maxGPU = 0;
  uint64_t finalCount =0;
  for(int i=0; i<NUM_GPU; i++) {
    cudaMemcpy(res, gpu_res[i], sizeof(size_t) * NWARPS_TOTAL, cudaMemcpyDeviceToHost);
    size_t tot_count = 0;
    for (int j=0; j<NWARPS_TOTAL; j++) {
      tot_count += res[j];
    }
    if(milliseconds[i]>maxGPU) maxGPU = milliseconds[i];
    finalCount+=tot_count;

    //printf("%f\t", milliseconds[i]);
  }
  printf("%f\t%lu\n", maxGPU, finalCount);

  return 0;
}
