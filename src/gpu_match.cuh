#pragma once

#include "graph.h"
#include "pattern.h"
#include "callstack.h"
#include "job_queue.h"

namespace libra {
  __global__ void _parallel_match(Graph* dev_graph, Pattern* dev_pattern, 
                            CallStack* dev_callstack, JobQueue* job_queue,  size_t* res, 
                             bool** idle_block, int* idle_warps_count);
}