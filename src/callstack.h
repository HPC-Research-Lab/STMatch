#pragma once

#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include "config.h"

namespace libra {

  typedef struct {

    graph_node_t path[PAT_SIZE];
    graph_node_t iter[PAT_SIZE];
    graph_node_t slot_size[PAT_SIZE][PAT_SIZE];
    graph_node_t(*slot_storage)[PAT_SIZE][GRAPH_DEGREE];
    pattern_node_t level;
    int start_level;
  } CallStack;

/*
  void init() {
    memset(path, 0, sizeof(path));
    memset(iter, 0, sizeof(iter));
    memset(slot_size, 0, sizeof(slot_size));
  }
  */
}