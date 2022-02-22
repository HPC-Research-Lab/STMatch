#pragma once

#include "GraphPreprocessor.h"
#include "PatternPreprocessor.h"
#include "CallStack.h"
#include "JobQueuePreprocessor.h"

namespace libra {
  __global__ void _parallel_match(Graph* dev_graph, Pattern* dev_pattern, CallStack* dev_callstack, JobQueue* job_queue);
}