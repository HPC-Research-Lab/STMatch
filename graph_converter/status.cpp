#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include "def.h"

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "usage: ./convert <snap file>\n";
    return 1;
  }
  std::string fname = argv[1];
  const auto raw_edge = MemoryMappedFile<vidType>::Read(fname + ".raw.edge.bin");
  uint64_t nonzero = 0;
  for(uint64_t e = 0; e < raw_edge.size(); e++) {
    if(e % 1000000 == 0) {
      LOG("%lu edges processed, %lu, (%f%%) nonzero",
          e, nonzero, (100 * nonzero / (double)e));
    }
    nonzero += !!raw_edge[e];
  }
  double n_edges = raw_edge.size();
  std::cout << 100 * nonzero / n_edges << "% nonzero\n";
  return 0;
}

