#pragma once

#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include "Constants.h"

namespace libra {

  struct CallStack {

    size_t path_[MAX_PAT_SIZE];
    size_t iter_[MAX_PAT_SIZE];
    size_t slot_size[MAX_PAT_SIZE][MAX_PAT_SIZE];
    size_t(*slot_storage)[MAX_PAT_SIZE][MAX_GRAPH_DEGREE];

    CallStack() {
      memset(path_, 0, sizeof(path_));
      memset(iter_, 0, sizeof(iter_));
      memset(slot_size, 0, sizeof(slot_size));
    }
  };
}